#include "lexer.h"
#include <iostream>

int main()
{
	std::string_view input = u8"Hi there 😍";

	lexer::LexerToken token;
	do
	{
		lexer::next_token(input, token);
		std::cout << lexer::TOKEN_NAMES[token.token.raw] << ": \"" << token.text << '"' << std::endl;
	} while (token.text.size());

	std::cout << "End of stream" << std::endl;
	std::cin.get();
}
