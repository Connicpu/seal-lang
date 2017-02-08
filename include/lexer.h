#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>

namespace lexer
{
    enum class TokenType : int32_t
    {
#define TOKEN(name, _) name,
#include "tokens.inl"
#undef TOKEN
        TokenCount
    };

    extern std::string_view TOKEN_NAMES[(size_t)TokenType::TokenCount];

    union TokenAccepting
    {
        TokenType type;
        int32_t raw;
    };

    struct TokenError
    {
        size_t location;
        std::string_view message;
    };

    struct LexerToken
    {
        TokenAccepting token;
        std::string_view text;
        std::optional<TokenError> error;
    };

    void next_token(std::string_view &input, LexerToken &token);
}
