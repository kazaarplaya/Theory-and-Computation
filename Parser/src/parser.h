#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>

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

struct ParseNode {
    std::string name;
    std::string lexeme;
    std::vector<ParseNode> children;

    ParseNode(std::string name, std::string lexeme = "")
        : name(name), lexeme(lexeme) {}
};

enum class ActionType {
    SHIFT,
    REDUCE,
    ACCEPT,
    ERROR
};

struct Action {
    ActionType action;
    int value;

    Action() : action(ActionType::ERROR), value(-1) {}
    Action(ActionType type, int value): action(type), value(value) {}
};

class Rule {
    public:
        Rule(std::string lhs, std::vector<std::string> rhs);

        const std::string& getLHS() const {
            return lhs;
        }

        const std::vector<std::string>& getRHS() const {
            return rhs;
        }

    private:
        std::string lhs;
        std::vector<std::string> rhs;
};

class Parser {
    public:
        Parser(const std::vector<Token>& tokens);
        void parse();
        void printTree(const ParseNode& node, std::ostream& out, int indent = 0) const;

    private:
        const std::vector<Token> tokens;
        std::vector<Rule> grammarRules;
        std::set<std::string> terminals;
        std::set<std::string> nonTerminals;

        std::map<std::pair<int, TokenType>, Action> actionTable;
        std::map<std::pair<int, std::string>, int> gotoTable;
        std::map<int, Action> defaultActions;

        void buildGrammar();
        void addGrammarRule(const std::string& lhs, const std::vector<std::string>& rhs);
        void buildParsingTable();
        Action getAction(int state, TokenType type);
};

TokenType mapToToken(const std::string& category, const std::string& lexeme);
std::string tokenTypeToString(TokenType type);
int extractNumberAfterEquals(const std::string& text);

#endif
