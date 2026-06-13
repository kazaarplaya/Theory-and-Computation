#include "parser.h"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

Rule::Rule(std::string lhs, std::vector<std::string> rhs)
    : lhs(lhs), rhs(rhs) {}

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
        case TokenType::LeftBrace: return "LEFTBRACE";
        case TokenType::RightBrace: return "RIGHTBRACE";
        case TokenType::LeftParen: return "LEFTPAREN";
        case TokenType::RightParen: return "RIGHPAREN";
        case TokenType::LeftBracket: return "LEFTBRACKET";
        case TokenType::RightBracket: return "RIGHTBRACKET";
        case TokenType::Semicolon: return "SEMICOLON";
        case TokenType::Comma: return "COMMA";
        case TokenType::Assign: return "=";
        case TokenType::Plus: return "+";
        case TokenType::Minus: return "-";
        case TokenType::Star: return "*";
        case TokenType::Slash: return "/";
        case TokenType::Percent: return "%";
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

Parser::Parser(const std::vector<Token>& tokens): tokens(tokens) {
    buildGrammar();
    buildParsingTable();
};

void Parser::buildGrammar(){
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
        
void Parser::addGrammarRule(const std::string& lhs, const std::vector<std::string>& rhs){
    Rule rule(lhs, rhs);

    // Add to rule to grammarRules and add LHS to nonterminals
    grammarRules.push_back(rule);
    nonTerminals.insert(lhs);
}

void Parser::buildParsingTable(){
    actionTable[{0, TokenType::Class}] = Action(ActionType::SHIFT, 1);
    gotoTable[{0, "Program"}] = 2;
    gotoTable[{0, "ClassDeclarationList"}] = 3;
    gotoTable[{0, "ClassDeclaration"}] = 4;
    actionTable[{1, TokenType::Identifier}] = Action(ActionType::SHIFT, 5);
    actionTable[{2, TokenType::EndOfInput}] = Action(ActionType::SHIFT, 6);
    actionTable[{4, TokenType::Class}] = Action(ActionType::SHIFT, 1);
    gotoTable[{4, "ClassDeclarationList"}] = 7;
    gotoTable[{4, "ClassDeclaration"}] = 4;
    actionTable[{5, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 8);
    actionTable[{8, TokenType::Public}] = Action(ActionType::SHIFT, 9);
    actionTable[{8, TokenType::Private}] = Action(ActionType::SHIFT, 10);
    actionTable[{8, TokenType::Protected}] = Action(ActionType::SHIFT, 11);
    gotoTable[{8, "MemberList"}] = 12;
    gotoTable[{8, "MemberDeclaration"}] = 13;
    gotoTable[{8, "Visibility"}] = 14;
    actionTable[{12, TokenType::RightBrace}] = Action(ActionType::SHIFT, 15);
    actionTable[{13, TokenType::Public}] = Action(ActionType::SHIFT, 9);
    actionTable[{13, TokenType::Private}] = Action(ActionType::SHIFT, 10);
    actionTable[{13, TokenType::Protected}] = Action(ActionType::SHIFT, 11);
    gotoTable[{13, "MemberList"}] = 16;
    gotoTable[{13, "MemberDeclaration"}] = 13;
    gotoTable[{13, "Visibility"}] = 14;
    actionTable[{14, TokenType::Class}] = Action(ActionType::SHIFT, 1);
    actionTable[{14, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{14, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{14, TokenType::Void}] = Action(ActionType::SHIFT, 19);
    actionTable[{14, TokenType::Identifier}] = Action(ActionType::SHIFT, 20);
    gotoTable[{14, "ClassDeclaration"}] = 21;
    gotoTable[{14, "Type"}] = 22;
    gotoTable[{14, "VarDeclaration"}] = 23;
    gotoTable[{14, "FunctionDeclaration"}] = 24;
    actionTable[{15, TokenType::Semicolon}] = Action(ActionType::SHIFT, 25);
    actionTable[{19, TokenType::Identifier}] = Action(ActionType::SHIFT, 26);
    actionTable[{22, TokenType::Identifier}] = Action(ActionType::SHIFT, 27);
    actionTable[{26, TokenType::LeftParen}] = Action(ActionType::SHIFT, 28);
    actionTable[{27, TokenType::Semicolon}] = Action(ActionType::SHIFT, 29);
    actionTable[{27, TokenType::Assign}] = Action(ActionType::SHIFT, 30);
    actionTable[{27, TokenType::LeftParen}] = Action(ActionType::SHIFT, 31);
    actionTable[{28, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{28, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{28, TokenType::Identifier}] = Action(ActionType::SHIFT, 20);
    gotoTable[{28, "Type"}] = 32;
    gotoTable[{28, "FunctionParameters"}] = 33;
    gotoTable[{28, "Parameter"}] = 34;
    actionTable[{30, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{30, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{30, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{30, TokenType::New}] = Action(ActionType::SHIFT, 38);
    actionTable[{30, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{30, "NewExpression"}] = 40;
    gotoTable[{30, "Expression"}] = 41;
    gotoTable[{30, "AdditiveExpression"}] = 42;
    gotoTable[{30, "Term"}] = 43;
    gotoTable[{30, "Factor"}] = 44;
    actionTable[{31, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{31, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{31, TokenType::Identifier}] = Action(ActionType::SHIFT, 20);
    gotoTable[{31, "Type"}] = 32;
    gotoTable[{31, "FunctionParameters"}] = 45;
    gotoTable[{31, "Parameter"}] = 34;
    actionTable[{32, TokenType::Identifier}] = Action(ActionType::SHIFT, 46);
    actionTable[{33, TokenType::RightParen}] = Action(ActionType::SHIFT, 47);
    actionTable[{34, TokenType::Comma}] = Action(ActionType::SHIFT, 48);
    gotoTable[{34, "ParameterListTail"}] = 49;
    actionTable[{38, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{38, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{38, TokenType::Identifier}] = Action(ActionType::SHIFT, 20);
    gotoTable[{38, "Type"}] = 50;
    actionTable[{39, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{39, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{39, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{39, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{39, "Expression"}] = 51;
    gotoTable[{39, "AdditiveExpression"}] = 42;
    gotoTable[{39, "Term"}] = 43;
    gotoTable[{39, "Factor"}] = 44;
    actionTable[{40, TokenType::Semicolon}] = Action(ActionType::SHIFT, 52);
    actionTable[{41, TokenType::Semicolon}] = Action(ActionType::SHIFT, 53);
    actionTable[{43, TokenType::Plus}] = Action(ActionType::SHIFT, 54);
    actionTable[{43, TokenType::Minus}] = Action(ActionType::SHIFT, 55);
    gotoTable[{43, "AdditiveTail"}] = 56;
    actionTable[{44, TokenType::Star}] = Action(ActionType::SHIFT, 57);
    actionTable[{44, TokenType::Slash}] = Action(ActionType::SHIFT, 58);
    actionTable[{44, TokenType::Percent}] = Action(ActionType::SHIFT, 59);
    gotoTable[{44, "TermTail"}] = 60;
    actionTable[{45, TokenType::RightParen}] = Action(ActionType::SHIFT, 61);
    actionTable[{47, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{47, "Block"}] = 63;
    actionTable[{48, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{48, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{48, TokenType::Identifier}] = Action(ActionType::SHIFT, 20);
    gotoTable[{48, "Type"}] = 32;
    gotoTable[{48, "Parameter"}] = 64;
    actionTable[{50, TokenType::LeftBracket}] = Action(ActionType::SHIFT, 65);
    actionTable[{51, TokenType::RightParen}] = Action(ActionType::SHIFT, 66);
    actionTable[{54, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{54, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{54, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{54, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{54, "Term"}] = 67;
    gotoTable[{54, "Factor"}] = 44;
    actionTable[{55, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{55, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{55, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{55, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{55, "Term"}] = 68;
    gotoTable[{55, "Factor"}] = 44;
    actionTable[{57, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{57, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{57, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{57, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{57, "Factor"}] = 69;
    actionTable[{58, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{58, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{58, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{58, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{58, "Factor"}] = 70;
    actionTable[{59, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{59, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{59, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{59, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{59, "Factor"}] = 71;
    actionTable[{61, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{61, "Block"}] = 72;
    actionTable[{62, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{62, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{62, TokenType::Identifier}] = Action(ActionType::SHIFT, 73);
    actionTable[{62, TokenType::Return}] = Action(ActionType::SHIFT, 74);
    actionTable[{62, TokenType::If}] = Action(ActionType::SHIFT, 75);
    actionTable[{62, TokenType::While}] = Action(ActionType::SHIFT, 76);
    actionTable[{62, TokenType::For}] = Action(ActionType::SHIFT, 77);
    actionTable[{62, TokenType::Do}] = Action(ActionType::SHIFT, 78);
    actionTable[{62, TokenType::Delete}] = Action(ActionType::SHIFT, 79);
    actionTable[{62, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{62, "Type"}] = 80;
    gotoTable[{62, "VarDeclaration"}] = 81;
    gotoTable[{62, "Block"}] = 82;
    gotoTable[{62, "StatementList"}] = 83;
    gotoTable[{62, "Statement"}] = 84;
    gotoTable[{62, "Assignment"}] = 85;
    gotoTable[{62, "ReturnStatement"}] = 86;
    gotoTable[{62, "IfStatement"}] = 87;
    gotoTable[{62, "WhileStatement"}] = 88;
    gotoTable[{62, "ForStatement"}] = 89;
    gotoTable[{62, "DoStatement"}] = 90;
    gotoTable[{62, "DeleteStatement"}] = 91;
    actionTable[{64, TokenType::Comma}] = Action(ActionType::SHIFT, 48);
    gotoTable[{64, "ParameterListTail"}] = 92;
    actionTable[{65, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{65, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{65, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{65, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{65, "Expression"}] = 93;
    gotoTable[{65, "AdditiveExpression"}] = 42;
    gotoTable[{65, "Term"}] = 43;
    gotoTable[{65, "Factor"}] = 44;
    actionTable[{67, TokenType::Plus}] = Action(ActionType::SHIFT, 54);
    actionTable[{67, TokenType::Minus}] = Action(ActionType::SHIFT, 55);
    gotoTable[{67, "AdditiveTail"}] = 94;
    actionTable[{68, TokenType::Plus}] = Action(ActionType::SHIFT, 54);
    actionTable[{68, TokenType::Minus}] = Action(ActionType::SHIFT, 55);
    gotoTable[{68, "AdditiveTail"}] = 95;
    actionTable[{69, TokenType::Star}] = Action(ActionType::SHIFT, 57);
    actionTable[{69, TokenType::Slash}] = Action(ActionType::SHIFT, 58);
    actionTable[{69, TokenType::Percent}] = Action(ActionType::SHIFT, 59);
    gotoTable[{69, "TermTail"}] = 96;
    actionTable[{70, TokenType::Star}] = Action(ActionType::SHIFT, 57);
    actionTable[{70, TokenType::Slash}] = Action(ActionType::SHIFT, 58);
    actionTable[{70, TokenType::Percent}] = Action(ActionType::SHIFT, 59);
    gotoTable[{70, "TermTail"}] = 97;
    actionTable[{71, TokenType::Star}] = Action(ActionType::SHIFT, 57);
    actionTable[{71, TokenType::Slash}] = Action(ActionType::SHIFT, 58);
    actionTable[{71, TokenType::Percent}] = Action(ActionType::SHIFT, 59);
    gotoTable[{71, "TermTail"}] = 98;
    actionTable[{73, TokenType::Assign}] = Action(ActionType::SHIFT, 99);
    actionTable[{74, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{74, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{74, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{74, TokenType::Semicolon}] = Action(ActionType::SHIFT, 100);
    actionTable[{74, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{74, "Expression"}] = 101;
    gotoTable[{74, "AdditiveExpression"}] = 42;
    gotoTable[{74, "Term"}] = 43;
    gotoTable[{74, "Factor"}] = 44;
    actionTable[{75, TokenType::LeftParen}] = Action(ActionType::SHIFT, 102);
    actionTable[{76, TokenType::LeftParen}] = Action(ActionType::SHIFT, 103);
    actionTable[{77, TokenType::LeftParen}] = Action(ActionType::SHIFT, 104);
    actionTable[{78, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{78, "Block"}] = 105;
    actionTable[{79, TokenType::Identifier}] = Action(ActionType::SHIFT, 106);
    actionTable[{79, TokenType::LeftBracket}] = Action(ActionType::SHIFT, 107);
    actionTable[{80, TokenType::Identifier}] = Action(ActionType::SHIFT, 108);
    actionTable[{83, TokenType::RightBrace}] = Action(ActionType::SHIFT, 109);
    actionTable[{84, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{84, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{84, TokenType::Identifier}] = Action(ActionType::SHIFT, 73);
    actionTable[{84, TokenType::Return}] = Action(ActionType::SHIFT, 74);
    actionTable[{84, TokenType::If}] = Action(ActionType::SHIFT, 75);
    actionTable[{84, TokenType::While}] = Action(ActionType::SHIFT, 76);
    actionTable[{84, TokenType::For}] = Action(ActionType::SHIFT, 77);
    actionTable[{84, TokenType::Do}] = Action(ActionType::SHIFT, 78);
    actionTable[{84, TokenType::Delete}] = Action(ActionType::SHIFT, 79);
    actionTable[{84, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{84, "Type"}] = 80;
    gotoTable[{84, "VarDeclaration"}] = 81;
    gotoTable[{84, "Block"}] = 82;
    gotoTable[{84, "StatementList"}] = 110;
    gotoTable[{84, "Statement"}] = 84;
    gotoTable[{84, "Assignment"}] = 85;
    gotoTable[{84, "ReturnStatement"}] = 86;
    gotoTable[{84, "IfStatement"}] = 87;
    gotoTable[{84, "WhileStatement"}] = 88;
    gotoTable[{84, "ForStatement"}] = 89;
    gotoTable[{84, "DoStatement"}] = 90;
    gotoTable[{84, "DeleteStatement"}] = 91;
    actionTable[{85, TokenType::Semicolon}] = Action(ActionType::SHIFT, 111);
    actionTable[{91, TokenType::Semicolon}] = Action(ActionType::SHIFT, 112);
    actionTable[{93, TokenType::RightBracket}] = Action(ActionType::SHIFT, 113);
    actionTable[{99, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{99, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{99, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{99, TokenType::New}] = Action(ActionType::SHIFT, 38);
    actionTable[{99, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{99, "NewExpression"}] = 114;
    gotoTable[{99, "Expression"}] = 115;
    gotoTable[{99, "AdditiveExpression"}] = 42;
    gotoTable[{99, "Term"}] = 43;
    gotoTable[{99, "Factor"}] = 44;
    actionTable[{101, TokenType::Semicolon}] = Action(ActionType::SHIFT, 116);
    actionTable[{102, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{102, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{102, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{102, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{102, "Expression"}] = 117;
    gotoTable[{102, "AdditiveExpression"}] = 42;
    gotoTable[{102, "Term"}] = 43;
    gotoTable[{102, "Factor"}] = 44;
    actionTable[{103, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{103, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{103, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{103, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{103, "Expression"}] = 118;
    gotoTable[{103, "AdditiveExpression"}] = 42;
    gotoTable[{103, "Term"}] = 43;
    gotoTable[{103, "Factor"}] = 44;
    actionTable[{104, TokenType::Int}] = Action(ActionType::SHIFT, 17);
    actionTable[{104, TokenType::Char}] = Action(ActionType::SHIFT, 18);
    actionTable[{104, TokenType::Identifier}] = Action(ActionType::SHIFT, 73);
    gotoTable[{104, "Type"}] = 119;
    gotoTable[{104, "Assignment"}] = 120;
    gotoTable[{104, "ForInitialisation"}] = 121;
    actionTable[{105, TokenType::While}] = Action(ActionType::SHIFT, 122);
    actionTable[{107, TokenType::RightBracket}] = Action(ActionType::SHIFT, 123);
    actionTable[{108, TokenType::Semicolon}] = Action(ActionType::SHIFT, 29);
    actionTable[{108, TokenType::Assign}] = Action(ActionType::SHIFT, 30);
    actionTable[{117, TokenType::RightParen}] = Action(ActionType::SHIFT, 124);
    actionTable[{118, TokenType::RightParen}] = Action(ActionType::SHIFT, 125);
    actionTable[{119, TokenType::Identifier}] = Action(ActionType::SHIFT, 126);
    actionTable[{121, TokenType::Semicolon}] = Action(ActionType::SHIFT, 127);
    actionTable[{122, TokenType::LeftParen}] = Action(ActionType::SHIFT, 128);
    actionTable[{123, TokenType::Identifier}] = Action(ActionType::SHIFT, 129);
    actionTable[{124, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{124, "Block"}] = 130;
    actionTable[{125, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{125, "Block"}] = 131;
    actionTable[{126, TokenType::Assign}] = Action(ActionType::SHIFT, 132);
    actionTable[{127, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{127, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{127, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{127, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{127, "Expression"}] = 133;
    gotoTable[{127, "AdditiveExpression"}] = 42;
    gotoTable[{127, "Term"}] = 43;
    gotoTable[{127, "Factor"}] = 44;
    actionTable[{128, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{128, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{128, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{128, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{128, "Expression"}] = 134;
    gotoTable[{128, "AdditiveExpression"}] = 42;
    gotoTable[{128, "Term"}] = 43;
    gotoTable[{128, "Factor"}] = 44;
    actionTable[{130, TokenType::Else}] = Action(ActionType::SHIFT, 135);
    actionTable[{132, TokenType::Identifier}] = Action(ActionType::SHIFT, 35);
    actionTable[{132, TokenType::Integer}] = Action(ActionType::SHIFT, 36);
    actionTable[{132, TokenType::Character}] = Action(ActionType::SHIFT, 37);
    actionTable[{132, TokenType::LeftParen}] = Action(ActionType::SHIFT, 39);
    gotoTable[{132, "Expression"}] = 136;
    gotoTable[{132, "AdditiveExpression"}] = 42;
    gotoTable[{132, "Term"}] = 43;
    gotoTable[{132, "Factor"}] = 44;
    actionTable[{133, TokenType::Semicolon}] = Action(ActionType::SHIFT, 137);
    actionTable[{134, TokenType::RightParen}] = Action(ActionType::SHIFT, 138);
    actionTable[{135, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{135, "Block"}] = 139;
    actionTable[{137, TokenType::Identifier}] = Action(ActionType::SHIFT, 140);
    gotoTable[{137, "Assignment"}] = 141;
    gotoTable[{137, "ForUpdate"}] = 142;
    actionTable[{138, TokenType::Semicolon}] = Action(ActionType::SHIFT, 143);
    actionTable[{140, TokenType::Assign}] = Action(ActionType::SHIFT, 99);
    actionTable[{140, TokenType::PlusPlus}] = Action(ActionType::SHIFT, 144);
    actionTable[{140, TokenType::MinusMinus}] = Action(ActionType::SHIFT, 145);
    actionTable[{142, TokenType::RightParen}] = Action(ActionType::SHIFT, 146);
    actionTable[{146, TokenType::LeftBrace}] = Action(ActionType::SHIFT, 62);
    gotoTable[{146, "Block"}] = 147;

    defaultActions[0] = Action(ActionType::REDUCE, 3);
    defaultActions[3] = Action(ActionType::REDUCE, 1);
    defaultActions[4] = Action(ActionType::REDUCE, 3);
    defaultActions[6] = Action(ActionType::ACCEPT, 0);
    defaultActions[7] = Action(ActionType::REDUCE, 2);
    defaultActions[8] = Action(ActionType::REDUCE, 6);
    defaultActions[9] = Action(ActionType::REDUCE, 10);
    defaultActions[10] = Action(ActionType::REDUCE, 11);
    defaultActions[11] = Action(ActionType::REDUCE, 12);
    defaultActions[13] = Action(ActionType::REDUCE, 6);
    defaultActions[16] = Action(ActionType::REDUCE, 5);
    defaultActions[17] = Action(ActionType::REDUCE, 13);
    defaultActions[18] = Action(ActionType::REDUCE, 14);
    defaultActions[20] = Action(ActionType::REDUCE, 15);
    defaultActions[21] = Action(ActionType::REDUCE, 9);
    defaultActions[23] = Action(ActionType::REDUCE, 7);
    defaultActions[24] = Action(ActionType::REDUCE, 8);
    defaultActions[25] = Action(ActionType::REDUCE, 4);
    defaultActions[28] = Action(ActionType::REDUCE, 22);
    defaultActions[29] = Action(ActionType::REDUCE, 16);
    defaultActions[31] = Action(ActionType::REDUCE, 22);
    defaultActions[34] = Action(ActionType::REDUCE, 24);
    defaultActions[35] = Action(ActionType::REDUCE, 69);
    defaultActions[36] = Action(ActionType::REDUCE, 70);
    defaultActions[37] = Action(ActionType::REDUCE, 71);
    defaultActions[42] = Action(ActionType::REDUCE, 59);
    defaultActions[43] = Action(ActionType::REDUCE, 63);
    defaultActions[44] = Action(ActionType::REDUCE, 68);
    defaultActions[46] = Action(ActionType::REDUCE, 25);
    defaultActions[49] = Action(ActionType::REDUCE, 21);
    defaultActions[50] = Action(ActionType::REDUCE, 55);
    defaultActions[52] = Action(ActionType::REDUCE, 18);
    defaultActions[53] = Action(ActionType::REDUCE, 17);
    defaultActions[56] = Action(ActionType::REDUCE, 60);
    defaultActions[60] = Action(ActionType::REDUCE, 64);
    defaultActions[62] = Action(ActionType::REDUCE, 28);
    defaultActions[63] = Action(ActionType::REDUCE, 20);
    defaultActions[64] = Action(ActionType::REDUCE, 24);
    defaultActions[66] = Action(ActionType::REDUCE, 72);
    defaultActions[67] = Action(ActionType::REDUCE, 63);
    defaultActions[68] = Action(ActionType::REDUCE, 63);
    defaultActions[69] = Action(ActionType::REDUCE, 68);
    defaultActions[70] = Action(ActionType::REDUCE, 68);
    defaultActions[71] = Action(ActionType::REDUCE, 68);
    defaultActions[72] = Action(ActionType::REDUCE, 19);
    defaultActions[73] = Action(ActionType::REDUCE, 15);
    defaultActions[81] = Action(ActionType::REDUCE, 29);
    defaultActions[82] = Action(ActionType::REDUCE, 37);
    defaultActions[84] = Action(ActionType::REDUCE, 28);
    defaultActions[86] = Action(ActionType::REDUCE, 31);
    defaultActions[87] = Action(ActionType::REDUCE, 32);
    defaultActions[88] = Action(ActionType::REDUCE, 33);
    defaultActions[89] = Action(ActionType::REDUCE, 34);
    defaultActions[90] = Action(ActionType::REDUCE, 35);
    defaultActions[92] = Action(ActionType::REDUCE, 23);
    defaultActions[94] = Action(ActionType::REDUCE, 61);
    defaultActions[95] = Action(ActionType::REDUCE, 62);
    defaultActions[96] = Action(ActionType::REDUCE, 65);
    defaultActions[97] = Action(ActionType::REDUCE, 66);
    defaultActions[98] = Action(ActionType::REDUCE, 67);
    defaultActions[100] = Action(ActionType::REDUCE, 40);
    defaultActions[104] = Action(ActionType::REDUCE, 46);
    defaultActions[106] = Action(ActionType::REDUCE, 57);
    defaultActions[109] = Action(ActionType::REDUCE, 26);
    defaultActions[110] = Action(ActionType::REDUCE, 27);
    defaultActions[111] = Action(ActionType::REDUCE, 30);
    defaultActions[112] = Action(ActionType::REDUCE, 36);
    defaultActions[113] = Action(ActionType::REDUCE, 56);
    defaultActions[114] = Action(ActionType::REDUCE, 39);
    defaultActions[115] = Action(ActionType::REDUCE, 38);
    defaultActions[116] = Action(ActionType::REDUCE, 41);
    defaultActions[120] = Action(ActionType::REDUCE, 47);
    defaultActions[126] = Action(ActionType::REDUCE, 49);
    defaultActions[129] = Action(ActionType::REDUCE, 58);
    defaultActions[130] = Action(ActionType::REDUCE, 42);
    defaultActions[131] = Action(ActionType::REDUCE, 44);
    defaultActions[136] = Action(ActionType::REDUCE, 48);
    defaultActions[137] = Action(ActionType::REDUCE, 50);
    defaultActions[139] = Action(ActionType::REDUCE, 43);
    defaultActions[141] = Action(ActionType::REDUCE, 51);
    defaultActions[143] = Action(ActionType::REDUCE, 54);
    defaultActions[144] = Action(ActionType::REDUCE, 52);
    defaultActions[145] = Action(ActionType::REDUCE, 53);
    defaultActions[147] = Action(ActionType::REDUCE, 45);
}

Action Parser::getAction(int state, TokenType type){
    std::pair<int, TokenType> key = {state, type};

    // Check if input has a defined action
    if (actionTable.count(key)){
    return actionTable[key];
    }

    // Check if state has a default action
    if (defaultActions.count(state)){
    return defaultActions[state];
    }

    // Default error
    return Action(ActionType::ERROR, -1);
}

void Parser::parse() {
    std::vector<int> stateStack;
    std::vector<ParseNode> nodeStack;

    stateStack.push_back(0);

    size_t tokenPosition = 0;
    while (true){
        int state = stateStack.back();
        TokenType tokenType = TokenType::EndOfInput;

        // Check if token is not the last
        if (tokenPosition < tokens.size()) {
            tokenType = tokens[tokenPosition].type;
        }
        Action action = getAction(state, tokenType);

        if (action.action == ActionType::SHIFT){
            stateStack.push_back(action.value);
            if (tokenPosition < tokens.size()) {
                const Token& shiftedToken = tokens[tokenPosition];
                nodeStack.push_back(ParseNode(tokenTypeToString(shiftedToken.type), shiftedToken.lexeme));
            }
            tokenPosition++;
        } 

        if (action.action == ActionType::REDUCE){
            Rule rule = grammarRules[action.value];

            // Determine how many RHS symbols to reduce/pop
            int rhsSize = rule.getRHS().size();
            std::vector<ParseNode> children;
            for (int i = 0; i < rhsSize; ++i){
                stateStack.pop_back();
                children.push_back(nodeStack.back());
                nodeStack.pop_back();
            }
            
            // Get currente state and lhs
            int currentState = stateStack.back();
            std::string lhs = rule.getLHS();

            // Identify next state and add to stack
            int nextState = gotoTable[{currentState, lhs}];
            stateStack.push_back(nextState);

            std::reverse(children.begin(), children.end());
            ParseNode parent(lhs);
            if (children.empty()) {
                parent.children.push_back(ParseNode("epsilon"));
            } else {
                parent.children = children;
            }
            nodeStack.push_back(parent);
        }

        if (action.action == ActionType::ACCEPT){
            std::cout << "Parse successful!" << std::endl;
            if (!nodeStack.empty()) {
                std::cout << "Parse tree:" << std::endl;
                printTree(nodeStack.front(), std::cout);
            }
            return;
        }

        if (action.action == ActionType::ERROR){
            Token currentToken = {TokenType::EndOfInput, "EOF", -1, -1};
            if (tokenPosition < tokens.size()) {
                currentToken = tokens[tokenPosition];
            }
            std::cerr << "Syntax error at token '"
                    << currentToken.lexeme
                    << "' on line "
                    << currentToken.line
                    << ", column "
                    << currentToken.column
                    << std::endl;
            return;
        }
    }
}

void Parser::printTree(const ParseNode& node, std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << node.name;
    if (!node.lexeme.empty()) {
        out << " (" << node.lexeme << ")";
    }
    out << '\n';

    for (const ParseNode& child : node.children) {
        printTree(child, out, indent + 2);
    }
}
