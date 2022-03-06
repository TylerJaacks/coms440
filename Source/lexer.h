//
// Created by Tyler Jaacks on 3/5/22.
//

#ifndef TESTPROJECT_LEXER_H
#define TESTPROJECT_LEXER_H

#include <string>
#include <vector>

#include "token.h"

class lexer {
public:
    std::string characters;
    int currentIndex;

    std::string fileName = "";
    int currentLine = 0;

public:
    lexer();
    lexer(std::string fileName, std::string text);

    bool isWhitespace(char c);
    bool isNewline(char c);
    bool isLetter(char c);
    bool isDigit(char c);

    void preprocessor();

    void ignoreWhitespace();
    void ignoreComments();
    void ignoreNewLine();

    std::vector<token> getTokens();

    void lex(token &token);
};


#endif //TESTPROJECT_LEXER_H
