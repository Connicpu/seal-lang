#include "lexer.h"
#include <iostream>
#include <vector>

int main()
{
    std::string_view input =
        "struct Test {\n"
        "    stuff: char,\n"
        "    id: symbol,\n"
        "}\n"
        "\n"
        "let x = Test { stuff: '\\u{1F98A}', id: 'test_sym };\n";

    std::cout << input << std::endl;

    std::vector<lexer::LexerToken> tokens;
    lexer::LexerToken token;
    do
    {
        lexer::next_token(input, token);

        std::cout << lexer::TOKEN_NAMES[token.token.raw] << ": " << token.text << std::endl;
        if (token.error)
        {
            std::cout << "^ Token has problems: " << token.error->message << std::endl;
        }

        tokens.push_back(token);

    } while (token.text.size());

    std::cout << "End of stream" << std::endl;
    std::cin.get();
}
