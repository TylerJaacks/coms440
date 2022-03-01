#include "lexer.h"
#include "util.h"
#include "parser.h"

void print_usage_statements() {
    fprintf(stderr, "Usage: \n");
    fprintf(stderr, "\t mycc -mode [options] infile\n");
    fprintf(stderr, "Valid modes: \n");
    fprintf(stderr, "\t -0: Version information only\n");
    fprintf(stderr, "\t -1: Part 1\n");
    fprintf(stderr, "\t -2: Part 2 (not yet implemented)\n");
    fprintf(stderr, "\t -3: Part 3 (not yet implemented)\n");
    fprintf(stderr, "\t -4: Part 4 (not yet implemented)\n");
    fprintf(stderr, "\t -5: Part 5 (not yet implemented)\n");
}

void print_about_statements() {
    fprintf(stdout, "My bare-bones C compiler (for COM 440/540)\n");
    fprintf(stdout, "\tWritten by Tyler Jaacks (tjaacks@iastate.edu)\n");
    fprintf(stdout, "\tVersion 2.0\n");
    fprintf(stdout, "\t28 January, 2022\n");
}

result_code_t save_about_statements(const char *filename) {
   string_t about_str = "My bare-bones C compiler (for COM 440/540)\n\tWritten by Tyler Jaacks (tjaacks@iastate.edu)\n\tVersion 2.0\n\t28 January, 2022\n";

   FILE *fp = fopen(filename, "w+");

   result_code_t result_code = fputs(about_str, fp);

   fflush(fp);
   fclose(fp);

   return result_code;
}

struct arguments {
    int8_t mode;
    string_t output_path;
    string_t input_path;
};

uint8_t parse_arguments(struct arguments *arguments, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        char first_char = argv[i][0];

        if (first_char == '-') {
            char next_char = argv[i][1];

            if (isdigit(next_char)) {
                arguments->mode = (int) next_char;
            }
            else if (next_char == 'o') {
                arguments->output_path = argv[i + 1];
                i++;
            }
        }
        else {
            arguments->input_path = argv[i];
        }
    }

    return SUCCESS;
}

result_code_t handle_arguments(struct arguments* arguments) {
    result_code_t result_code = SUCCESS;

    if (arguments->mode == -1) {
        print_usage_statements();
    }

    if (arguments->mode == '1') {
        /**
         * https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
         */
        char * buffer = 0;
        long length;

        FILE * f = fopen (arguments->input_path, "rb");

        if (f) {
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            buffer = malloc (length);
            if (buffer) {
                fread (buffer, 1, length, f);
            }
            fclose (f);
        }

        if (buffer) {
            lexer_state_t lexer_state;

            lexer_state_initialize(&lexer_state, buffer);

            token_t *token;

            if (strcmp(arguments->output_path, "a.out") != 0) {
                lexer_get_tokens(&lexer_state, &token );
            }
            else {
                freopen(arguments->output_path,"w",stdout);
                lexer_get_tokens(&lexer_state, &token);
            }
        }

        return SUCCESS;
    }

    if (isdigit(arguments->mode)) {
        if (strlen(arguments->output_path) && strlen(arguments->output_path) != 0) {
            save_about_statements(arguments->output_path);
            return ERROR;
        }

        if (strlen(arguments->output_path) == 0) {
            print_about_statements();
            return ERROR;
        }
    }

    return result_code;
}

result_code_t main(int argc, char *argv[]) {
    result_code_t result_code = 0;

//    struct arguments arguments = { -1, "a.out", "" };

//    result_code = parse_arguments(&arguments, argc, argv);
//    result_code = handle_arguments(&arguments);

    parser_state_t *parser_state = malloc(sizeof(parser_state));
    token_t *tokens = malloc(4096 * sizeof(token_t));

    token_t token = {
            .value = "int",
            .type = TOKEN_TYPE,
            .id = 0
    };

    token_t token1 = {
            .value = "a",
            .type = TOKEN_IDENTIFIER,
            .id = 1
    };

    token_t token2 = {
            .value = "=",
            .type = TOKEN_EQUAL,
            .id = 2
    };

    token_t token3 = {
            .value = "9",
            .type = TOKEN_INTEGER_LITERAL,
            .id = 3
    };

    token_t token4 = {
            .value = ";",
            .type = TOKEN_SEMICOLON,
            .id = 4
    };

    token_t token5 = {
            .value = "double",
            .type = TOKEN_TYPE,
            .id = 5
    };

    token_t token6 = {
            .value = "b",
            .type = TOKEN_IDENTIFIER,
            .id = 6
    };

    token_t token7 = {
            .value = ";",
            .type = TOKEN_SEMICOLON,
            .id = 7
    };

    token_t token8 = {
            .value = "int",
            .type = TOKEN_TYPE,
            .id = 8
    };

    token_t token9 = {
            .value = "main",
            .type = TOKEN_IDENTIFIER,
            .id = 9
    };

    token_t token10 = {
            .value = "(",
            .type = TOKEN_LEFT_PAREN,
            .id = 10
    };

    token_t token11 = {
            .value = "int",
            .type = TOKEN_TYPE,
            .id = 11
    };

    token_t token12 = {
            .value = "argc",
            .type = TOKEN_IDENTIFIER,
            .id = 12
    };

    token_t token13 = {
            .value = ",",
            .type = TOKEN_COMMA,
            .id = 13
    };

    token_t token14 = {
            .value = "char",
            .type = TOKEN_TYPE,
            .id = 14
    };

    token_t token15 = {
            .value = "argv",
            .type = TOKEN_IDENTIFIER,
            .id = 15
    };

    token_t token16 = {
            .value = ")",
            .type = TOKEN_RIGHT_PAREN,
            .id = 16
    };

    token_t token17 = {
            .value = "{",
            .type = TOKEN_LEFT_CURLY_BRACE,
            .id = 17
    };

    token_t token18 = {
            .value = "}",
            .type = TOKEN_RIGHT_CURLY_BRACE,
            .id = 18
    };

    vector_push_back(tokens, token);
    vector_push_back(tokens, token1);
    vector_push_back(tokens, token2);
    vector_push_back(tokens, token3);
    vector_push_back(tokens, token4);
    vector_push_back(tokens, token5);
    vector_push_back(tokens, token6);
    vector_push_back(tokens, token7);
    vector_push_back(tokens, token8);
    vector_push_back(tokens, token9);
    vector_push_back(tokens, token10);
    vector_push_back(tokens, token11);
    vector_push_back(tokens, token12);
    vector_push_back(tokens, token13);
    vector_push_back(tokens, token14);
    vector_push_back(tokens, token15);
    vector_push_back(tokens, token16);
    vector_push_back(tokens, token17);
    vector_push_back(tokens, token18);

    parser_state->tokens = tokens;
    parser_state->token_count = 18;
    parser_state->current_token_index = 0;

    parser_parse_program(parser_state);

    return result_code;
}