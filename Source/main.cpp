#include <vector>

#include "lexer.h"

std::string programText = "#include \n # include \n int main() \n { \n int b = 1; \n double b = 3.0; \n float f = 4.0f; \n const char** c = \"Hello\"; \n } \n";
std::string fileName = "Hello.c";

std::string test1 =
                    "/*\n"
                    " * Multiline comment\n"
                    " * \n"
                    " */\n"
                    "\n"
                    "// Single line comment\n"
                    "int a = 1;\n"
                    "int b = 2;\n"
                    "\n"
                    "double c = 3.14;\n"
                    "\n"
                    "int d, e, f;\n"
                    "\n"
                    "int g, h, i = 2;\n"
                    "\n"
                    "char j = 'a';\n"
                    "\n"
                    "void test();\n"
                    "\n"
                    "void test2(int a, int b) {\n"
                    "    printf(\"%d\\n\", a + b);\n"
                    "}\n"
                    "\n"
                    "int main() {\n"
                    "    printf(\"Hello World\\n\");\n"
                    "\n"
                    "    printf(\"%d\\n\", a);\n"
                    "\n"
                    "    return 1;\n"
                    "}";

int main() {
    auto program_lexer = new class lexer(fileName, test1);

    std::vector<token> program_tokens = program_lexer->getTokens();

    for (int i = 0; i < program_tokens.size(); i++) {
        printf("FILE: %s LINE: %i  TOKEN: %i VALUE: %s\n", program_tokens[i].fileName.c_str(), program_tokens[i].lineNumber, program_tokens[i].type, program_tokens[i].value.c_str());
    }

    return 1;
}