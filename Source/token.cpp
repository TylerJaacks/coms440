//
// Created by Tyler Jaacks on 3/6/22.
//

#include "token.h"

token::token(token_type type, std::string value, int lineNumber, std::string fileName) {
    this->type = type;
    this->value = value;
    this->lineNumber = lineNumber;
    this->fileName = fileName;
}

token::token() { }
