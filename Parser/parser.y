%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%token CLASS PUBLIC PRIVATE PROTECTED
%token INT CHAR VOID
%token IDENTIFIER INTEGER CHARACTER
%token RETURN IF ELSE WHILE FOR DO NEW DELETE

%%

program:
    class_declaration_list
    ;

class_declaration_list:
    class_declaration class_declaration_list
    | /* empty */
    ;

class_declaration:
    CLASS IDENTIFIER '{' member_list '}' ';'
    ;

member_list:
    member_declaration member_list
    | /* empty */
    ;

member_declaration:
    visibility var_declaration
    | visibility function_declaration
    | visibility class_declaration
    ;

visibility:
    PUBLIC
    | PRIVATE
    | PROTECTED
    ;

type:
    INT
    | CHAR
    | IDENTIFIER
    ;

var_declaration:
    type IDENTIFIER ';'
    | type IDENTIFIER '=' expression ';'
    | type IDENTIFIER '=' new_expression ';'
    ;

function_declaration:
    type IDENTIFIER '(' function_parameters ')' block
    | VOID IDENTIFIER '(' function_parameters ')' block
    ;

function_parameters:
    parameter parameter_list_tail
    | /* empty */
    ;

parameter_list_tail:
    ',' parameter parameter_list_tail
    | /* empty */
    ;

parameter:
    type IDENTIFIER
    ;

block:
    '{' statement_list '}'
    ;

statement_list:
    statement statement_list
    | /* empty */
    ;

statement:
    var_declaration
    | assignment ';'
    | return_statement
    | if_statement
    | while_statement
    | for_statement
    | do_statement
    | delete_statement ';'
    | block
    ;

assignment:
    IDENTIFIER '=' expression
    | IDENTIFIER '=' new_expression
    ;

return_statement:
    RETURN ';'
    | RETURN expression ';'
    ;

if_statement:
    IF '(' expression ')' block
    | IF '(' expression ')' block ELSE block
    ;

while_statement:
    WHILE '(' expression ')' block
    ;

for_statement:
    FOR '(' for_initialisation ';' expression ';' for_update ')' block
    ;

for_initialisation:
    /* empty */
    | assignment
    | type IDENTIFIER '=' expression
    | type IDENTIFIER
    ;

for_update:
    /* empty */
    | assignment
    ;

do_statement:
    DO block WHILE '(' expression ')' ';'
    ;

new_expression:
    NEW type
    | NEW type '[' expression ']'
    ;

delete_statement:
    DELETE IDENTIFIER
    | DELETE '[' ']' IDENTIFIER
    ;

expression:
    additive_expression
    ;

additive_expression:
    term additive_tail
    ;

additive_tail:
    '+' term additive_tail
    | '-' term additive_tail
    | /* empty */
    ;

term:
    factor term_tail
    ;

term_tail:
    '*' factor term_tail
    | '/' factor term_tail
    | '%' factor term_tail
    | /* empty */
    ;

factor:
    IDENTIFIER
    | INTEGER
    | CHARACTER
    | '(' expression ')'
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
