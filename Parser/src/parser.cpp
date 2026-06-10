#include "parser.h"

TokenType mapToToken(const std::string& category, const std::string& lexeme) {
    if (category == "KEYWORD") {
        if (lexeme == "int") return TokenType::Int;
        if (lexeme == "char") return TokenType::Char;
        if (lexeme == "class") return TokenType::Class;
        if (lexeme == "public") return TokenType::Public;
        if (lexeme == "private") return TokenType::Private;
        if (lexeme == "protected") return TokenType::Protected;
        if (lexeme == "void") return TokenType::Void;
        if (lexeme == "return") return TokenType::Return;
        if (lexeme == "if") return TokenType::If;
        if (lexeme == "else") return TokenType::Else;
        if (lexeme == "while") return TokenType::While;
        if (lexeme == "for") return TokenType::For;
        if (lexeme == "do") return TokenType::Do;
        if (lexeme == "new") return TokenType::New;
        if (lexeme == "delete") return TokenType::Delete;
    }

    if (category == "IDENTIFIER") return TokenType::Identifier;
    if (category == "INTEGER") return TokenType::Integer;
    if (category == "CHARACTER") return TokenType::Character;

    if (category == "OPERATOR") {
        if (lexeme == "=") return TokenType::Assign;
        if (lexeme == "+") return TokenType::Plus;
        if (lexeme == "-") return TokenType::Minus;
        if (lexeme == "*") return TokenType::Star;
        if (lexeme == "/") return TokenType::Slash;
        if (lexeme == "%") return TokenType::Percent;
        if (lexeme == "++") return TokenType::PlusPlus;
        if (lexeme == "--") return TokenType::MinusMinus;
    }

    if (category == "DELIMITER") {
        if (lexeme == ";") return TokenType::Semicolon;
        if (lexeme == ",") return TokenType::Comma;
        if (lexeme == "(") return TokenType::LeftParen;
        if (lexeme == ")") return TokenType::RightParen;
        if (lexeme == "{") return TokenType::LeftBrace;
        if (lexeme == "}") return TokenType::RightBrace;
        if (lexeme == "[") return TokenType::LeftBracket;
        if (lexeme == "]") return TokenType::RightBracket;
    }

    if (category == "EOF") return TokenType::EndOfInput;

    return TokenType::Unknown;
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Class: return "CLASS";
        case TokenType::Public: return "PUBLIC";
        case TokenType::Private: return "PRIVATE";
        case TokenType::Protected: return "PROTECTED";
        case TokenType::Int: return "INT";
        case TokenType::Char: return "CHAR";
        case TokenType::Void: return "VOID";
        case TokenType::Identifier: return "IDENTIFIER";
        case TokenType::Integer: return "INTEGER";
        case TokenType::Character: return "CHARACTER";
        case TokenType::Return: return "RETURN";
        case TokenType::If: return "IF";
        case TokenType::Else: return "ELSE";
        case TokenType::While: return "WHILE";
        case TokenType::For: return "FOR";
        case TokenType::Do: return "DO";
        case TokenType::New: return "NEW";
        case TokenType::Delete: return "DELETE";
        case TokenType::LeftBrace: return "{";
        case TokenType::RightBrace: return "}";
        case TokenType::LeftParen: return "(";
        case TokenType::RightParen: return ")";
        case TokenType::LeftBracket: return "[";
        case TokenType::RightBracket: return "]";
        case TokenType::Semicolon: return ";";
        case TokenType::Comma: return ",";
        case TokenType::Assign: return "=";
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Percent: return "%";
        case TokenType::PlusPlus: return "++";
        case TokenType::MinusMinus: return "--";
        case TokenType::EndOfInput: return "$end";
        case TokenType::Unknown: return "UNKNOWN";
    }

    return "UNKNOWN";
}

int extractNumberAfterEquals(const std::string& text) {
    size_t pos = text.find('=');
    if (pos == std::string::npos) {
        return -1;
    }

    return std::stoi(text.substr(pos + 1));
}
