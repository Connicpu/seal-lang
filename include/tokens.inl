TOKEN(Invalid, "Invalid")

TOKEN(Identifier, "Identifier")
TOKEN(IntegerLiteral, "IntegerLiteral")
TOKEN(FloatLiteral, "FloatLiteral")
TOKEN(StringLiteral, "StringLiteral")
TOKEN(CharacterLiteral, "CharacterLiteral")
TOKEN(Whitespace, "Whitespace")
TOKEN(SingleLineComment, "SingleLineComment")
TOKEN(MultiLineComment, "MultiLineComment")
TOKEN(Label, "Label")

// Every value below this value aligns with the list of symbol names / values provided in TokenSymbols.txt (remember the +1)
TOKEN(SymbolStart, "SymbolStart")
#include "tokens_operators.inl"

// Every value below this value aligns with the list of keyword names / values provided in TokenKeywords.txt (remember the +1)
TOKEN(KeywordStart, "KeywordStart")
#include "tokens_keywords.inl"
