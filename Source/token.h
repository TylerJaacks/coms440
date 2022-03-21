//
// Created by Tyler Jaacks on 3/6/22.
//

#ifndef TESTPROJECT_TOKEN_H
#define TESTPROJECT_TOKEN_H

#include <string>

enum token_type {
    TOKEN_UNDEFINED,

    TOKEN_EOF,

    TOKEN_PREPROCESSOR_INCLUDE,
    TOKEN_PREPROCESSOR_DEFINE,
    TOKEN_PREPROCESSOR_UNDEF,
    TOKEN_PREPROCESSOR_IF_DEF,
    TOKEN_PREPROCESSOR_IF_N_DEF,
    TOKEN_PREPROCESSOR_IF,
    TOKEN_PREPROCESSOR_ELSE,
    TOKEN_PREPROCESSOR_ELIF,
    TOKEN_PREPROCESSOR_ENDIF,
    TOKEN_PREPROCESSOR_ERROR,
    TOKEN_PREPROCESSOR_PRAGMA,

    TOKEN_TYPE_INTEGER,
    TOKEN_TYPE_DOUBLE,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_CHAR,
    TOKEN_TYPE_SHORT,
    TOKEN_TYPE_LONG,
    TOKEN_TYPE_VOID,

    TOKEN_TYPE_MODIFIER_UNSIGNED,
    TOKEN_TYPE_MODIFIER_SIGNED,

    TOKEN_IDENTIFIER,

    TOKEN_LITERAL_STRING,
    TOKEN_LITERAL_NUMBER,
    TOKEN_LITERAL_REAL,
    TOKEN_LITERAL_CHAR,

    TOKEN_SYMBOL_LEFT_PAREN,
    TOKEN_SYMBOL_RIGHT_PAREN,
    TOKEN_SYMBOL_LEFT_BRACKET,
    TOKEN_SYMBOL_RIGHT_BRACKET,

    TOKEN_SYMBOL_PLUS_SIGN,
    TOKEN_SYMBOL_MINUS_SIGN,
    TOKEN_SYMBOL_ASTERISK,
    TOKEN_SYMBOL_FORWARD_SLASH,
    TOKEN_SYMBOL_PERCENT_SIGN,

    TOKEN_SYMBOL_EXCLAMATION_MARK,
    TOKEN_SYMBOL_LESS_THAN,
    TOKEN_SYMBOL_GREATER_THAN,
    TOKEN_SYMBOL_EQUAL_SIGN,
    TOKEN_SYMBOL_LEFT_BRACE,
    TOKEN_SYMBOL_RIGHT_BRACE,

    TOKEN_SYMBOL_COLON,
    TOKEN_BACKWARD_SLASH,
    TOKEN_SYMBOL_SEMICOLON,
    TOKEN_SYMBOL_AMPERSAND,
    TOKEN_SYMBOL_COMMA,
    TOKEN_SYMBOL_PERIOD,
    TOKEN_SYMBOL_PIPE,

    TOKEN_KEYWORD_AUTO,
    TOKEN_KEYWORD_BREAK,
    TOKEN_KEYWORD_CONST,
    TOKEN_KEYWORD_CASE,
    TOKEN_KEYWORD_CONTINUE,
    TOKEN_KEYWORD_DEFAULT,
    TOKEN_KEYWORD_DO,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_ENUM,
    TOKEN_KEYWORD_EXTERN,
    TOKEN_KEYWORD_FOR,
    TOKEN_KEYWORD_GOTO,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_REGISTER,
    TOKEN_KEYWORD_RETURN,
    TOKEN_KEYWORD_SIZEOF,
    TOKEN_KEYWORD_STATIC,
    TOKEN_KEYWORD_STRUCT,
    TOKEN_KEYWORD_SWITCH,
    TOKEN_KEYWORD_TYPEDEF,
    TOKEN_KEYWORD_UNION,
    TOKEN_KEYWORD_VOLATILE,
    TOKEN_KEYWORD_WHILE
};

class token {
public:
    token();

    token_type type = TOKEN_UNDEFINED;
    std::string value = "";

    int lineNumber = 0;
    std::string fileName = "";

public:
    token(token_type type, std::string value, int lineNumber, std::string fileName);
};


#endif //TESTPROJECT_TOKEN_H
