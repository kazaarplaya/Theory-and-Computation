#include "parser.h"

#include <string>
#include <vector>
#include <set>

class Rule {
    public:
        Rule(std::string lhs, std::vector<std::string> rhs);

        const std::string& getLHS() const {
            return lhs;
        };

        const std::vector<std::string>& getRHS() const {
            return rhs;
        };

    private:
        std::string lhs;
        std::vector<std::string> rhs;
};

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

class Parser {
    public:
        Parser(const std::vector<Token> tokens);

    private:
        const std::vector<Token> tokens;
        std::vector<Rule> grammarRules;
        std::set<std::string> terminals;
        std::set<std::string> nonTerminals;

        void buildGrammar(){
            addGrammarRule("S'", {"Program"});
            addGrammarRule("Program", {"ClassDeclarationList"});
            addGrammarRule("ClassDeclarationList", {"ClassDeclaration", "ClassDeclarationList"});
            addGrammarRule("ClassDeclarationList", {});
            addGrammarRule("ClassDeclaration", {"class", "identifier", "{", "MemberList", "}", ";"});
            addGrammarRule("MemberList", {"MemberDeclaration", "MemberList"});
            addGrammarRule("MemberList", {});
            addGrammarRule("MemberDeclaration", {"Visibility", "VarDeclaration"});
            addGrammarRule("MemberDeclaration", {"Visibility", "FunctionDeclaration"});
            addGrammarRule("MemberDeclaration", {"Visibility", "ClassDeclaration"});
            addGrammarRule("Visibility", {"public"});
            addGrammarRule("Visibility", {"private"});
            addGrammarRule("Visibility", {"protected"});
            addGrammarRule("Type", {"int"});
            addGrammarRule("Type", {"char"});
            addGrammarRule("Type", {"identifier"});
            addGrammarRule("VarDeclaration", {"Type", "identifier", ";"});
            addGrammarRule("VarDeclaration", {"Type", "identifier", "=", "Expression", ";"});
            addGrammarRule("VarDeclaration", {"Type", "identifier", "=", "NewExpression", ";"});
            addGrammarRule("FunctionDeclaration", {"Type", "identifier", "(", "FunctionParameters", ")", "Block"});
            addGrammarRule("FunctionDeclaration", {"void", "identifier", "(", "FunctionParameters", ")", "Block"});
            addGrammarRule("FunctionParameters", {"Parameter", "ParameterListTail"});
            addGrammarRule("FunctionParameters", {});
            addGrammarRule("ParameterListTail", {",", "Parameter", "ParameterListTail"});
            addGrammarRule("ParameterListTail", {});
            addGrammarRule("Parameter", {"Type", "identifier"});
            addGrammarRule("Block", {"{", "StatementList", "}"});
            addGrammarRule("StatementList", {"Statement", "StatementList"});
            addGrammarRule("StatementList", {});
            addGrammarRule("Statement", {"VarDeclaration"});
            addGrammarRule("Statement", {"Assignment", ";"});
            addGrammarRule("Statement", {"ReturnStatement"});
            addGrammarRule("Statement", {"IfStatement"});
            addGrammarRule("Statement", {"WhileStatement"});
            addGrammarRule("Statement", {"ForStatement"});
            addGrammarRule("Statement", {"DoStatement"});
            addGrammarRule("Statement", {"DeleteStatement", ";"});
            addGrammarRule("Statement", {"Block"});
            addGrammarRule("Assignment", {"identifier", "=", "Expression"});
            addGrammarRule("Assignment", {"identifier", "=", "NewExpression"});
            addGrammarRule("ReturnStatement", {"return", ";"});
            addGrammarRule("ReturnStatement", {"return", "Expression", ";"});
            addGrammarRule("IfStatement", {"if", "(", "Expression", ")", "Block"});
            addGrammarRule("IfStatement", {"if", "(", "Expression", ")", "Block", "else", "Block"});
            addGrammarRule("WhileStatement", {"while", "(", "Expression", ")", "Block"});
            addGrammarRule("ForStatement", {"for", "(", "ForInitialisation", ";", "Expression", ";", "ForUpdate", ")", "Block"});
            addGrammarRule("ForInitialisation", {});
            addGrammarRule("ForInitialisation", {"Assignment"});
            addGrammarRule("ForInitialisation", {"Type", "identifier", "=", "Expression"});
            addGrammarRule("ForInitialisation", {"Type", "identifier"});
            addGrammarRule("ForUpdate", {});
            addGrammarRule("ForUpdate", {"Assignment"});
            addGrammarRule("ForUpdate", {"identifier", "++"});
            addGrammarRule("ForUpdate", {"identifier", "--"});
            addGrammarRule("DoStatement", {"do", "Block", "while", "(", "Expression", ")", ";"});
            addGrammarRule("NewExpression", {"new", "Type"});
            addGrammarRule("NewExpression", {"new", "Type", "[", "Expression", "]"});
            addGrammarRule("DeleteStatement", {"delete", "identifier"});
            addGrammarRule("DeleteStatement", {"delete", "[", "]", "identifier"});
            addGrammarRule("Expression", {"AdditiveExpression"});
            addGrammarRule("AdditiveExpression", {"Term", "AdditiveTail"});
            addGrammarRule("AdditiveTail", {"+", "Term", "AdditiveTail"});
            addGrammarRule("AdditiveTail", {"-", "Term", "AdditiveTail"});
            addGrammarRule("AdditiveTail", {});
            addGrammarRule("Term", {"Factor", "TermTail"});
            addGrammarRule("TermTail", {"*", "Factor", "TermTail"});
            addGrammarRule("TermTail", {"/", "Factor", "TermTail"});
            addGrammarRule("TermTail", {"%", "Factor", "TermTail"});
            addGrammarRule("TermTail", {});
            addGrammarRule("Factor", {"identifier"});
            addGrammarRule("Factor", {"integer"});
            addGrammarRule("Factor", {"character"});
            addGrammarRule("Factor", {"(", "Expression", ")"});

            // insert terminals
            for (const Rule& rule : grammarRules) {
                for (const std::string& symbol : rule.getRHS()) {
                    // If symbol is not a non-terminal, it is a terminal
                    if (nonTerminals.count(symbol) == 0) {
                        terminals.insert(symbol);
                    }
                }
            }
            
            // add end terminal
            terminals.insert("$");
        }

        void addGrammarRule(const std::string& lhs, const std::vector<std::string>& rhs){
            Rule rule(lhs, rhs);
            
            // add to rules and add lhs to nonterminals
            grammarRules.push_back(rule);
            nonTerminals.insert(lhs);
        }
};