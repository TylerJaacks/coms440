//
// Created by Tyler Jaacks on 3/6/22.
//

#ifndef COMS440_PARSER_H
#define COMS440_PARSER_H

#include <vector>

#include "token.h"
#include "parser_result.h"

class parser {
public:
    std::vector<token> tokens;

    int currentTokenIndex;

public:
    parser(std::vector<token> tokens) { this->tokens = tokens; currentTokenIndex = 0;}

private:
    /**
     * Gets the next token without consuming the current token.
     * @return the next token.
     */
    token peak_next_token();

    /**
     * Gets the previous token without consuming the previous token.
     * @return the previous token.
     */
    token peak_previous_token();

    /**
     * Gets the current token and advance the current token index.
     * @return the current token.
     */
    token consume_token();


public:
    /**
     * Parse the parsers token input.
     * @return the result of the parse.
     */
    parser_result parse();
};

#endif //COMS440_PARSER_H
