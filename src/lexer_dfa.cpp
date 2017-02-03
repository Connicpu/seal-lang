#include "lexer_dfa.h"
#include <assert.h>

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

void lexer::DfaNode::Transition(DfaNode *node)
{
	default_edge = node;
}

void lexer::DfaNode::Transition(DfaNode *node, char c, bool peek)
{
	ByteMatch match;
	match.peek = peek;
	match.match = c;
	match.node = node;
	byte_edges.insert_or_assign(c, match);
}

void lexer::DfaNode::Transition(DfaNode *node, char begin, char end, bool peek)
{
	assert(end >= begin);
	ByteRangeMatch match;
	match.peek = peek;
	match.begin = begin;
	match.end = end;
	match.node = node;
	byte_range_edges.push_back(match);
}

void lexer::DfaNode::Transition(DfaNode *node, utf::unicode_classifier classifier, bool peek)
{
	UnicodeMatch match;
	match.peek = peek;
	match.classifier = classifier;
	match.node = node;
	unicode_edges.push_back(match);
}

auto lexer::DfaNode::TryTransition(std::string_view &input) const -> std::tuple<bool, size_t, DfaNode *>
{
	// First check for a byte-based edge
	auto byte_i = byte_edges.find(input[0]);
	if (byte_i != byte_edges.end())
	{
		auto &match = byte_i->second;
		auto advance = match.peek ? 0 : 1;
		input = input.substr(advance);
		return { !!match.node, advance, match.node };
	}

	// Check for a byte range match
	for (auto &range : byte_range_edges)
	{
		if (input[0] >= range.begin && input[0] <= range.end)
		{
			auto advance = range.peek ? 0 : 1;
			input = input.substr(advance);
			return { !!range.node, advance, range.node };
		}
	}

	// Check unicode characters
	const std::string_view original = input;
	utf::codepoint c;
	size_t count = utf::next_char(input, c);
	if (count == 0)
	{
		return { false, 0, nullptr };
	}

	for (auto &match : unicode_edges)
	{
		if (match.classifier(c))
		{
			if (match.peek)
			{
				input = original;
				return { !!match.node, 0, match.node };
			}
			else
			{
				return { !!match.node, count, match.node };
			}
		}
	}

	if (default_edge)
	{
		return { true, count, default_edge };
	}

	input = original;
	return { false, 0, nullptr };
}

