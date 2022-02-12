#include <sys/cdefs.h>

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "vector.h"

typedef enum {
    TOKEN_UNDEFINED,
    TOKEN_PREPROCESSOR_DIRECTIVE_INCLUDE,
    TOKEN_TYPE,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_INTEGER_LITERAL,
    TOKEN_DOUBLE_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_IDENTIFIER,

    TOKEN_KEYWORD,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_CURLY_BRACE,
    TOKEN_RIGHT_CURLY_BRACE,
    TOKEN_LEFT_BRACKET_BRACE,
    TOKEN_RIGHT_BRACKET_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_EXCLAMATION_POINT,
    TOKEN_PLUS,
    TOKEN_DOT,
    TOKEN_LEFT_ANGLE_BRACKET,
    TOKEN_RIGHT_ANGLE_BRACKET,
    TOKEN_MODULO,
    TOKEN_COMMA,
    TOKEN_DIVIDE,
    TOKEN_QUESTION_MARK,
    TOKEN_AMPERSAND,
    TOKEN_STAR,
    TOKEN_MINUS,
    TOKEN_COLON,
    TOKEN_EQUAL,
    TOKEN_PIPELINE,

    TOKEN_EQUAL_COMPARISON,
    TOKEN_LESS_THAN_EQUAL_COMPARISON,
    TOKEN_OR_COMPARISON,
    TOKEN_MINUS_EQUAL,
    TOKEN_NOT_EQUAL_COMPARISON,
    TOKEN_INCREMENT,
    TOKEN_AND_COMPARISON,
    TOKEN_MULTIPLY_EQUAL,
    TOKEN_GREATER_THAN_EQUAL_COMPARISON,
    TOKEN_DECREMENT,
    TOKEN_PLUS_EQUAL,
    TOKEN_DIVIDE_EQUAL
} token_type_enum;

typedef struct {
    size_t id;
//    string_t file_name;
    //size_t line_number;
    string_t value;
    token_type_enum type;
} token_t;

typedef struct {
    token_t *tokens;
    size_t token_count;
    string_t input;
    size_t line_number;
    size_t current_character_index;
    char current_character;
} lexer_state_t;

void lexer_state_initialize(lexer_state_t *lexer_state, string_t input);

void add_token(lexer_state_t *lexer_state, token_t *token);

void create_token(lexer_state_t *lexer_state, token_t *token, string_t value, token_type_enum token_type);

void lexer_next(lexer_state_t *lexer_state);

void lexer_ignore_whitespaces(lexer_state_t *lexer_state);

void lexer_ignore_newline(lexer_state_t *lexer_state);

void lexer_ignore_comments(lexer_state_t *lexer_state);

void lexer_preprocessor_directives(lexer_state_t *lexer_state);

void lexer_lex_letters(lexer_state_t *lexer_state);

void lexer_lex_single_symbols(lexer_state_t *lexer_state);

void lexer_lex_double_symbols(lexer_state_t *lexer_state);

void lexer_lex_integers(lexer_state_t *lexer_state);

void lexer_lex_string(lexer_state_t *lexer_state);

void lexer_get_tokens(lexer_state_t* lexer_state, token_t **token);

void lexer_printer(token_t *tokens);

#endif