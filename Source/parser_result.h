//
// Created by Tyler Jaacks on 3/8/22.
//

#ifndef COMS440_PARSER_RESULT_H
#define COMS440_PARSER_RESULT_H

#include <string>

class parser_result {
public:
    bool successful;

    std::string errorMessage;
    std::string fileName;

    int lineNumber;

    parser_result(bool successful, std::string errorMessage, std::string fileName, int lineNumber) {
        this->successful = successful;
        this->errorMessage = errorMessage;
        this->fileName = fileName;
        this->lineNumber = lineNumber;
    }
};


#endif //COMS440_PARSER_RESULT_H
