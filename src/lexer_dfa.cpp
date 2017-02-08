#include "lexer_dfa.h"
#include <assert.h>

static std::pair<const char *, lexer::TokenType> operator_list[] =
{
#define TOKEN(name, str) { str, lexer::TokenType::name },
#include "tokens_operators.inl"
#undef TOKEN
};

lexer::Dfa::Dfa()
{
    using namespace utf;

    /////////////////////////////////////////////////////////////////
    // Whitespace
    auto whitespace = new DfaNode(TokenType::Whitespace);
    root.Transition(whitespace, classifiers::whitespace);
    whitespace->Transition(whitespace, classifiers::whitespace);

    /////////////////////////////////////////////////////////////////
    // Identifiers
    auto identifier = new DfaNode(TokenType::Identifier);
    auto identifier_emb = new DfaNode(TokenType::Identifier);
    root.Transition(identifier_emb, classifiers::emoji_modifier_base);
    root.Transition(identifier, classifiers::emoji_presentation);
    root.Transition(identifier, classifiers::xid_start);
    identifier->Transition(identifier_emb, classifiers::emoji_modifier_base);
    identifier->Transition(identifier, classifiers::emoji_presentation);
    identifier->Transition(identifier, classifiers::xid_continue);
    identifier_emb->Transition(identifier_emb, classifiers::emoji_modifier_base);
    identifier_emb->Transition(identifier, classifiers::emoji_modifier);
    identifier_emb->Transition(identifier, classifiers::emoji_presentation);
    identifier_emb->Transition(identifier, classifiers::xid_continue);

    /////////////////////////////////////////////////////////////////
    // Integer literals
    auto int_literal = new DfaNode(TokenType::IntegerLiteral);
    auto int_separator = new DfaNode;

    auto zero_prefix = new DfaNode(TokenType::IntegerLiteral);

    auto hex_start = new DfaNode;
    auto hex_literal = new DfaNode(TokenType::HexLiteral);
    auto hex_separator = new DfaNode;

    auto oct_start = new DfaNode;
    auto oct_literal = new DfaNode(TokenType::OctalLiteral);
    auto oct_separator = new DfaNode;

    auto bin_start = new DfaNode;
    auto bin_literal = new DfaNode(TokenType::BinaryLiteral);
    auto bin_separator = new DfaNode;

    root.Transition(zero_prefix, '0');
    root.Transition(int_literal, '1', '9');

    int_literal->Transition(int_literal, '0', '9');
    int_literal->Transition(int_separator, '_');
    int_separator->Transition(int_literal, '0', '9');

    zero_prefix->Transition(int_literal, '0', '9');
    zero_prefix->Transition(hex_start, 'x');
    zero_prefix->Transition(oct_start, 'o');
    zero_prefix->Transition(bin_start, 'b');

    hex_start->Transition(hex_literal, '0', '9');
    hex_start->Transition(hex_literal, 'A', 'F');
    hex_start->Transition(hex_literal, 'a', 'f');
    hex_literal->Transition(hex_literal, '0', '9');
    hex_literal->Transition(hex_literal, 'A', 'F');
    hex_literal->Transition(hex_literal, 'a', 'f');
    hex_literal->Transition(hex_separator, '_');
    hex_separator->Transition(hex_literal, '0', '9');
    hex_separator->Transition(hex_literal, 'A', 'F');
    hex_separator->Transition(hex_literal, 'a', 'f');

    oct_start->Transition(oct_literal, '0', '7');
    oct_literal->Transition(oct_literal, '0', '7');
    oct_literal->Transition(oct_separator, '_');
    oct_separator->Transition(oct_literal, '0', '7');

    bin_start->Transition(bin_literal, '0', '1');
    bin_literal->Transition(bin_literal, '0', '1');
    bin_literal->Transition(bin_separator, '_');
    bin_separator->Transition(bin_literal, '0', '1');

    /////////////////////////////////////////////////////////////////
    // Float literals
    auto dotted_float = new DfaNode;
    auto float_literal = new DfaNode(TokenType::FloatLiteral);
    auto float_separator = new DfaNode;

    int_literal->Transition(dotted_float, '.');
    zero_prefix->Transition(dotted_float, '.');

    dotted_float->Transition(float_literal, '0', '9');
    float_literal->Transition(float_literal, '0', '9');
    float_literal->Transition(float_separator, '_');
    float_separator->Transition(float_literal, '0', '9');

    /////////////////////////////////////////////////////////////////
    // Scientific notation
    auto scientific_start = new DfaNode;
    auto signed_scientific = new DfaNode;
    auto scientific = new DfaNode(TokenType::FloatLiteral);

    float_literal->Transition(scientific_start, 'e');

    scientific_start->Transition(signed_scientific, '+');
    scientific_start->Transition(signed_scientific, '-');
    scientific_start->Transition(scientific, '0', '9');
    signed_scientific->Transition(scientific, '0', '9');
    scientific->Transition(scientific, '0', '9');

    /////////////////////////////////////////////////////////////////
    // Strings
    auto string_literal = new DfaNode(TokenType::StringLiteral);

    auto string_body = new DfaNode;
    auto string_escape = new DfaNode;

    auto string_u_begin = new DfaNode;
    auto string_u_value = new DfaNode;

    root.Transition(string_body, '"');

    string_body->Transition(string_body);
    string_body->Transition(string_literal, '"');
    string_body->Transition(string_escape, '\\');
    string_escape->Transition(string_body);
    string_escape->Transition(string_u_begin, 'u');

    string_u_begin->Transition(string_u_value, '{');
    string_u_value->Transition(string_u_value, '0', '9');
    string_u_value->Transition(string_u_value, 'A', 'F');
    string_u_value->Transition(string_u_value, 'a', 'f');
    string_u_value->Transition(string_body, '}');

    /////////////////////////////////////////////////////////////////
    // Char literals and labels. Fun with apostraphes!
    auto char_literal = new DfaNode(TokenType::CharacterLiteral);
    auto label1 = new DfaNode(TokenType::Label);
    auto label = new DfaNode(TokenType::Label);

    auto label_begin = new DfaNode;

    auto char_escape = new DfaNode;
    auto char_end = new DfaNode;

    auto char_u_begin = new DfaNode;
    auto char_u_value = new DfaNode;

    root.Transition(label_begin, '\'');

    label1->Transition(char_literal, '\'');
    label1->Transition(label, classifiers::xid_continue);
    label->Transition(label, classifiers::xid_continue);
    label->Transition(label, classifiers::not_whitespace, "Labels may only contain identifier characters");
    label->Transition(char_literal, '\'', "Character literals may only contain 1 character");

    label_begin->Transition(char_end);
    label_begin->Transition(label, classifiers::xid_start);
    label_begin->Transition(char_escape, '\\');

    char_escape->Transition(char_end);
    char_escape->Transition(char_u_begin, 'u');
    char_end->Transition(char_literal, '\'');
    char_end->Transition(char_literal, classifiers::not_whitespace, "Labels may only contain identifier characters");

    char_u_begin->Transition(char_u_value, '{');
    char_u_value->Transition(char_u_value, '0', '9');
    char_u_value->Transition(char_u_value, 'A', 'Z');
    char_u_value->Transition(char_u_value, 'a', 'z');
    char_u_value->Transition(char_end, '}');

    /////////////////////////////////////////////////////////////////
    // Operators
    for (auto &op : operator_list)
    {
        auto str = op.first;
        auto node = &root;

        while (str[0])
        {
            auto it = node->byte_edges.find(str[0]);
            if (it == node->byte_edges.end())
            {
                auto new_node = new DfaNode;
                node->Transition(new_node, str[0]);
                node = new_node;
            }
            else
            {
                node = it->second.node;
            }

            str = str + 1;
        }

        node->token.type = op.second;
    }

    return;
}

lexer::Dfa::~Dfa()
{
    static thread_local std::vector<DfaNode *> nodes;
    auto visit = [this](DfaNode *node, auto &visit)
    {
        if (node->token.raw == -1)
        {
            return;
        }

        if (node != &root)
        {
            nodes.push_back(node);
        }

        node->token.raw = -1;

        for (auto &pair : node->byte_edges)
        {
            if (pair.second.node)
            {
                visit(pair.second.node, visit);
            }
        }
        for (auto &edge : node->byte_range_edges)
        {
            if (edge.node)
            {
                visit(edge.node, visit);
            }
        }
        for (auto &edge : node->unicode_edges)
        {
            if (edge.node)
            {
                visit(edge.node, visit);
            }
        }
        if (node->default_edge)
        {
            visit(node->default_edge, visit);
        }
    };

    visit(&root, visit);

    for (auto &node : nodes)
    {
        delete node;
    }

    nodes.clear();
}

void lexer::DfaNode::Transition(DfaNode *node, DfaError err)
{
    default_edge = node;
    default_error = err;
}

void lexer::DfaNode::Transition(DfaNode *node, char c, DfaError err)
{
    ByteMatch match;
    match.error_token = err;
    match.match = c;
    match.node = node;
    byte_edges.insert_or_assign(c, match);
}

void lexer::DfaNode::Transition(DfaNode *node, char begin, char end, DfaError err)
{
    assert(end >= begin);
    ByteRangeMatch match;
    match.error_token = err;
    match.begin = begin;
    match.end = end;
    match.node = node;
    byte_range_edges.push_back(match);
}

void lexer::DfaNode::Transition(DfaNode *node, utf::unicode_classifier classifier, DfaError err)
{
    UnicodeMatch match;
    match.error_token = err;
    match.classifier = classifier;
    match.node = node;
    unicode_edges.push_back(match);
}

auto lexer::DfaNode::TryTransition(std::string_view &input) const -> std::tuple<bool, size_t, DfaNode *, DfaError>
{
    // First check for a byte-based edge
    auto byte_i = byte_edges.find(input[0]);
    if (byte_i != byte_edges.end())
    {
        auto &match = byte_i->second;
        input = input.substr(1);
        return { !!match.node, 1, match.node, match.error_token };
    }

    // Check for a byte range match
    for (auto &range : byte_range_edges)
    {
        if (input[0] >= range.begin && input[0] <= range.end)
        {
            input = input.substr(1);
            return { !!range.node, 1, range.node, range.error_token };
        }
    }

    // Check unicode characters
    const std::string_view original = input;
    utf::codepoint c;
    size_t count = utf::next_char(input, c);
    if (count == 0)
    {
        return { false, 0, nullptr, {} };
    }

    for (auto &match : unicode_edges)
    {
        if (match.classifier(c))
        {
            return { !!match.node, count, match.node, match.error_token };
        }
    }

    if (default_edge)
    {
        return { true, count, default_edge, default_error };
    }

    input = original;
    return { false, 0, nullptr, {} };
}

