#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>

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
    EndOfInput,
    Unknown
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

TokenType mapToToken(const std::string& category, const std::string& lexeme);
std::string tokenTypeToString(TokenType type);
int extractNumberAfterEquals(const std::string& text);

#endif
