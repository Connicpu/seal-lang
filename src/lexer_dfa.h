#pragma once

#include <vector>
#include <unordered_map>
#include "lexer.h"
#include "unicode_categories.h"

namespace lexer
{
    struct DfaNode;

    using DfaError = std::optional<std::string_view>;

    struct UnicodeMatch
    {
        DfaError error_token;
        utf::unicode_classifier classifier;
        DfaNode *node;
    };

    struct ByteMatch
    {
        DfaError error_token;
        char match;
        DfaNode *node;
    };

    struct ByteRangeMatch
    {
        DfaError error_token;
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

        void Transition(DfaNode *node, DfaError err = {});
        void Transition(DfaNode *node, char c, DfaError err = {});
        void Transition(DfaNode *node, char begin, char end, DfaError err = {});
        void Transition(DfaNode *node, utf::unicode_classifier classifier, DfaError err = {});

        auto TryTransition(std::string_view &input) const->std::tuple<bool, size_t, DfaNode *, DfaError>;

        TokenAccepting token;

        std::unordered_map<char, ByteMatch> byte_edges;
        std::vector<ByteRangeMatch> byte_range_edges;
        std::vector<UnicodeMatch> unicode_edges;
        DfaNode *default_edge;
        DfaError default_error;
    };

    struct Dfa
    {
        Dfa();
        ~Dfa();

        DfaNode root;
    };
}
