//
// Created by Tyler Jaacks on 3/6/22.
//

#include "parser.h"
#include "parser_result.h"

#include "ast.h"

token parser::peak_previous_token() {
    return tokens[currentTokenIndex - 1];
}

token parser::peak_next_token() {
    return tokens[currentTokenIndex + 1];
}

token parser::consume_token()  {
    token token = tokens[currentTokenIndex];

    currentTokenIndex += 1;

    return token;
}

parser_result parser::parse() {
    while (true) {
        if (currentTokenIndex == tokens.size()) {
            break;
        }

        token token = consume_token();

        while (token.type != TOKEN_SYMBOL_SEMICOLON) {
            printf("%s\n", token.value.c_str());

            token = consume_token();
        }

        if (peak_previous_token().type != TOKEN_SYMBOL_SEMICOLON) {
            fprintf(stderr, "ERROR: Expected a ';' but got '%s' instead. FILE: %s LINE: %i",
                    token.value.c_str(),
                    token.fileName.c_str(),
                    token.lineNumber);
        }
    }
}