#include "util.h"

void print_usage_statements() {
    fprintf(stderr, "Usage: \n");
    fprintf(stderr, "\t mycc -mode [options] infile\n");
    fprintf(stderr, "Valid modes: \n");
    fprintf(stderr, "\t -0: Version information only\n");
    fprintf(stderr, "\t -1: Part 1 (not yet implemented)\n");
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

static struct arguments {
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

    struct arguments arguments = { -1, "", "" };

    result_code = parse_arguments(&arguments, argc, argv);
    result_code = handle_arguments(&arguments);

    return result_code;
}
