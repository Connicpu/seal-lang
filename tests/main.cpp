#include "lexer.h"
#include <iostream>

int main()
{
	std::string_view input =
		"use std::io\n"
		"\n"
		"pub fn write_that()? {\n"
		"    let mut file = io::open(\"test.txt\", io::APPEND)?\n"
		"    file.write(\"hooplah\\n\")?\n"
		"}\n"
		"\n"
		"fn main() {\n"
		"    catch e = write_that() {\n"
		"        print(e)\n"
		"    }\n"
		"}\n";

	std::cout << input << std::endl;

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
