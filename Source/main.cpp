#include <string>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "typechecker.h"

#define SUCCESS 0
#define ERROR -1

void print_usage_statements() {
    fprintf(stderr, "Usage: \n");
    fprintf(stderr, "\t mycc -mode [options] infile\n");
    fprintf(stderr, "Valid modes: \n");
    fprintf(stderr, "\t -0: Version information only\n");
    fprintf(stderr, "\t -1: Part 1\n");
    fprintf(stderr, "\t -2: Part 2\n");
    fprintf(stderr, "\t -3: Part 3\n");
    fprintf(stderr, "\t -4: Part 4\n");
    fprintf(stderr, "\t -5: Part 5\n");
}

void print_about_statements() {
    fprintf(stdout, "My bare-bones C compiler (for COM 440/540)\n");
    fprintf(stdout, "\tWritten by Tyler Jaacks (tjaacks@iastate.edu)\n");
    fprintf(stdout, "\tVersion 2.0\n");
    fprintf(stdout, "\t28 January, 2022\n");
}

void save_about_statements(std::string filename) {
    std::string about_str = "My bare-bones C compiler (for COM 440/540)\n\tWritten by Tyler Jaacks (tjaacks@iastate.edu)\n\tVersion 2.0\n\t28 January, 2022\n";

    FILE *fp = fopen(filename.c_str(), "w+");

    fputs(about_str.c_str(), fp);

    fflush(fp);
    fclose(fp);
}

struct arguments {
    int8_t mode;
    std::string output_path;
    std::string input_path;
};

void parse_arguments(struct arguments *arguments, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        char first_char = argv[i][0];

        if (first_char == '-') {
            char next_char = argv[i][1];

            if (isdigit(next_char)) {
                arguments->mode = (int) next_char;
            } else if (next_char == 'o') {
                arguments->output_path = argv[i + 1];
                i++;
            }
        } else {
            arguments->input_path = argv[i];
        }
    }
}

void handle_arguments(struct arguments *arguments) {
    if (arguments->mode == -1) {
        print_usage_statements();
    }

    if      (arguments->mode == '0') {
        if (strcmp(arguments->output_path.c_str(), "a.out") == 0) {
            print_about_statements();
        }
        else if (strcmp(arguments->output_path.c_str(), "a.out") != 0) {
            save_about_statements(arguments->output_path.c_str());
        }
    }
    else if (arguments->mode == '1') {
        char *buffer = nullptr;
        long length;

        FILE *f = fopen(arguments->input_path.c_str(), "rb");

        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char*) malloc(length);
            if (buffer) {
                fread(buffer, 1, length, f);
            }
            fclose(f);
        }

        if (buffer) {
            if (arguments->output_path == "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);

                std::vector<token> tokens = program_lexer.getTokens();

                for (int i = 0; i < tokens.size() - 1; i++) {
                    fprintf(stdout, "File %s Line %i Token %i Text %s\n", tokens[i].fileName.c_str(), tokens[i].lineNumber, tokens[i].type, tokens[i].value.c_str());
                }

                exit(SUCCESS);
            }
            else if (arguments->output_path != "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);

                std::vector<token> tokens = program_lexer.getTokens();

                FILE *f = fopen(arguments->output_path.c_str(), "w+");

                for (int i = 0; i < tokens.size() - 1; i++) {
                    fprintf(f, "File %s Line %i Token %i Text %s\n", tokens[i].fileName.c_str(), tokens[i].lineNumber, tokens[i].type, tokens[i].value.c_str());
                }

                fclose(f);

                exit(SUCCESS);
            }
        }
    }
<<<<<<< HEAD
    else if (arguments->mode == '2') {
<<<<<<< Updated upstream
        char *buffer = 0;
=======
    else if (arguments->mode == '3') {
        char *buffer = nullptr;
>>>>>>> 4a12976cb3e260456003cc085d1bc0dec2066604
        long length;

        FILE *f = fopen(arguments->input_path.c_str(), "rb");

        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char*) malloc(length);
            if (buffer) {
                fread(buffer, 1, length, f);
            }
            fclose(f);
        }

        if (buffer) {
            if (arguments->output_path == "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);
=======
        std::string testProgram = "int a, b = 0;\n"
                                  "double d;\n"
                                  "\n"
                                  "char c = 'c';\n"
                                  "\n"
                                  "void printf(char t);\n"
                                  "\n"
                                  "int main()\n"
                                  "{\n"
                                  "    if (true)\n"
                                  "        printf(\"Hello, world!\");\n"
                                  "    else {\n"
                                  "    }\n"
                                  "\n"
                                  "    return 1;\n"
                                  "}";
>>>>>>> Stashed changes

                std::vector<token> tokens = program_lexer.getTokens();

                parser program_parser(tokens);

                program_parser.Program();

<<<<<<< HEAD
<<<<<<< Updated upstream
=======
                fprintf(stdout, "File %s is syntactically correct.", arguments->input_path.c_str());

>>>>>>> 4a12976cb3e260456003cc085d1bc0dec2066604
                exit(SUCCESS);
            }
            else if (arguments->output_path != "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);

                std::vector<token> tokens = program_lexer.getTokens();

                FILE *f = fopen(arguments->output_path.c_str(), "w+");

                parser program_parser(tokens);

                program_parser.Program();

                fprintf(f, "File %s is syntactically correct.", arguments->input_path.c_str());

                exit(SUCCESS);
            }
        }
=======
        parser.S();
>>>>>>> Stashed changes
    }
    else if (arguments->mode == '4') {
        char *buffer = nullptr;
        long length;

        FILE *f = fopen(arguments->input_path.c_str(), "rb");

        if (f) {
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char*) malloc(length);
            if (buffer) {
                fread(buffer, 1, length, f);
            }
            fclose(f);
        }

        if (buffer) {
            if (arguments->output_path == "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);

                std::vector<token> tokens = program_lexer.getTokens();

                typechecker program_typechecker(tokens);

                program_typechecker.Program();

                if (program_typechecker.type_error_list.empty()) {
                    for (auto & i : program_typechecker.type_decl_list) {
                        fprintf(stdout, "%s", i.c_str());
                    }
                }
                else if (!program_typechecker.type_error_list.empty()) {
                    for (auto & i : program_typechecker.type_error_list) {
                        fprintf(stderr, "%s", i.c_str());
                    }
                }

                exit(SUCCESS);
            }
            else if (arguments->output_path != "a.out") {
                lexer program_lexer = lexer(arguments->input_path, buffer);

                std::vector<token> tokens = program_lexer.getTokens();

                FILE *fileOutput = fopen(arguments->output_path.c_str(), "w+");

                typechecker program_typechecker(tokens);

                program_typechecker.Program();

                if (program_typechecker.type_error_list.empty()) {
                    for (int i = 0; i < program_typechecker.type_decl_list.size() - 1; i++) {
                        fprintf(fileOutput, "%s", program_typechecker.type_decl_list[i].c_str());
                    }
                }
                else if (!program_typechecker.type_error_list.empty()) {
                    for (int i = 0; i < program_typechecker.type_error_list.size() - 1; i++) {
                        fprintf(fileOutput, "%s", program_typechecker.type_error_list[i].c_str());
                    }
                }

                exit(SUCCESS);
            }
        }
    }

    if (isdigit(arguments->mode)) {
        if (strlen(arguments->output_path.c_str()) && strlen(arguments->output_path.c_str()) != 0) {
            save_about_statements(arguments->output_path);
            exit(ERROR);
        }

        if (strlen(arguments->output_path.c_str()) == 0) {
            print_about_statements();
            exit(ERROR);
        }
    }
}

int main(int argc, char *argv[]) {
    struct arguments arguments = {-1, "a.out", ""};

    parse_arguments(&arguments, argc, argv);
    handle_arguments(&arguments);

    return SUCCESS;
}