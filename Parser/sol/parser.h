#ifndef SIMPLE_LR1_PARSER_H
#define SIMPLE_LR1_PARSER_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

enum class TokenType {
    Class,
    Public,
    Private,
    Protected,
    Int,
    Char,
    Void,
    Identifier,
    Integer,
    Character,
    Return,
    If,
    Else,
    While,
    For,
    Do,
    New,
    Delete,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    Semicolon,
    Comma,
    Assign,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    PlusPlus,
    MinusMinus,
    EndOfInput
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line = 0;
    int column = 0;
};

struct ParseNode {
    std::string name;
    std::string lexeme;
    std::vector<std::unique_ptr<ParseNode>> children;

    ParseNode(std::string nodeName, std::string tokenLexeme = "");
};

struct ParseResult {
    bool success = false;
    std::unique_ptr<ParseNode> tree;
    std::string errorMessage;
};

class LR1Parser {
public:
    LR1Parser();

    ParseResult parse(const std::vector<Token>& lexerTokens) const;
    void printTree(const ParseNode& node, std::ostream& out, int indent = 0) const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl;
};

const char* tokenTypeName(TokenType type);
TokenType tokenTypeFromName(const std::string& name);

#endif
