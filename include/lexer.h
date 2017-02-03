#pragma once

#include <string_view>
#include <stdint.h>

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

	struct LexerToken
	{
		TokenAccepting token;
		std::string_view text;
	};

	void next_token(std::string_view &input, LexerToken &token);
}
