#include "lexer.h"
#include "utf8.h"
#include "lexer_dfa.h"
#include <string>

using namespace std::string_literals;

void lexer::next_token(std::string_view &input, LexerToken &token)
{
	if (input.size() == 0)
	{
		token.token.type = TokenType::Whitespace;
		token.text = "";
		return;
	}

	static Dfa dfa;
	const auto original = input;

	const DfaNode *node = &dfa.root;
	size_t state = 0;
	std::pair<size_t, const DfaNode *> last_accepting = { 0, node };

	while (input.size() > 0 && node)
	{
		bool success; size_t count; const DfaNode *next_node;
		std::tie(success, count, next_node) = node->TryTransition(input);
		if (!success)
		{
			break;
		}

		state += count;
		if (next_node->token.raw != 0)
		{
			last_accepting = { state, next_node };
		}

		node = next_node;
	}

	if (last_accepting.second->token.raw)
	{
		input = original.substr(last_accepting.first);
		token.text = original.substr(0, last_accepting.first);
		token.token = last_accepting.second->token;
	}
	else
	{
		input = original.substr(1);
		token.text = original.substr(0, 1);
		token.token.type = TokenType::Invalid;
	}
}

std::string_view lexer::TOKEN_NAMES[(size_t)TokenType::TokenCount] =
{
#define TOKEN(name, _) #name,
#include "tokens.inl"
#undef TOKEN
};
