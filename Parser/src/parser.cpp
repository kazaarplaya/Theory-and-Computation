#include "parser.h"

std::string mapToToken(const std::string& category, const std::string& lexeme) {
    if (category == "KEYWORD") {
        if (lexeme == "int") return "INT";
        if (lexeme == "char") return "CHAR";
        if (lexeme == "class") return "CLASS";
        if (lexeme == "public") return "PUBLIC";
        if (lexeme == "private") return "PRIVATE";
        if (lexeme == "protected") return "PROTECTED";
        if (lexeme == "void") return "VOID";
        if (lexeme == "return") return "RETURN";
        if (lexeme == "if") return "IF";
        if (lexeme == "else") return "ELSE";
        if (lexeme == "while") return "WHILE";
        if (lexeme == "for") return "FOR";
        if (lexeme == "do") return "DO";
        if (lexeme == "new") return "NEW";
        if (lexeme == "delete") return "DELETE";
    }

    if (category == "IDENTIFIER") return "IDENTIFIER";
    if (category == "INTEGER") return "INTEGER";
    if (category == "CHARACTER") return "CHARACTER";

    if (category == "OPERATOR") {
        if (lexeme == "=") return "=";
        if (lexeme == "+") return "+";
        if (lexeme == "-") return "-";
        if (lexeme == "*") return "*";
        if (lexeme == "/") return "/";
        if (lexeme == "%") return "%";
        if (lexeme == "++") return "++";
        if (lexeme == "--") return "--";
    }

    if (category == "DELIMITER") {
        if (lexeme == ";") return ";";
        if (lexeme == ",") return ",";
        if (lexeme == "(") return "(";
        if (lexeme == ")") return ")";
        if (lexeme == "{") return "{";
        if (lexeme == "}") return "}";
        if (lexeme == "[") return "[";
        if (lexeme == "]") return "]";
    }

    if (category == "EOF") return "$end";

    return "UNKNOWN";
}

int extractNumberAfterEquals(const std::string& text) {
    size_t pos = text.find('=');
    if (pos == std::string::npos) {
        return -1;
    }

    return std::stoi(text.substr(pos + 1));
}