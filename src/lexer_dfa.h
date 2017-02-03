#pragma once

#include <vector>
#include <unordered_map>
#include "lexer.h"
#include "unicode_categories.h"

namespace lexer
{
	struct DfaNode;

	struct UnicodeMatch
	{
		bool peek;
		utf::unicode_classifier classifier;
		DfaNode *node;
	};

	struct ByteMatch
	{
		bool peek;
		char match;
		DfaNode *node;
	};

	struct ByteRangeMatch
	{
		bool peek;
		char begin;
		char end;
		DfaNode *node;
	};

	struct DfaNode
	{
		DfaNode()
		{
			default_edge = nullptr;
			token.raw = 0;
		}

		DfaNode(TokenType tt)
		{
			default_edge = nullptr;
			token.type = tt;
		}

		void Transition(DfaNode *node);
		void Transition(DfaNode *node, char c, bool peek = false);
		void Transition(DfaNode *node, char begin, char end, bool peek = false);
		void Transition(DfaNode *node, utf::unicode_classifier classifier, bool peek = false);

		auto TryTransition(std::string_view &input) const -> std::tuple<bool, size_t, DfaNode *>;

		TokenAccepting token;

		std::unordered_map<char, ByteMatch> byte_edges;
		std::vector<ByteRangeMatch> byte_range_edges;
		std::vector<UnicodeMatch> unicode_edges;
		DfaNode *default_edge;
	};

	struct Dfa
	{
		Dfa();
		~Dfa();

		DfaNode root;
	};
}
