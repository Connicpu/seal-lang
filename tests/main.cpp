#include "lexer.h"
#include <iostream>
#include <vector>

int main()
{
	std::string_view input =
		"struct Test {\n"
		"    field: i32,\n"
		"}\n";

	std::cout << input << std::endl;

	std::vector<lexer::LexerToken> tokens;
	lexer::LexerToken token;
	do
	{
		lexer::next_token(input, token);

		// Ignore whitespace
		if (token.token.type != lexer::TokenType::Whitespace)
		{
			tokens.push_back(token);
		}

	} while (token.text.size());

	std::cout << "End of stream" << std::endl;
	std::cin.get();
}
