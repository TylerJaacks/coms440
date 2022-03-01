#include "parser.h"

void advance_token(parser_state_t *state) {
    state->current_token_index = state->current_token_index + 1;
}

void retreat_token(parser_state_t *state) {
    state->current_token_index = state->current_token_index - 1;
}

size_t has_next_token(parser_state_t *state) {
    return state->current_token_index < state->token_count;
}

/**
 * RULE: decl -> func_decl | var_decl
 * @param state
 */
void parser_parse_declarations(parser_state_t *state) {
    token_t current_token = state->tokens[state->current_token_index];

    int has_return_type = 1;

    if (current_token.type == TOKEN_TYPE) {
        if (strcmp(current_token.value, "void") == 0) {
            has_return_type = 0;
        }

        advance_token(state);
        current_token = state->tokens[state->current_token_index];

        if (current_token.type == TOKEN_IDENTIFIER) {
            advance_token(state);
            current_token = state->tokens[state->current_token_index];

            if (current_token.type == TOKEN_LEFT_PAREN) {
                advance_token(state);

                parser_parse_function_declaration(state, has_return_type);
            }
            else if (current_token.type == TOKEN_EQUAL || current_token.type == TOKEN_SEMICOLON) {
                parser_parse_variable_declaration(state);
            }
        }
    }
}

/**
 * RULE: func_decl -> type id(type id...); | type id(type id...) { expr* }
 * @param state
 */
void parser_parse_function_declaration(parser_state_t *state, int has_return_type) {
    token_t current_token = state->tokens[state->current_token_index];

    int i = 0;

    while (i != 253) {
        if (current_token.type == TOKEN_RIGHT_PAREN) {
            break;
        }

        if (state->tokens[state->current_token_index - 1].type == TOKEN_LEFT_PAREN) {
            if (current_token.type != TOKEN_TYPE) {
                fprintf(stderr, "ERROR: Expected a Type in Function Declaration Parameter List.\n");
                exit(-1);
            }
        }
        else if (state->tokens[state->current_token_index - 1].type == TOKEN_IDENTIFIER) {
            if (current_token.type != TOKEN_COMMA) {
                fprintf(stderr, "ERROR: Expected a Comma in Function Declaration Parameter List.\n");
                exit(-1);
            }
        }
        else if (state->tokens[state->current_token_index - 1].type == TOKEN_IDENTIFIER) {
            if (current_token.type != TOKEN_COMMA) {
                fprintf(stderr, "ERROR: Expected an Identifier in Function Declaration Parameter List.\n");
                exit(-1);
            }
        }
        else if (state->tokens[state->current_token_index - 1].type == TOKEN_COMMA) {
            if (current_token.type != TOKEN_TYPE) {
                fprintf(stderr, "ERROR: Expected an Type in Function Declaration Parameter List.\n");
                exit(-1);
            }
        }

        advance_token(state);
        current_token = state->tokens[state->current_token_index];
        i++;
    }

    advance_token(state);
    current_token = state->tokens[state->current_token_index];

    // TODO: Come back to this.
    if (current_token.type == TOKEN_LEFT_CURLY_BRACE) {
        advance_token(state);
        current_token = state->tokens[state->current_token_index];

        int has_seen_return = 0;

        while (1) {
            if (current_token.type == TOKEN_RIGHT_PAREN) {
                break;
            }

            // Handle Variable Declarations
            if (current_token.type == TOKEN_TYPE) {
                if (strcmp(current_token.value, "void") == 0) {
                    has_return_type = 0;
                }

                advance_token(state);
                current_token = state->tokens[state->current_token_index];

                if (current_token.type == TOKEN_IDENTIFIER) {
                    advance_token(state);
                    current_token = state->tokens[state->current_token_index];

                    if (current_token.type == TOKEN_EQUAL || current_token.type == TOKEN_SEMICOLON) {
                        parser_parse_variable_declaration(state);
                    }
                }
            }

            parser_parse_function_call(state);

            if (has_return_type) {
                if (current_token.type == TOKEN_RETURN) {
                    advance_token(state);
                    current_token = state->tokens[state->current_token_index];

                    if (!(current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_INTEGER_LITERAL || current_token.type == TOKEN_DOUBLE_LITERAL || current_token.type == TOKEN_STRING_LITERAL || current_token.type == TOKEN_CHARACTER_LITERAL)) {
                        fprintf(stderr, "ERROR: Expected a return expression.");
                        exit(-1);
                    }

                    has_seen_return = 1;
                }
            }
        }

        if (has_return_type && !has_seen_return) {
            fprintf(stderr, "ERROR: Expected a return expression but saw none.");
        }
    }
    else if (current_token.type != TOKEN_SEMICOLON) {
        fprintf(stderr, "ERROR: Expected a ; or { in Function Declaration Parameter List.\n");
        exit(-1);
    }
}

/**
 * RULE: var_decl -> type id; | type id = val
 * @param state
 */
void parser_parse_variable_declaration(parser_state_t *state) {
    token_t current_token = state->tokens[state->current_token_index];

    if (current_token.type == TOKEN_SEMICOLON) {
        advance_token(state);
    }
    else if (current_token.type == TOKEN_EQUAL) {
        advance_token(state);
        current_token = state->tokens[state->current_token_index];

        if (current_token.type == TOKEN_CHARACTER_LITERAL || TOKEN_STRING_LITERAL || TOKEN_DOUBLE_LITERAL || TOKEN_INTEGER_LITERAL || TOKEN_IDENTIFIER) {
            advance_token(state);
            current_token = state->tokens[state->current_token_index];

            if (current_token.type != TOKEN_SEMICOLON) {
                fprintf(stderr, "ERROR: Expected that the line be terminated with a semicolon.");
                exit(-1);
            }

            advance_token(state);
        }
    }
}

// TODO: Come back to this.
void parser_parse_function_call(parser_state_t *state) {
    token_t current_token = state->tokens[state->current_token_index];

    if (current_token.type == TOKEN_IDENTIFIER) {
        advance_token(state);
        current_token = state->tokens[state->current_token_index];

        if (current_token.type != TOKEN_LEFT_PAREN) {
            fprintf(stderr, "ERROR: Expected a '(' in the function call.");
            exit(-1);
        }

        advance_token(state);
        current_token = state->tokens[state->current_token_index];

        int i = 0;

        while (i != 253) {
            if (current_token.type == TOKEN_RIGHT_PAREN) {
                break;
            }

            if (current_token.type != TOKEN_CHARACTER_LITERAL || current_token.type != TOKEN_STRING_LITERAL || current_token.type != TOKEN_DOUBLE_LITERAL || current_token.type != TOKEN_INTEGER_LITERAL || current_token.type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "ERROR: Expected function arguments to be of literals or identifiers.");
                exit(-1);
            }

            advance_token(state);
            current_token = state->tokens[state->current_token_index];
            i++;
        }
    }
}

void parser_parse_if(parser_state_t *state) {

}

void parser_parse_program(parser_state_t *state) {
    while (has_next_token(state)) {
        parser_parse_declarations(state);
    }
}