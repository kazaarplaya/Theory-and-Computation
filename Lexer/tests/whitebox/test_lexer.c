#include "../unity/unity.h"
#include "../lexer.h"
#include "test_lexer.h"

void setUp(void) {
    // Runs before each test.
    // Leave empty unless you need shared setup.
}

void tearDown(void) {
    // Runs after each test.
    // Leave empty unless you need cleanup.
}

void test_keyword_int(void) {
    Lexer lexer;
    init_lexer(&lexer, "int");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_KEYWORD, token.type);
    TEST_ASSERT_EQUAL_STRING("int", token.lexeme);
    TEST_ASSERT_EQUAL(1, token.line);
    TEST_ASSERT_EQUAL(1, token.column);
}

void test_keyword_return(void) {
    Lexer lexer;
    init_lexer(&lexer, "return");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_KEYWORD, token.type);
    TEST_ASSERT_EQUAL_STRING("return", token.lexeme);
}

void test_identifier(void) {
    Lexer lexer;
    init_lexer(&lexer, "student_count");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, token.type);
    TEST_ASSERT_EQUAL_STRING("student_count", token.lexeme);
}

void test_integer_literal(void) {
    Lexer lexer;
    init_lexer(&lexer, "12345");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_INTEGER, token.type);
    TEST_ASSERT_EQUAL_STRING("12345", token.lexeme);
}

void test_delimiter_semicolon(void) {
    Lexer lexer;
    init_lexer(&lexer, ";");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_DELIMITER, token.type);
    TEST_ASSERT_EQUAL_STRING(";", token.lexeme);
}

void test_operator_equals(void) {
    Lexer lexer;
    init_lexer(&lexer, "=");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_OPERATOR, token.type);
    TEST_ASSERT_EQUAL_STRING("=", token.lexeme);
}

void test_invalid_character(void) {
    Lexer lexer;
    init_lexer(&lexer, "@");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_ERROR, token.type);
    TEST_ASSERT_EQUAL_STRING("@", token.lexeme);
}

void test_whitespace_is_skipped(void) {
    Lexer lexer;
    init_lexer(&lexer, "   \t int");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_KEYWORD, token.type);
    TEST_ASSERT_EQUAL_STRING("int", token.lexeme);
}

void test_simple_declaration(void) {
    Lexer lexer;
    init_lexer(&lexer, "int x = 10;");

    Token t1 = tokenize(&lexer);
    Token t2 = tokenize(&lexer);
    Token t3 = tokenize(&lexer);
    Token t4 = tokenize(&lexer);
    Token t5 = tokenize(&lexer);
    Token t6 = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_KEYWORD, t1.type);
    TEST_ASSERT_EQUAL_STRING("int", t1.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_IDENTIFIER, t2.type);
    TEST_ASSERT_EQUAL_STRING("x", t2.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_OPERATOR, t3.type);
    TEST_ASSERT_EQUAL_STRING("=", t3.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_INTEGER, t4.type);
    TEST_ASSERT_EQUAL_STRING("10", t4.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_DELIMITER, t5.type);
    TEST_ASSERT_EQUAL_STRING(";", t5.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_EOF, t6.type);
}

void test_invalid_integer_panic_mode(void) {
    Lexer lexer;
    init_lexer(&lexer, "12abc;");

    Token t1 = tokenize(&lexer);
    Token t2 = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_ERROR, t1.type);
    TEST_ASSERT_EQUAL_STRING("12abc", t1.lexeme);

    TEST_ASSERT_EQUAL(TOKEN_DELIMITER, t2.type);
    TEST_ASSERT_EQUAL_STRING(";", t2.lexeme);
}

void test_line_and_column_tracking(void) {
    Lexer lexer;
    init_lexer(&lexer, "\nint");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_KEYWORD, token.type);
    TEST_ASSERT_EQUAL_STRING("int", token.lexeme);
    TEST_ASSERT_EQUAL(2, token.line);
    TEST_ASSERT_EQUAL(1, token.column);
}

void test_empty_input_returns_eof(void) {
    Lexer lexer;
    init_lexer(&lexer, "");

    Token token = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_keyword_int);
    RUN_TEST(test_keyword_return);
    RUN_TEST(test_identifier);
    RUN_TEST(test_integer_literal);
    RUN_TEST(test_delimiter_semicolon);
    RUN_TEST(test_operator_equals);
    RUN_TEST(test_invalid_character);
    RUN_TEST(test_whitespace_is_skipped);
    RUN_TEST(test_simple_declaration);
    RUN_TEST(test_invalid_integer_panic_mode);
    RUN_TEST(test_line_and_column_tracking);
    RUN_TEST(test_empty_input_returns_eof);

    return UNITY_END();
}