//
// Created by Tyler Jaacks on 3/5/22.
//

#include "lexer.h"

lexer::lexer(std::string fileName, std::string text) {
    this->fileName = fileName;
    this->characters = text;
    this->currentIndex = 0;
}

bool lexer::isWhitespace(char c) {
    if (c == ' ') {
        return true;
    }
    else if (c == '\t') {
        return true;
    }

    return false;
}

bool lexer::isNewline(char c) {
    if (c == '\n') {
        return true;
    }
    else if (c == '\r') {
        return true;
    }

    return false;
}

bool lexer::isLetter(char c) {
    if ((c >= 'A') && (c <= 'Z')) return true;
    if ((c >= 'a') && (c <= 'z')) return true;

    return false;
}

bool lexer::isDigit(char c) {
    if ((c >= '0') && (c <= '9')) return true;

    return false;
}

void lexer::ignoreWhitespace() {
    if (isWhitespace(characters[currentIndex])) {
        currentIndex++;
    }
}

void lexer::preprocessor() {
    if (characters[currentIndex] == '#') {
        std::string preprocessorDirectiveStr;


    }
}

/**
 * TODO: Handle comments with '/' that allows multiple lines for a single line comment.
 */
void lexer::ignoreComments() {
    if (characters[currentIndex] == '/' && characters[currentIndex++] == '/') {
        while (!isNewline(characters[currentIndex])) {
            currentIndex++;
        }

        currentIndex++;
    }
    else if (characters[currentIndex] == '/' && characters[currentIndex++] == '*') {
        while (characters[currentIndex] != '*' && characters[currentIndex++] != '/') {
            currentIndex++;
        }

        currentIndex += 2;
    }
}

void lexer::ignoreNewLine() {
    if (isNewline(characters[currentIndex])) {
        currentLine += 1;
        currentIndex++;
    }
}

void lexer::lex(token &token) {
    switch (characters[currentIndex]) {
        case '(':
            token.type = TOKEN_SYMBOL_LEFT_PAREN;
            token.value = "(";
            currentIndex++;
            break;
        case ')':
            token.type = TOKEN_SYMBOL_RIGHT_PAREN;
            token.value = ")";
            currentIndex++;
            break;
        case '[':
            token.type = TOKEN_SYMBOL_LEFT_BRACKET;
            token.value = "[";
            currentIndex++;
            break;
        case ']':
            token.type = TOKEN_SYMBOL_RIGHT_BRACKET;
            token.value = "]";
            currentIndex++;
            break;
        case '{':
            token.type = TOKEN_SYMBOL_LEFT_BRACE;
            token.value = "{";
            currentIndex++;
            break;
        case '}':
            token.type = TOKEN_SYMBOL_RIGHT_BRACE;
            token.value = "}";
            currentIndex++;
            break;

        case '+':
            token.type = TOKEN_SYMBOL_PLUS_SIGN;
            token.value = "+";
            currentIndex++;
            break;
        case '-':
            token.type = TOKEN_SYMBOL_MINUS_SIGN;
            token.value = "-";
            currentIndex++;
            break;
        case '*':
            token.type = TOKEN_SYMBOL_ASTERISK;
            token.value = "*";
            currentIndex++;
            break;
        case '/':
            token.type = TOKEN_SYMBOL_FORWARD_SLASH;
            token.value = "/";
            currentIndex++;
            break;
        case '%':
            token.type = TOKEN_SYMBOL_PERCENT_SIGN;
            token.value = "%";
            currentIndex++;
            break;

        case '!':
            token.type = TOKEN_SYMBOL_EXCLAMATION_MARK;
            token.value = "!";
            currentIndex++;
            break;
        case '<':
            token.type = TOKEN_SYMBOL_LESS_THAN;
            token.value = "<";
            currentIndex++;
            break;
        case '>':
            token.type = TOKEN_SYMBOL_GREATER_THAN;
            token.value = ">";
            currentIndex++;
            break;
        case '=':
            token.type = TOKEN_SYMBOL_EQUAL_SIGN;
            token.value = "=";
            currentIndex++;
            break;
        case ':':
            token.type = TOKEN_SYMBOL_COLON;
            token.value = ":";
            currentIndex++;
            break;
        case '\\':
            token.type = TOKEN_BACKWARD_SLASH;
            token.value = "\\";
            currentIndex++;
            break;
        case ';':
            token.type = TOKEN_SYMBOL_SEMICOLON;
            token.value = ";";
            currentIndex++;
            break;
        case '&':
            token.type = TOKEN_SYMBOL_AMPERSAND;
            token.value = "&";
            currentIndex++;
            break;
        case ',':
            token.type = TOKEN_SYMBOL_COMMA;
            token.value = ",";
            currentIndex++;
            break;
        case '.':
            token.type = TOKEN_SYMBOL_PERIOD;
            token.value = ".";
            currentIndex++;
            break;
        case '|':
            token.type = TOKEN_SYMBOL_PIPE;
            token.value = "|";
            currentIndex++;
            break;

        case '\"':
            /**
             * TODO: Handle Quotes in Quotes.
             */
            if (characters[currentIndex] == '\"') {
                currentIndex++;

                std::string stringLiteral;

                while (characters[currentIndex] != '\"') {
                    stringLiteral += characters[currentIndex];

                    currentIndex++;
                }

                token.type = TOKEN_LITERAL_STRING;
                token.value = stringLiteral;

                currentIndex++;
            }
        case '\'':
            if (characters[currentIndex] == '\'') {
                currentIndex++;

                std::string stringLiteral;

                while (characters[currentIndex] != '\'') {
                    stringLiteral += characters[currentIndex];

                    currentIndex++;
                }

                token.type = TOKEN_LITERAL_CHAR;
                token.value = stringLiteral;

                currentIndex++;
            }
        default:
            if (isLetter(characters[currentIndex]) || characters[currentIndex] == '_') {
                std::string identifierStr;

                while (isLetter(characters[currentIndex]) || isDigit(characters[currentIndex]) || characters[currentIndex] == '_') {
                    identifierStr += characters[currentIndex];

                    currentIndex++;
                }

                /**
                 * KEYWORDS
                 */

                /**
                 * TYPES
                 */
                if (strcmp(identifierStr.c_str(), "int") == 0) {
                    token.type = TOKEN_TYPE_INTEGER;
                    token.value = "int";
                }
                else if (strcmp(identifierStr.c_str(), "double") == 0) {
                    token.type = TOKEN_TYPE_DOUBLE;
                    token.value = "double";
                }
                else if (strcmp(identifierStr.c_str(), "float") == 0) {
                    token.type = TOKEN_TYPE_FLOAT;
                    token.value = "float";
                }
                else if (strcmp(identifierStr.c_str(), "char") == 0) {
                    token.type = TOKEN_TYPE_CHAR;
                    token.value = "char";
                }
                else if (strcmp(identifierStr.c_str(), "void") == 0) {
                    token.type = TOKEN_TYPE_VOID;
                    token.value = "void";
                }
                else if (strcmp(identifierStr.c_str(), "short") == 0) {
                    token.type = TOKEN_TYPE_SHORT;
                    token.value = "short";
                }
                else if (strcmp(identifierStr.c_str(), "long") == 0) {
                    token.type = TOKEN_TYPE_LONG;
                    token.value = "long";
                }


                else if (strcmp(identifierStr.c_str(), "signed") == 0) {
                    token.type = TOKEN_TYPE_MODIFIER_SIGNED;
                    token.value = "signed";
                }
                else if (strcmp(identifierStr.c_str(), "unsigned") == 0) {
                    token.type = TOKEN_TYPE_MODIFIER_UNSIGNED;
                    token.value = "unsigned";
                }

                else if (strcmp(identifierStr.c_str(), "auto") == 0) {
                    token.type = TOKEN_KEYWORD_AUTO;
                    token.value = "auto";
                }
                else if (strcmp(identifierStr.c_str(), "break") == 0) {
                    token.type = TOKEN_KEYWORD_BREAK;
                    token.value = "break";
                }
                else if (strcmp(identifierStr.c_str(), "const") == 0) {
                    token.type = TOKEN_KEYWORD_CONST;
                    token.value = "const";
                }
                else if (strcmp(identifierStr.c_str(), "case") == 0) {
                    token.type = TOKEN_KEYWORD_CASE;
                    token.value = "case";
                }
                else if (strcmp(identifierStr.c_str(), "continue") == 0) {
                    token.type = TOKEN_KEYWORD_CONTINUE;
                    token.value = "continue";
                }
                else if (strcmp(identifierStr.c_str(), "default") == 0) {
                    token.type = TOKEN_KEYWORD_DEFAULT;
                    token.value = "default";
                }
                else if (strcmp(identifierStr.c_str(), "do") == 0) {
                    token.type = TOKEN_KEYWORD_DO;
                    token.value = "do";
                }
                else if (strcmp(identifierStr.c_str(), "else") == 0) {
                    token.type = TOKEN_KEYWORD_ELSE;
                    token.value = "else";
                }
                else if (strcmp(identifierStr.c_str(), "enum") == 0) {
                    token.type = TOKEN_KEYWORD_ENUM;
                    token.value = "enum";
                }
                else if (strcmp(identifierStr.c_str(), "extern") == 0) {
                    token.type = TOKEN_KEYWORD_EXTERN;
                    token.value = "extern";
                }
                else if (strcmp(identifierStr.c_str(), "for") == 0) {
                    token.type = TOKEN_KEYWORD_FOR;
                    token.value = "for";
                }
                else if (strcmp(identifierStr.c_str(), "goto") == 0) {
                    token.type = TOKEN_KEYWORD_GOTO;
                    token.value = "goto";
                }
                else if (strcmp(identifierStr.c_str(), "if") == 0) {
                    token.type = TOKEN_KEYWORD_IF;
                    token.value = "if";
                }
                else if (strcmp(identifierStr.c_str(), "register") == 0) {
                    token.type = TOKEN_KEYWORD_REGISTER;
                    token.value = "register";
                }
                else if (strcmp(identifierStr.c_str(), "return") == 0) {
                    token.type = TOKEN_KEYWORD_RETURN;
                    token.value = "return";
                }
                else if (strcmp(identifierStr.c_str(), "sizeof") == 0) {
                    token.type = TOKEN_KEYWORD_SIZEOF;
                    token.value = "sizeof";
                }
                else if (strcmp(identifierStr.c_str(), "static") == 0) {
                    token.type = TOKEN_KEYWORD_STATIC;
                    token.value = "static";
                }
                else if (strcmp(identifierStr.c_str(), "struct") == 0) {
                    token.type = TOKEN_KEYWORD_STRUCT;
                    token.value = "struct";
                }
                else if (strcmp(identifierStr.c_str(), "switch") == 0) {
                    token.type = TOKEN_KEYWORD_SWITCH;
                    token.value = "switch";
                }
                else if (strcmp(identifierStr.c_str(), "typedef") == 0) {
                    token.type = TOKEN_KEYWORD_TYPEDEF;
                    token.value = "typedef";
                }
                else if (strcmp(identifierStr.c_str(), "union") == 0) {
                    token.type = TOKEN_KEYWORD_UNION;
                    token.value = "union";
                }
                else if (strcmp(identifierStr.c_str(), "volatile") == 0) {
                    token.type = TOKEN_KEYWORD_VOLATILE;
                    token.value = "volatile";
                }
                else if (strcmp(identifierStr.c_str(), "while") == 0) {
                    token.type = TOKEN_KEYWORD_WHILE;
                    token.value = "while";
                }

                else {
                    token.type = TOKEN_IDENTIFIER;
                    token.value = identifierStr;
                }
            }

            if (isDigit(characters[currentIndex])) {
                std::string valueStr;

                while (isDigit(characters[currentIndex]) || characters[currentIndex] == '.' || characters[currentIndex] == 'f') {
                    valueStr += characters[currentIndex];

                    currentIndex++;
                }

                if (characters[currentIndex - 1] == 'f') {
                    token.type = TOKEN_LITERAL_REAL;
                    token.value = valueStr;
                }
                else if (valueStr.find('.') != std::string::npos) {
                    token.type = TOKEN_LITERAL_REAL;
                    token.value = valueStr;
                }
                else if (characters[currentIndex - 1] != 'f' && valueStr.find('.') == std::string::npos) {
                    token.type = TOKEN_LITERAL_NUMBER;
                    token.value = valueStr;
                }
            }

            break;
    }

    if (token.type != TOKEN_UNDEFINED) {
        token.lineNumber = this->currentLine;
        token.fileName = this->fileName;
    }
}

std::vector<token> lexer::getTokens() {
    std::vector<token> tokens = std::vector<token>();

    while (characters[currentIndex] != '\0') {
        token token(TOKEN_UNDEFINED, "", 0, "");

        ignoreWhitespace();
        ignoreComments();
        ignoreNewLine();

        lex(token);

        if (token.type != TOKEN_UNDEFINED) tokens.push_back(token);
    }

    return tokens;
}
