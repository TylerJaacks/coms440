//
// Created by tylerjaacks on 2/28/22.
//

#ifndef COMS440_PARSER_H
#define COMS440_PARSER_H

#include "vector.h"
#include "lexer.h"

typedef struct {
    token_t *tokens;
    size_t token_count;
    size_t current_token_index;
} parser_state_t;

void advance_token(parser_state_t *state);

void retreat_token(parser_state_t *state);

void parser_parse_function_declaration(parser_state_t *state, int has_return_type);

void parser_parse_variable_declaration(parser_state_t *state);

void parser_parse_declarations(parser_state_t *state);

void parser_parse_if(parser_state_t *state);

void parser_parse_function_call(parser_state_t *state);

void parser_parse_program(parser_state_t *state);

#endif //COMS440_PARSER_H
