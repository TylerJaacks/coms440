//
// Created by Tyler Jaacks on 3/6/22.
//

#ifndef COMS440_PARSER_H
#define COMS440_PARSER_H

#include <vector>

#include "token.h"

class parser {
public:
    std::vector<token> tokens;

    int currentTokenIndex;
    int totalTokens;

public:
    parser(std::vector<token> tokens);

public:
    void parse();
};


#endif //COMS440_PARSER_H
