#include "lexer.h"
#include "util.h"

void lexer_state_initialize(lexer_state_t *lexer_state, string_t input) {
    lexer_state->token_count = 0;
    lexer_state->input = input;
    lexer_state->line_number = 0;
    lexer_state->current_character_index = 0;
    lexer_state->current_character = lexer_state->input[lexer_state->current_character_index];

    lexer_state->tokens = NULL;
}

void add_token(lexer_state_t *lexer_state, token_t *token) {
    vector_push_back(lexer_state->tokens, *token);
}

void create_token(lexer_state_t *lexer_state, token_t *token, string_t value, token_type_enum token_type) {
    size_t token_count = lexer_state->token_count + 1;
    size_t line_number = lexer_state->line_number;
    string_t token_value_string = value;


    lexer_state->token_count = token_count;
    //token->line_number = line_number;
    token->id = token_count;
    token->value = token_value_string;
    token->type = token_type;
}

void lexer_next(lexer_state_t *lexer_state) {
    if (lexer_state->current_character != '\0' && lexer_state->current_character_index < strlen(lexer_state->input)) {
        lexer_state->current_character_index = lexer_state->current_character_index + 1;
        lexer_state->current_character = lexer_state->input[lexer_state->current_character_index];
    }
}

void lexer_ignore_whitespaces(lexer_state_t *lexer_state) {
    if (lexer_state->current_character == ' ') {
        lexer_next(lexer_state);
    }
}

void lexer_ignore_newline(lexer_state_t *lexer_state) {
    if (lexer_state->current_character == '\n') {
        lexer_next(lexer_state);
        lexer_state->line_number = lexer_state->line_number + 1;
    }
}

void lexer_ignore_comments(lexer_state_t *lexer_state) {
    if (lexer_state->current_character == '/' && lexer_state->input[lexer_state->current_character_index + 1] == '/') {
        while (lexer_state->current_character != '\n') {
            if (lexer_state->current_character == '\n') lexer_state->line_number = lexer_state->line_number + 1;

            lexer_next(lexer_state);
        }
    } else if (lexer_state->current_character == '/' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '*') {
        lexer_next(lexer_state);
        lexer_next(lexer_state);

        if (lexer_state->current_character != '\0' &&
            lexer_state->current_character_index < strlen(lexer_state->input)) {
            lexer_state->current_character_index = lexer_state->current_character_index + 1;
            lexer_state->current_character = lexer_state->input[lexer_state->current_character_index];
        }

        if (lexer_state->current_character != '\0' &&
            lexer_state->current_character_index < strlen(lexer_state->input)) {
            lexer_state->current_character_index = lexer_state->current_character_index + 1;
            lexer_state->current_character = lexer_state->input[lexer_state->current_character_index];
        }

        while (lexer_state->current_character != '*') {
            lexer_next(lexer_state);

            while (lexer_state->current_character != '\n') {
                lexer_next(lexer_state);
            }

            if (lexer_state->current_character == '\n') {
                lexer_next(lexer_state);
                lexer_state->line_number = lexer_state->line_number + 1;
            }

            lexer_next(lexer_state);

            if (lexer_state->current_character == '/') {
                lexer_next(lexer_state);

                if (lexer_state->current_character == '\n') {
                    lexer_next(lexer_state);
                    lexer_state->line_number = lexer_state->line_number + 1;
                }

                break;
            }
        }
    }
}

void lexer_preprocessor_directives(lexer_state_t *lexer_state) {
    if (lexer_state->current_character == '#') {
        lexer_next(lexer_state);

        string_t preprocessor_directive_type = calloc(1024, sizeof(string_t));

        int i = 0;

        while (lexer_state->current_character != ' ') {
            preprocessor_directive_type[i] = lexer_state->current_character;
            lexer_next(lexer_state);
            i++;
        }

        lexer_next(lexer_state);

        if (strcmp(preprocessor_directive_type, "include") == 0) {
            lexer_next(lexer_state);

            string_t include_file = malloc(sizeof(string_t) * 1024);

            int i = 0;

            while (lexer_state->current_character != '>') {
                include_file[i] = lexer_state->current_character;
                lexer_next(lexer_state);
                i++;
            }

            lexer_next(lexer_state);

            token_t *token = malloc(sizeof(token_t));

            create_token(lexer_state, token, include_file, TOKEN_PREPROCESSOR_DIRECTIVE_INCLUDE);
            add_token(lexer_state, token);
        }

        free(preprocessor_directive_type);
    }
}

void lexer_lex_letters(lexer_state_t *lexer_state) {
    string_t collected_str = calloc(1024, sizeof(string_t));

    int i = 0;

    token_t *token = NULL;

    while ((lexer_state->current_character >= 'a' && lexer_state->current_character <= 'z') ||
           (lexer_state->current_character >= 'A' && lexer_state->current_character <= 'Z')) {
        collected_str[i] = lexer_state->current_character;

        if (strcmp(collected_str, "int") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "int", TOKEN_TYPE);
        } else if (strcmp(collected_str, "double") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "double", TOKEN_TYPE);
        } else if (strcmp(collected_str, "float") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "float", TOKEN_TYPE);
        } else if (strcmp(collected_str, "char") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "char", TOKEN_TYPE);
        } else if (strcmp(collected_str, "void") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "void", TOKEN_TYPE);
        } else if (strcmp(collected_str, "const") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "const", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "while") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "while", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "else") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "else", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "return") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "return", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "default") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "default", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "struct") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "struct", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "do") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "do", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "break") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "break", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "switch") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "switch", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "for") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "for", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "if") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "if", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "continue") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, "continue", TOKEN_KEYWORD);
        } else if (strcmp(collected_str, "case") == 0) {
            token = malloc(sizeof(token_t));

            create_token(lexer_state, token, collected_str, TOKEN_INTEGER_LITERAL);
        }

        if (token) {
            add_token(lexer_state, token);
            collected_str = calloc(2048, sizeof(string_t));
            lexer_next(lexer_state);
            break;
        }

        lexer_next(lexer_state);

        i++;
    }

    if (strcmp(collected_str, "") != 0) {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, collected_str, TOKEN_IDENTIFIER);
        add_token(lexer_state, token);
    }
}

void lexer_lex_single_symbols(lexer_state_t *lexer_state) {
    token_t *token = NULL;

    if (lexer_state->current_character == '(') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "(", TOKEN_LEFT_PAREN);
    } else if (lexer_state->current_character == ')') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, ")", TOKEN_RIGHT_PAREN);
    } else if (lexer_state->current_character == '{') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "{", TOKEN_LEFT_CURLY_BRACE);
    } else if (lexer_state->current_character == '}') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "}", TOKEN_RIGHT_CURLY_BRACE);
    } else if (lexer_state->current_character == '[') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "[", TOKEN_LEFT_BRACKET_BRACE);
    } else if (lexer_state->current_character == ']') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "]", TOKEN_RIGHT_BRACKET_BRACE);
    } else if (lexer_state->current_character == ';') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, ";", TOKEN_SEMICOLON);
    } else if (lexer_state->current_character == '!') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "!", TOKEN_EXCLAMATION_POINT);
    } else if (lexer_state->current_character == '+') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "+", TOKEN_PLUS);
    } else if (lexer_state->current_character == '.') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, ".", TOKEN_DOT);
    } else if (lexer_state->current_character == '<') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "<", TOKEN_LEFT_ANGLE_BRACKET);
    } else if (lexer_state->current_character == '>') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, ">", TOKEN_RIGHT_ANGLE_BRACKET);
    } else if (lexer_state->current_character == '%') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "%", TOKEN_MODULO);
    } else if (lexer_state->current_character == ',') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, ",", TOKEN_COMMA);
    } else if (lexer_state->current_character == '/') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "/", TOKEN_DIVIDE);
    } else if (lexer_state->current_character == '?') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "?", TOKEN_QUESTION_MARK);
    } else if (lexer_state->current_character == '&') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "&", TOKEN_AMPERSAND);
    } else if (lexer_state->current_character == '*') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "*", TOKEN_STAR);
    } else if (lexer_state->current_character == '-') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "-", TOKEN_MINUS);
    } else if (lexer_state->current_character == ':') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, ":", TOKEN_COLON);
    } else if (lexer_state->current_character == '=') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "=", TOKEN_EQUAL);
    } else if (lexer_state->current_character == '|') {
        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, "|", TOKEN_PIPELINE);
    }

    if (token) {
        add_token(lexer_state, token);
        lexer_next(lexer_state);
    }
}

void lexer_lex_double_symbols(lexer_state_t *lexer_state) {
    token_t *token = NULL;

    if (lexer_state->current_character == '=' && lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "==", TOKEN_EQUAL_COMPARISON);
    } else if (lexer_state->current_character == '<' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "<=", TOKEN_LESS_THAN_EQUAL_COMPARISON);
    } else if (lexer_state->current_character == '|' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '|') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "||", TOKEN_OR_COMPARISON);
    } else if (lexer_state->current_character == '-' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "-=", TOKEN_MINUS_EQUAL);
    } else if (lexer_state->current_character == '!' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "!=", TOKEN_NOT_EQUAL_COMPARISON);
    } else if (lexer_state->current_character == '+' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '+') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "++", TOKEN_INCREMENT);
    } else if (lexer_state->current_character == '&' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '&') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "&&", TOKEN_AND_COMPARISON);
    } else if (lexer_state->current_character == '*' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "*=", TOKEN_MULTIPLY_EQUAL);
    } else if (lexer_state->current_character == '>' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, ">=", TOKEN_GREATER_THAN_EQUAL_COMPARISON);
    } else if (lexer_state->current_character == '-' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '-') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "--", TOKEN_DECREMENT);
    } else if (lexer_state->current_character == '+' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "+=", TOKEN_PLUS_EQUAL);
    } else if (lexer_state->current_character == '/' &&
               lexer_state->input[lexer_state->current_character_index + 1] == '=') {
        token = malloc(sizeof(token));

        create_token(lexer_state, token, "/=", TOKEN_DIVIDE_EQUAL);
    }

    if (token) {
        add_token(lexer_state, token);
        lexer_next(lexer_state);
    }
}

void lexer_lex_integers(lexer_state_t *lexer_state) {
    if (lexer_state->current_character >= '0' && lexer_state->current_character <= '9') {
        string_t collected_str = calloc(2048, sizeof(string_t));

        int i = 0;

        token_t *token = NULL;

        while ((lexer_state->current_character >= '0' && lexer_state->current_character <= '9')) {
            collected_str[i] = lexer_state->current_character;

            lexer_next(lexer_state);

            i++;
        }

        token = malloc(sizeof(token_t));

        create_token(lexer_state, token, collected_str, TOKEN_INTEGER_LITERAL);

        add_token(lexer_state, token);
    }
}

void lexer_lex_string(lexer_state_t *lexer_state) {
    if (lexer_state->current_character == '\"') {
        string_t string = calloc(2048, sizeof(string_t));

        lexer_next(lexer_state);

        int i = 0;

        while (lexer_state->current_character != '\"') {
            string[i] = lexer_state->current_character;
            lexer_next(lexer_state);

            i++;
        }

        token_t *token = malloc(sizeof(token_t));

        create_token(lexer_state, token, string, TOKEN_STRING_LITERAL);
        add_token(lexer_state, token);

        lexer_next(lexer_state);
    }
}

void lexer_get_tokens(lexer_state_t *lexer_state, token_t **token) {
    while (lexer_state->current_character != '\0' && lexer_state->current_character < strlen(lexer_state->input)) {
        lexer_ignore_whitespaces(lexer_state);
        lexer_ignore_newline(lexer_state);
        lexer_ignore_comments(lexer_state);
        lexer_preprocessor_directives(lexer_state);
        lexer_lex_letters(lexer_state);
        lexer_lex_double_symbols(lexer_state);
        lexer_lex_single_symbols(lexer_state);
        lexer_lex_integers(lexer_state);
        lexer_lex_string(lexer_state);
    }

    for (int i = 0; i < vector_size(lexer_state->tokens); i++) {
        printf("File: %s Line: %i Id: %i Token: %i Value: %s\n", "File.c", 0, lexer_state->tokens[i].id, lexer_state->tokens[i].type, lexer_state->tokens[i].value);
    }
}