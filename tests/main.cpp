#include "lexer.h"
#include <iostream>

int main()
{
	std::string_view input = u8"[Hi, there, 😍 + 👱🏼, 0, 1, 01, 0x1, 0b1000_0010, 1.0, 2.0f, 1.6e10, 1.2e-5]";

	lexer::LexerToken token;
	do
	{
		lexer::next_token(input, token);

		// Ignore whitespace
		if (token.token.type != lexer::TokenType::Whitespace)
		{
			std::cout << lexer::TOKEN_NAMES[token.token.raw] << ": \"" << token.text << '"' << std::endl;
		}

	} while (token.text.size());

	std::cout << "End of stream" << std::endl;
	std::cin.get();
}
