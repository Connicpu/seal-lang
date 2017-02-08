#include "lexer.h"
#include "utf8.h"
#include "lexer_dfa.h"
#include <string>

std::string_view lexer::TOKEN_NAMES[(size_t)TokenType::TokenCount] =
{
#define TOKEN(name, _) #name,
#include "tokens.inl"
#undef TOKEN
};

using keyword_entry = std::pair<std::string_view, lexer::TokenType>;
static const keyword_entry KEYWORDS[] =
{
#define TOKEN(name, str) { str, lexer::TokenType::name },
#include "tokens_keywords.inl"
#undef TOKEN
};

int keyword_cmp(const void *vkey, const void *ventry)
{
    auto &key = *(const std::string_view *)vkey;
    auto &value = *(const keyword_entry *)ventry;

    if (key < value.first)
        return -1;
    if (key > value.first)
        return 1;
    return 0;
}

void lexer::next_token(std::string_view &input, LexerToken &token)
{
    if (input.size() == 0)
    {
        token.token.type = TokenType::Whitespace;
        token.text = "";
        token.error = {};
        return;
    }

    static Dfa dfa;
    const auto original = input;

    const DfaNode *node = &dfa.root;
    size_t state = 0;
    std::pair<size_t, const DfaNode *> last_accepting = { 0, node };
    DfaError tok_err = {}; size_t err_pos = 0;

    while (input.size() > 0 && node)
    {
        bool success; size_t count; const DfaNode *next_node; DfaError err;
        std::tie(success, count, next_node, err) = node->TryTransition(input);
        if (!success)
        {
            break;
        }

        if (err && !tok_err)
        {
            tok_err = err;
            err_pos = state;
        }

        state += count;
        if (next_node->token.raw != 0)
        {
            last_accepting = { state, next_node };
        }

        node = next_node;
    }

    if (tok_err)
    {
        token.error = TokenError{ err_pos, *tok_err };
    }
    else
    {
        token.error = {};
    }

    if (last_accepting.second->token.raw)
    {
        input = original.substr(last_accepting.first);
        token.text = original.substr(0, last_accepting.first);
        token.token = last_accepting.second->token;

        if (token.token.type == TokenType::Identifier)
        {
            auto elem_size = sizeof(keyword_entry);
            auto count = sizeof(KEYWORDS) / sizeof(keyword_entry);
            if (auto entry = std::bsearch(&token.text, KEYWORDS, count, elem_size, keyword_cmp))
            {
                token.token.type = ((const keyword_entry *)entry)->second;
            }
        }
    }
    else
    {
        input = original.substr(1);
        token.text = original.substr(0, 1);
        token.token.type = TokenType::Invalid;
    }
}
