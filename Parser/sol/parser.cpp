#include "parser.h"

#include <algorithm>
#include <cctype>
#include <deque>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {

const std::string Epsilon = "<epsilon>";
const std::string End = "$";
const std::string Start = "S'";

struct Rule {
    std::string lhs;
    std::vector<std::string> rhs;
};

struct Item {
    int rule = 0;
    int dot = 0;
    std::string lookahead;

    bool operator<(const Item& other) const {
        if (rule != other.rule) return rule < other.rule;
        if (dot != other.dot) return dot < other.dot;
        return lookahead < other.lookahead;
    }

    bool operator==(const Item& other) const {
        return rule == other.rule && dot == other.dot && lookahead == other.lookahead;
    }
};

struct Action {
    enum Kind { Shift, Reduce, Accept } kind;
    int value = 0;
};

std::string upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

} // namespace

ParseNode::ParseNode(std::string nodeName, std::string tokenLexeme)
    : name(std::move(nodeName)), lexeme(std::move(tokenLexeme)) {}

struct LR1Parser::Impl {
    std::vector<Rule> rules;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    std::map<std::string, std::set<std::string>> firstSets;
    std::vector<std::set<Item>> states;
    std::map<std::pair<int, std::string>, Action> actions;
    std::map<std::pair<int, std::string>, int> gotos;

    Impl() {
        buildGrammar();
        buildTables();
    }

    void addRule(const std::string& lhs, std::initializer_list<const char*> rhs) {
        Rule rule;
        rule.lhs = lhs;
        for (const char* symbol : rhs) {
            rule.rhs.push_back(symbol);
        }
        rules.push_back(rule);
        nonTerminals.insert(lhs);
    }

    void buildGrammar() {
        addRule(Start, {"Program"});
        addRule("Program", {"ClassDeclarationList"});
        addRule("ClassDeclarationList", {"ClassDeclaration", "ClassDeclarationList"});
        addRule("ClassDeclarationList", {});
        addRule("ClassDeclaration", {"class", "identifier", "{", "MemberList", "}", ";"});
        addRule("MemberList", {"MemberDeclaration", "MemberList"});
        addRule("MemberList", {});
        addRule("MemberDeclaration", {"Visibility", "VarDeclaration"});
        addRule("MemberDeclaration", {"Visibility", "FunctionDeclaration"});
        addRule("MemberDeclaration", {"Visibility", "ClassDeclaration"});
        addRule("Visibility", {"public"});
        addRule("Visibility", {"private"});
        addRule("Visibility", {"protected"});
        addRule("Type", {"int"});
        addRule("Type", {"char"});
        addRule("Type", {"identifier"});
        addRule("VarDeclaration", {"Type", "identifier", ";"});
        addRule("VarDeclaration", {"Type", "identifier", "=", "Expression", ";"});
        addRule("VarDeclaration", {"Type", "identifier", "=", "NewExpression", ";"});
        addRule("FunctionDeclaration", {"Type", "identifier", "(", "FunctionParameters", ")", "Block"});
        addRule("FunctionDeclaration", {"void", "identifier", "(", "FunctionParameters", ")", "Block"});
        addRule("FunctionParameters", {"Parameter", "ParameterListTail"});
        addRule("FunctionParameters", {});
        addRule("ParameterListTail", {",", "Parameter", "ParameterListTail"});
        addRule("ParameterListTail", {});
        addRule("Parameter", {"Type", "identifier"});
        addRule("Block", {"{", "StatementList", "}"});
        addRule("StatementList", {"Statement", "StatementList"});
        addRule("StatementList", {});
        addRule("Statement", {"VarDeclaration"});
        addRule("Statement", {"Assignment", ";"});
        addRule("Statement", {"ReturnStatement"});
        addRule("Statement", {"IfStatement"});
        addRule("Statement", {"WhileStatement"});
        addRule("Statement", {"ForStatement"});
        addRule("Statement", {"DoStatement"});
        addRule("Statement", {"DeleteStatement", ";"});
        addRule("Statement", {"Block"});
        addRule("Assignment", {"identifier", "=", "Expression"});
        addRule("Assignment", {"identifier", "=", "NewExpression"});
        addRule("ReturnStatement", {"return", ";"});
        addRule("ReturnStatement", {"return", "Expression", ";"});
        addRule("IfStatement", {"if", "(", "Expression", ")", "Block"});
        addRule("IfStatement", {"if", "(", "Expression", ")", "Block", "else", "Block"});
        addRule("WhileStatement", {"while", "(", "Expression", ")", "Block"});
        addRule("ForStatement", {"for", "(", "ForInitialisation", ";", "Expression", ";", "ForUpdate", ")", "Block"});
        addRule("ForInitialisation", {});
        addRule("ForInitialisation", {"Assignment"});
        addRule("ForInitialisation", {"Type", "identifier", "=", "Expression"});
        addRule("ForInitialisation", {"Type", "identifier"});
        addRule("ForUpdate", {});
        addRule("ForUpdate", {"Assignment"});
        addRule("ForUpdate", {"identifier", "++"});
        addRule("ForUpdate", {"identifier", "--"});
        addRule("DoStatement", {"do", "Block", "while", "(", "Expression", ")", ";"});
        addRule("NewExpression", {"new", "Type"});
        addRule("NewExpression", {"new", "Type", "[", "Expression", "]"});
        addRule("DeleteStatement", {"delete", "identifier"});
        addRule("DeleteStatement", {"delete", "[", "]", "identifier"});
        addRule("Expression", {"AdditiveExpression"});
        addRule("AdditiveExpression", {"Term", "AdditiveTail"});
        addRule("AdditiveTail", {"+", "Term", "AdditiveTail"});
        addRule("AdditiveTail", {"-", "Term", "AdditiveTail"});
        addRule("AdditiveTail", {});
        addRule("Term", {"Factor", "TermTail"});
        addRule("TermTail", {"*", "Factor", "TermTail"});
        addRule("TermTail", {"/", "Factor", "TermTail"});
        addRule("TermTail", {"%", "Factor", "TermTail"});
        addRule("TermTail", {});
        addRule("Factor", {"identifier"});
        addRule("Factor", {"integer"});
        addRule("Factor", {"character"});
        addRule("Factor", {"(", "Expression", ")"});

        for (const Rule& rule : rules) {
            for (const std::string& symbol : rule.rhs) {
                if (!nonTerminals.count(symbol)) {
                    terminals.insert(symbol);
                }
            }
        }
        terminals.insert(End);
    }

    void computeFirstSets() {
        for (const std::string& terminal : terminals) {
            firstSets[terminal].insert(terminal);
        }
        for (const std::string& nonTerminal : nonTerminals) {
            firstSets[nonTerminal];
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (const Rule& rule : rules) {
                bool nullablePrefix = true;
                if (rule.rhs.empty()) {
                    changed = firstSets[rule.lhs].insert(Epsilon).second || changed;
                    continue;
                }
                for (const std::string& symbol : rule.rhs) {
                    for (const std::string& first : firstSets[symbol]) {
                        if (first != Epsilon) {
                            changed = firstSets[rule.lhs].insert(first).second || changed;
                        }
                    }
                    if (!firstSets[symbol].count(Epsilon)) {
                        nullablePrefix = false;
                        break;
                    }
                }
                if (nullablePrefix) {
                    changed = firstSets[rule.lhs].insert(Epsilon).second || changed;
                }
            }
        }
    }

    std::set<std::string> firstOfSequence(std::vector<std::string> symbols) const {
        std::set<std::string> result;
        bool nullablePrefix = true;
        for (const std::string& symbol : symbols) {
            const auto found = firstSets.find(symbol);
            if (found == firstSets.end()) continue;
            for (const std::string& first : found->second) {
                if (first != Epsilon) result.insert(first);
            }
            if (!found->second.count(Epsilon)) {
                nullablePrefix = false;
                break;
            }
        }
        if (nullablePrefix) result.insert(Epsilon);
        return result;
    }

    std::set<Item> closure(std::set<Item> items) const {
        bool changed = true;
        while (changed) {
            changed = false;
            std::vector<Item> snapshot(items.begin(), items.end());
            for (const Item& item : snapshot) {
                const Rule& rule = rules[item.rule];
                if (item.dot >= static_cast<int>(rule.rhs.size())) continue;
                const std::string& next = rule.rhs[item.dot];
                if (!nonTerminals.count(next)) continue;

                std::vector<std::string> beta;
                for (int i = item.dot + 1; i < static_cast<int>(rule.rhs.size()); ++i) {
                    beta.push_back(rule.rhs[i]);
                }
                beta.push_back(item.lookahead);

                for (const std::string& lookahead : firstOfSequence(beta)) {
                    if (lookahead == Epsilon) continue;
                    for (int ruleIndex = 0; ruleIndex < static_cast<int>(rules.size()); ++ruleIndex) {
                        if (rules[ruleIndex].lhs == next) {
                            changed = items.insert({ruleIndex, 0, lookahead}).second || changed;
                        }
                    }
                }
            }
        }
        return items;
    }

    std::set<Item> goTo(const std::set<Item>& state, const std::string& symbol) const {
        std::set<Item> moved;
        for (const Item& item : state) {
            const Rule& rule = rules[item.rule];
            if (item.dot < static_cast<int>(rule.rhs.size()) && rule.rhs[item.dot] == symbol) {
                moved.insert({item.rule, item.dot + 1, item.lookahead});
            }
        }
        return moved.empty() ? moved : closure(moved);
    }

    int stateIndex(const std::set<Item>& state) const {
        for (int i = 0; i < static_cast<int>(states.size()); ++i) {
            if (states[i] == state) return i;
        }
        return -1;
    }

    void setAction(int state, const std::string& terminal, Action action) {
        auto key = std::make_pair(state, terminal);
        auto existing = actions.find(key);
        if (existing != actions.end()) {
            const Action old = existing->second;
            if (old.kind == Action::Shift && action.kind == Action::Reduce && terminal == "else") {
                return;
            }
            if (old.kind == Action::Reduce && action.kind == Action::Shift && terminal == "else") {
                existing->second = action;
                return;
            }
            if (old.kind != action.kind || old.value != action.value) {
                std::ostringstream message;
                message << "LR(1) conflict in state " << state << " on " << terminal;
                throw std::runtime_error(message.str());
            }
            return;
        }
        actions[key] = action;
    }

    void buildTables() {
        computeFirstSets();

        states.push_back(closure({Item{0, 0, End}}));
        std::deque<int> pending;
        pending.push_back(0);

        std::vector<std::string> symbols(terminals.begin(), terminals.end());
        symbols.insert(symbols.end(), nonTerminals.begin(), nonTerminals.end());

        while (!pending.empty()) {
            int state = pending.front();
            pending.pop_front();

            for (const std::string& symbol : symbols) {
                if (symbol == End) continue;
                std::set<Item> target = goTo(states[state], symbol);
                if (target.empty()) continue;
                int targetIndex = stateIndex(target);
                if (targetIndex == -1) {
                    targetIndex = static_cast<int>(states.size());
                    states.push_back(target);
                    pending.push_back(targetIndex);
                }

                if (terminals.count(symbol)) {
                    setAction(state, symbol, {Action::Shift, targetIndex});
                } else {
                    gotos[{state, symbol}] = targetIndex;
                }
            }
        }

        for (int state = 0; state < static_cast<int>(states.size()); ++state) {
            for (const Item& item : states[state]) {
                const Rule& rule = rules[item.rule];
                if (item.dot != static_cast<int>(rule.rhs.size())) continue;
                if (rule.lhs == Start && item.lookahead == End) {
                    setAction(state, End, {Action::Accept, 0});
                } else {
                    setAction(state, item.lookahead, {Action::Reduce, item.rule});
                }
            }
        }
    }

    std::string expectedTokens(int state) const {
        std::ostringstream out;
        bool first = true;
        for (const auto& entry : actions) {
            if (entry.first.first != state) continue;
            if (!first) out << ", ";
            out << entry.first.second;
            first = false;
        }
        return out.str();
    }
};

LR1Parser::LR1Parser() : impl(std::make_shared<Impl>()) {}

ParseResult LR1Parser::parse(const std::vector<Token>& lexerTokens) const {
    std::vector<Token> tokens = lexerTokens;
    if (tokens.empty() || tokens.back().type != TokenType::EndOfInput) {
        tokens.push_back({TokenType::EndOfInput, "", 0, 0});
    }

    std::vector<int> stateStack;
    std::vector<std::unique_ptr<ParseNode>> nodeStack;
    stateStack.push_back(0);

    std::size_t position = 0;
    while (position < tokens.size()) {
        const Token& token = tokens[position];
        const std::string terminal = tokenTypeName(token.type);
        const int state = stateStack.back();
        const auto actionIt = impl->actions.find({state, terminal});

        if (actionIt == impl->actions.end()) {
            std::ostringstream error;
            error << "Syntax error at token '" << (token.lexeme.empty() ? terminal : token.lexeme) << "'";
            if (token.line > 0) error << " line " << token.line;
            if (token.column > 0) error << ", column " << token.column;
            const std::string expected = impl->expectedTokens(state);
            if (!expected.empty()) error << ". Expected one of: " << expected;
            return {false, nullptr, error.str()};
        }

        const Action action = actionIt->second;
        if (action.kind == Action::Shift) {
            stateStack.push_back(action.value);
            nodeStack.push_back(std::make_unique<ParseNode>(terminal, token.lexeme));
            ++position;
            continue;
        }

        if (action.kind == Action::Reduce) {
            const Rule& rule = impl->rules[action.value];
            auto parent = std::make_unique<ParseNode>(rule.lhs);
            std::vector<std::unique_ptr<ParseNode>> children;

            for (std::size_t i = 0; i < rule.rhs.size(); ++i) {
                stateStack.pop_back();
                children.push_back(std::move(nodeStack.back()));
                nodeStack.pop_back();
            }
            for (auto child = children.rbegin(); child != children.rend(); ++child) {
                parent->children.push_back(std::move(*child));
            }
            if (rule.rhs.empty()) {
                parent->children.push_back(std::make_unique<ParseNode>(Epsilon));
            }

            const int gotoFrom = stateStack.back();
            const auto gotoIt = impl->gotos.find({gotoFrom, rule.lhs});
            if (gotoIt == impl->gotos.end()) {
                return {false, nullptr, "Internal parser error: missing goto for " + rule.lhs};
            }
            stateStack.push_back(gotoIt->second);
            nodeStack.push_back(std::move(parent));
            continue;
        }

        if (nodeStack.size() != 1) {
            return {false, nullptr, "Internal parser error: accept reached with an invalid parse stack"};
        }
        return {true, std::move(nodeStack.back()), ""};
    }

    return {false, nullptr, "Syntax error: unexpected end of input"};
}

void LR1Parser::printTree(const ParseNode& node, std::ostream& out, int indent) const {
    out << std::string(indent, ' ') << node.name;
    if (!node.lexeme.empty()) {
        out << " (" << node.lexeme << ")";
    }
    out << '\n';
    for (const auto& child : node.children) {
        printTree(*child, out, indent + 2);
    }
}

const char* tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::Class: return "class";
        case TokenType::Public: return "public";
        case TokenType::Private: return "private";
        case TokenType::Protected: return "protected";
        case TokenType::Int: return "int";
        case TokenType::Char: return "char";
        case TokenType::Void: return "void";
        case TokenType::Identifier: return "identifier";
        case TokenType::Integer: return "integer";
        case TokenType::Character: return "character";
        case TokenType::Return: return "return";
        case TokenType::If: return "if";
        case TokenType::Else: return "else";
        case TokenType::While: return "while";
        case TokenType::For: return "for";
        case TokenType::Do: return "do";
        case TokenType::New: return "new";
        case TokenType::Delete: return "delete";
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
        case TokenType::EndOfInput: return "$";
    }
    return "$";
}

TokenType tokenTypeFromName(const std::string& name) {
    static const std::unordered_map<std::string, TokenType> tokens = {
        {"CLASS", TokenType::Class}, {"class", TokenType::Class},
        {"PUBLIC", TokenType::Public}, {"public", TokenType::Public},
        {"PRIVATE", TokenType::Private}, {"private", TokenType::Private},
        {"PROTECTED", TokenType::Protected}, {"protected", TokenType::Protected},
        {"INT", TokenType::Int}, {"int", TokenType::Int},
        {"CHAR", TokenType::Char}, {"char", TokenType::Char},
        {"VOID", TokenType::Void}, {"void", TokenType::Void},
        {"IDENTIFIER", TokenType::Identifier}, {"identifier", TokenType::Identifier},
        {"INTEGER", TokenType::Integer}, {"integer", TokenType::Integer},
        {"CHARACTER", TokenType::Character}, {"character", TokenType::Character},
        {"RETURN", TokenType::Return}, {"return", TokenType::Return},
        {"IF", TokenType::If}, {"if", TokenType::If},
        {"ELSE", TokenType::Else}, {"else", TokenType::Else},
        {"WHILE", TokenType::While}, {"while", TokenType::While},
        {"FOR", TokenType::For}, {"for", TokenType::For},
        {"DO", TokenType::Do}, {"do", TokenType::Do},
        {"NEW", TokenType::New}, {"new", TokenType::New},
        {"DELETE", TokenType::Delete}, {"delete", TokenType::Delete},
        {"LBRACE", TokenType::LeftBrace}, {"{", TokenType::LeftBrace},
        {"RBRACE", TokenType::RightBrace}, {"}", TokenType::RightBrace},
        {"LPAREN", TokenType::LeftParen}, {"(", TokenType::LeftParen},
        {"RPAREN", TokenType::RightParen}, {")", TokenType::RightParen},
        {"LBRACKET", TokenType::LeftBracket}, {"[", TokenType::LeftBracket},
        {"RBRACKET", TokenType::RightBracket}, {"]", TokenType::RightBracket},
        {"SEMICOLON", TokenType::Semicolon}, {";", TokenType::Semicolon},
        {"COMMA", TokenType::Comma}, {",", TokenType::Comma},
        {"ASSIGN", TokenType::Assign}, {"=", TokenType::Assign},
        {"PLUS", TokenType::Plus}, {"+", TokenType::Plus},
        {"MINUS", TokenType::Minus}, {"-", TokenType::Minus},
        {"STAR", TokenType::Star}, {"*", TokenType::Star},
        {"SLASH", TokenType::Slash}, {"/", TokenType::Slash},
        {"PERCENT", TokenType::Percent}, {"%", TokenType::Percent},
        {"PLUSPLUS", TokenType::PlusPlus}, {"++", TokenType::PlusPlus},
        {"MINUSMINUS", TokenType::MinusMinus}, {"--", TokenType::MinusMinus},
        {"EOF", TokenType::EndOfInput}, {"$", TokenType::EndOfInput}
    };

    const auto found = tokens.find(name);
    if (found != tokens.end()) return found->second;
    const auto upperFound = tokens.find(upper(name));
    if (upperFound != tokens.end()) return upperFound->second;
    throw std::invalid_argument("Unknown token type: " + name);
}
