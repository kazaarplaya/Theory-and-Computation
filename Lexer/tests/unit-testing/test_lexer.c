#include "unity.h"
#include "lexer.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_lexer_initialisation(void){
    Lexer lexer = initialise_lexer("char test;");

    TEST_ASSERT_EQUAL_STRING("char test;", lexer.source);
    TEST_ASSERT_EQUAL(10, lexer.input_length);
    TEST_ASSERT_EQUAL(0, lexer.current_position);
    TEST_ASSERT_EQUAL(1, lexer.next_position);
    TEST_ASSERT_EQUAL('c', lexer.ch);
    TEST_ASSERT_EQUAL(1, lexer.line);
    TEST_ASSERT_EQUAL(1, lexer.column);
}

void test_empty_lexer(void){
    Lexer lexer = initialise_lexer("");

    Token token = tokenize(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_EOF, token.type);
}

void test_unknown_token_type_to_string(void){
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", token_type_to_string((TokenType) 523));
}

void test_repeated_eof(void) {
    Lexer lexer = initialise_lexer("");

    Token t1 = tokenize(&lexer);
    Token t2 = tokenize(&lexer);
    Token t3 = tokenize(&lexer);

    TEST_ASSERT_EQUAL(TOKEN_EOF, t1.type);
    TEST_ASSERT_EQUAL(TOKEN_EOF, t2.type);
    TEST_ASSERT_EQUAL(TOKEN_EOF, t3.type);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lexer_initialisation);
    RUN_TEST(test_empty_lexer);
    RUN_TEST(test_unknown_token_type_to_string);
    RUN_TEST(test_repeated_eof);
    return UNITY_END();
}