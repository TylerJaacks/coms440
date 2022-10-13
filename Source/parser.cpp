/**
 * TODO:
 * QUESTIONS:
 * 1. How do I handle Block of code braces i.e. if we have a single statement we don't need to use braces for while loops, for loops, etc. (ite1.c, ite3.c, loop3.c, loop4.c loop6.c loop8.c)?
 * 2. I have an error were the int a; a = 2 : x; where the precedence function allows this (exprs5.c)?
 * 3. Do I need to install the standard library functions?
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "parser.h"

#include <utility>

void parser::error(const std::string &expectedToken, const std::string &tokenValue, const std::string &fileName,
                   int lineNumber) {

    if (tokenValue == "EOF") {
        std::fprintf(stderr, "Parser error in file %s line %i near end of file\n\t Expected '%s'", fileName.c_str(),
                     lineNumber,
                     expectedToken.c_str());
    } else {
        std::fprintf(stderr, "Parser error in file %s line %i near text %s\n\t Expected '%s'", fileName.c_str(),
                     lineNumber,
                     tokenValue.c_str(), expectedToken.c_str());
    }

    exit(EXIT_FAILURE);
}

void parser::error_no_quotes(const std::string &expectedToken, const std::string &tokenValue, const std::string &fileName,
                        int lineNumber) {
    if (tokenValue == "EOF") {
        std::fprintf(stderr, "Parser error in file %s line %i near end of file\n\t Expected %s", fileName.c_str(),
                     lineNumber,
                     expectedToken.c_str());
    } else {
        std::fprintf(stderr, "Parser error in file %s line %i near text %s\n\t Expected %s", fileName.c_str(),
                     lineNumber,
                     tokenValue.c_str(), expectedToken.c_str());
    }

    exit(EXIT_FAILURE);
}

token parser::peak_next_token() {
    return tokens[currentTokenIndex + 1];
}

<<<<<<< HEAD
<<<<<<< Updated upstream
token parser::peak(int n) {
    return tokens[currentTokenIndex + n];
}

=======
>>>>>>> 4a12976cb3e260456003cc085d1bc0dec2066604
token parser::consume() {
    token token;

    if (currentTokenIndex != -1) {
        token = tokens[currentTokenIndex];
    }

=======
void parser::consume_token()  {
>>>>>>> Stashed changes
    currentTokenIndex += 1;
}

token parser::rollback() {
    token token;

    if (currentTokenIndex != -1) {
        token = tokens[currentTokenIndex];
    }

    currentTokenIndex -= 1;

    return token;
}

bool parser::match(token_type type) {
    if (peak_next_token().type == type) {
        return true;
    }

    return false;
}

<<<<<<< Updated upstream
// Program := Decl DecList | Decl
void parser::Program() {
    if (peak_next_token().type == TOKEN_EOF)
        return;
=======
// Program := DeclList Decl
void parser::Program() {
    DeclList();
}
>>>>>>> Stashed changes

    Decl();
    DeclList();
}

// <Decl> := <VarDecl> | <FuncProto> | <FuncDef>
void parser::Decl() {
<<<<<<< HEAD
<<<<<<< Updated upstream
    if (peak_next_token().type == TOKEN_EOF)
=======
    if (peak_next_token().type == TOKEN_EOF) {
>>>>>>> 4a12976cb3e260456003cc085d1bc0dec2066604
        return;
    }

    // <Decl> := <VarDecl> | <FuncDecl>
    if (peak_next_token().type == TOKEN_TYPE) {
        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
                rollback();
                rollback();

                FuncDecl();

                return;
            } else if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON || TOKEN_SYMBOL_COMMA) {
                rollback();
                rollback();

                VarDecl();

                return;
            }
        }
    } else {
        error("<VarDecl> or <FuncProto>",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
=======
    VarDecl();
    FuncDecl();
>>>>>>> Stashed changes
}

// <DeclList> := <Decl> <DeclList>
void parser::DeclList() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

<<<<<<< Updated upstream
    Decl();
    DeclList();
=======
    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume_token();
    }
    else {
        fprintf(stderr, "ERROR: Expected a ';' but got a %s at the end of a variable declaration.", peak_next_token().value.c_str());
    }

    return;
>>>>>>> Stashed changes
}

// <VarDecl> := <Type> <VarIdList> ';'
void parser::VarDecl() {
    type_t declType = Type();

    VarDeclList(declType);

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return;
    } else {
        error_no_quotes("',' or ';'",
                        peak_next_token().value,
                        peak_next_token().fileName,
                        peak_next_token().lineNumber);
    }
}

// <VarDeclIdList> := <VarDeclId> | <VarDeclId> ',' <VarDeclIdList>
void parser::VarDeclList(type_t type) {
    VarDeclId(type);

    if (match(TOKEN_SYMBOL_COMMA)) {
        consume();

        VarDeclList(type);
    }
}

// <VarDeclId> := <Id> | <Id> '[' <NumberLiteral>']'
void parser::VarDeclId(type_t type) {
    Id();

    if (global) {
        global_variables[tokens[currentTokenIndex].value] = type;
    } else {
        local_variables[tokens[currentTokenIndex].value] = type;
    }

    if (match(TOKEN_SYMBOL_LEFT_BRACKET)) {
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            error("identifier (within expression)",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        if (!match(TOKEN_LITERAL_NUMBER)) {
            error("<Integer Constant>", peak_next_token().value, peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        if (!match(TOKEN_SYMBOL_RIGHT_BRACKET)) {
            error("]", peak_next_token().value, peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();
    }
}

void parser::FuncDecl() {
    type_t type = Type();
    std::string id = Id();

    auto *func_params = new std::vector<function_parameter_t>();

    if (peak_next_token().type != TOKEN_SYMBOL_LEFT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    } else {
        consume();
    }

    FuncDeclParamList(func_params);

    function_t function = {.name = id, .return_type = type, .parameters = *func_params};

    for (auto p: function.parameters) {
        local_variables[p.name] = p.type;
    }

    functions.push_back(function);

    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    } else {
        consume();
    }

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return;
    } else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACE) {
        Block();

        return;
    } else {
        error("{' or ';",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
}

// FuncDeclParamList := e | FuncDeclParam | FuncDeclParam ',' FuncDeclParamList
void parser::FuncDeclParamList(std::vector<function_parameter_t> *function_parameters) {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        type_t type = Type();
        std::string id = Id();

        function_parameter_t param = {.type = type, .name = id};

        function_parameters->push_back(param);

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
                error("identifier (within expression)",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                consume();
            } else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                FuncDeclParamList(function_parameters);
            } else {
                error("<Type>",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    } else {
        return;
    }
}

// Block := '{' Stmt StmtList '}'
void parser::Block() {
    if (match(TOKEN_SYMBOL_LEFT_BRACE)) {
        consume();

        global = false;

        StmtList();

        if (match(TOKEN_SYMBOL_RIGHT_BRACE)) {
            consume();
        } else {
            error("}",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        global = true;

        local_variables.clear();
    } else {
        error("{",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
}

void parser::Stmt() {
    // <Stmt> := ';'
    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();
        return;
    }

        // <Stmt> := 'break' ';' | 'continue' ';'
    else if (peak_next_token().type == TOKEN_KEYWORD_BREAK || peak_next_token().type == TOKEN_KEYWORD_CONTINUE) {
        consume();

        if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
            error(";",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        return;
    }

        // <Stmt> := 'return' Expr ';' | 'return' ';'
    else if (peak_next_token().type == TOKEN_KEYWORD_RETURN) {
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
            consume();

            return;
        }

        // TODO: Get the Type of this return.
        type_t type = Expr();

        if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
            error(";",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        return;
    }

        // <IfStmt> := 'if' '(' Expr ')' <Block>
        //    | 'if' '(' Expr ')' <Block> 'else' <IfStmt>*
        //    | 'if' '(' Expr ')' <Block> 'else' <IfStmt>* else <Block>
    else if (peak_next_token().type == TOKEN_KEYWORD_IF) {
        IfLoop:
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            } else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Block();

            if (peak_next_token().type == TOKEN_KEYWORD_ELSE) {
                consume();

                if (peak_next_token().type != TOKEN_KEYWORD_IF) {
                    Block();

                    if (peak_next_token().type == TOKEN_KEYWORD_ELSE) {
                        error("identifier (within expression)",
                              peak_next_token().value,
                              peak_next_token().fileName,
                              peak_next_token().lineNumber);
                    }
                } else if (peak_next_token().type == TOKEN_KEYWORD_IF) {
                    goto IfLoop;
                }
            }
        } else {
            error("(",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        return;
    }

        // TODO: Check the for Stmt.
        // <ForStmt> := 'for' '(' <VarDecl> ';' <Expr> ';' <Expr> ';' ')' <Block>
    else if (peak_next_token().type == TOKEN_KEYWORD_FOR) {
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            Expr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            } else {
                consume();
            }

            Expr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            } else {
                consume();
            }

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            } else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Block();
        } else {
            error("(",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        return;
    }

        // <WhileStmt> := 'while' '(' <Expr> ')' <Block>
    else if (peak_next_token().type == TOKEN_KEYWORD_WHILE) {
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                error_no_quotes("identifier (within expression)",
                                peak_next_token().value,
                                peak_next_token().fileName,
                                peak_next_token().lineNumber);
            } else {
                Expr();
            }

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            } else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Block();
        } else {
            error("(",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        return;
    }

        // <DoWhileStmt> := 'do' <Block> 'while' '(' <Expr> ')' ';'
    else if (peak_next_token().type == TOKEN_KEYWORD_DO) {
        consume();

        Block();

        if (peak_next_token().type == TOKEN_KEYWORD_WHILE) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
                consume();

                Expr();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                    consume();
                } else {
                    error(")",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }
            } else {
                error("(",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            consume();
        }

        return;
    }

    if (peak_next_token().type == TOKEN_TYPE) {
        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            consume();

            rollback();
            rollback();

            VarDecl();
        }
    } else {
        Expr();

        consume();
    }
}

// StmtList := Stmt | Stmt StmtList
void parser::StmtList() {
    if (peak_next_token().type == TOKEN_EOF) {
        error_no_quotes("identifier (within expression)",
                        peak_next_token().value,
                        peak_next_token().fileName,
                        peak_next_token().lineNumber);
    }

    if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE) {
        return;
    }

    Stmt();
    StmtList();
}

// <FuncCallParamList> := e | <Expr> | Expr ',' <FuncDeclParamList>
void parser::FuncCallParamList(std::string funcName, std::vector<type_t> *types) {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            error(")",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        type_t type = Expr();

        types->push_back(type);

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                FuncCallParamList(std::move(funcName), types);
            } else {
                error("<Expr>",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }
}

parser::type_t parser::Expr() {
    type_t returnType = ComputeExpression(1);

    return returnType;
}

parser::type_t parser::ComputeExpression(int precedence) {
    type_t type = ComputeTerm();

    int nextPrecedenceLevel;

    while (true) {
        // TODO: This is a hack and we need a better way.
        if (peak_next_token().type == TOKEN_SYMBOL_PLUS_SIGN) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_PLUS_SIGN) {
                error("identifier (within expression)",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);

            }

            rollback();
        }

        if (!precedenceMap[peak_next_token().value] || precedenceMap[peak_next_token().value] < precedence) {
            break;
        } else if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN
                   || peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
            return type;
        }

        int _precedence = precedenceMap[peak_next_token().value];

        associativity_t associativity = associativityMap[peak_next_token().value];

        if (associativity == RIGHT_TO_LEFT) {
            nextPrecedenceLevel = _precedence;
        } else {
            nextPrecedenceLevel = _precedence + 1;
        }

        consume();

        ComputeExpression(nextPrecedenceLevel);
    }

    return type;
}

parser::type_t parser::ComputeTerm() {
    // Handle literals.
    switch (peak_next_token().type) {
        case TOKEN_LITERAL_NUMBER:
            consume();
            return type_t::INTEGER;
        case TOKEN_LITERAL_REAL:
            consume();
            return type_t::DOUBLE;
        case TOKEN_LITERAL_FLOAT:
            consume();
            return type_t::FLOAT;
        case TOKEN_LITERAL_CHAR:
            consume();
            return type_t::CHAR;
        case TOKEN_LITERAL_STRING:
            consume();
            return type_t::STRING;
        default:
            break;
    }

    // Handle decrement and increment operators.
    if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT || peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            std::string name = peak_next_token().value;

            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_EQUAL_SIGN
                || peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            // An Identifier, left parenthesis, a comma-separated list of zero or more expressions, and a right parenthesis.
            //
            // <Term> := <Ident> '(' <ExprList> ')';
            // <Term> := <Ident> '(' <Expr> ')';
            //
            // Returns: The return type of the function call.
            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
                consume();

                std::vector<type_t> types;

                FuncCallParamList(name, &types);

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                    consume();
                } else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                    error(")",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }

                if (!match(TOKEN_SYMBOL_SEMICOLON)) {
                    error(";",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                } else {
                    // Type checking function parameters.
                    std::vector<type_t> expectedTypes;
                    type_t expectedReturnType;

                    for (auto &function: functions) {
                        if (name == function.name) {
                            expectedReturnType = function.return_type;

                            for (auto &parameter: function.parameters) {
                                expectedTypes.push_back(parameter.type);
                            }

                            break;
                        }
                    }

                    // Check to see if the same number of parameters are passed.
                    // TODO: Change Error message.
                    if (expectedTypes.size() != types.size()) {
                        fprintf(stderr, "TYPE MISMATCH: Expected %lu arguments, got %lu\n", expectedTypes.size(),
                                types.size());
                        exit(-1);
                    }

                    return expectedReturnType;
                }
            }

            // An l-value with left and right brackets.
            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
                    error("identifier (within expression)",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }

                // TODO: Type check this.
                type_t type = Expr();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                    consume();
                } else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_BRACKET) {
                    error("]",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }

                if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
                    peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                    consume();

                    if (global) {
                        return global_variables[name];
                    } else {
                        return local_variables[name];
                    }
                }

                if (peak_next_token().type == TOKEN_SYMBOL_EQUAL_SIGN
                    || peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
                    consume();

                    type_t type = Expr();

                    // TODO: Fix the Type checking.
                    if (global) {
                        if (global_variables[name]) {
                            return global_variables[name];
                        } else {
                            global_variables[name] = type;
                            return type;
                        }
                    } else {
                        if (local_variables[name]) {
                            return local_variables[name];
                        } else {
                            local_variables[name] = type;
                            return type;
                        }
                    }
                }
            }

            // An l-value with increment or decrement.
            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
                peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                consume();

                if (global) {
                    return global_variables[name];
                } else {
                    return local_variables[name];
                }
            }

            if (
                    peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
                consume();

                type_t type = Expr();

                // TODO: Fix the Type checking.
                if (global) {
                    if (global_variables[name]) {
                        return global_variables[name];
                    } else {
                        global_variables[name] = type;
                        return type;
                    }
                } else {
                    if (local_variables[name]) {
                        if (local_variables[name] == type) {
                            return local_variables[name];
                        } else {
                            return global_variables[name];
                        }
                    } else {
                        local_variables[name] = type;
                        return type;
                    }
                }
            }

            for (auto &function: functions) {
                if (function.name == name) {
                    return function.return_type;
                }
            }

            if (global) {
                return global_variables[name];
            } else {
                return local_variables[name];
            }
        }
    }

    // Handle declarations of variables.
    if (peak_next_token().type == TOKEN_TYPE) {
        type_t type = Type();
        std::string name = Id();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
                error("identifier (within expression)",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            // TODO: Type check this.
            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                consume();
            } else {
                error("]",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }

        return type;
    }

    // Identifiers and l-values.
    if (peak_next_token().type == TOKEN_IDENTIFIER) {
        std::string name = peak_next_token().value;

        consume();

        // An Identifier, left parenthesis, a comma-separated list of zero or more expressions, and a right parenthesis.
        //
        // <Term> := <Ident> '(' <ExprList> ')';
        // <Term> := <Ident> '(' <Expr> ')';
        //
        // Returns: The return type of the function call.
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            std::vector<type_t> types;

            FuncCallParamList(name, &types);

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            } else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                error_no_quotes("',' or ')'",
                                peak_next_token().value,
                                peak_next_token().fileName,
                                peak_next_token().lineNumber);
            }

            // TODO: If we are calling a function with a return type, and the next token is a binop then we can continue.
            if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON
                || peak_next_token().type == TOKEN_SYMBOL_PLUS_SIGN
                || peak_next_token().type == TOKEN_SYMBOL_MINUS_SIGN
                || peak_next_token().type == TOKEN_SYMBOL_ASTERISK
                || peak_next_token().type == TOKEN_SYMBOL_FORWARD_SLASH
                || peak_next_token().type == TOKEN_SYMBOL_PERCENT_SIGN) {

                type_t return_type;

                bool function_found = false;

                for (auto &function: functions) {
                    if (function.name == name) {
                        if (types.size() == function.parameters.size()) {
                            function_found = true;
                            return_type = function.return_type;
                        }
                    }
                }

                // TODO: Related to question 3 above.
                if (!function_found) {
//                    error_no_quotes("No valid function found!",
//                          name,
//                          peak_next_token().fileName,
//                          peak_next_token().lineNumber);
                }

                return return_type;
            }
        }

        // An l-value with left and right brackets.
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
                error("identifier (within expression)",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            // TODO: Type check this.
            type_t type = Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                consume();
            } else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_BRACKET) {
                error("]",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
                peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_EQUAL_SIGN
                    || peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
                    || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
                    error(";",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }

                if (global) {
                    return global_variables[name];
                } else {
                    return local_variables[name];
                }
            }

            if (peak_next_token().type == TOKEN_SYMBOL_EQUAL_SIGN
                || peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
                || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
                consume();

                type_t type = Expr();

                // TODO: Fix the Type checking.
                if (global) {
                    if (global_variables[name]) {
                        return global_variables[name];
                    } else {
                        global_variables[name] = type;
                        return type;
                    }
                } else {
                    if (local_variables[name]) {
                        return local_variables[name];
                    } else {
                        local_variables[name] = type;
                        return type;
                    }
                }
            }
        }

        // An l-value with increment or decrement.
        if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
            peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
            consume();

            if (global) {
                return global_variables[name];
            } else {
                return local_variables[name];
            }
        }

        // Handle the case where we are followed by an assignment operator.
        if (peak_next_token().type == TOKEN_SYMBOL_EQUAL_SIGN
            || peak_next_token().type == TOKEN_SYMBOL_PLUS_ASSIGNMENT
            || peak_next_token().type == TOKEN_SYMBOL_MINUS_ASSIGNMENT
            || peak_next_token().type == TOKEN_SYMBOL_MULT_ASSIGNMENT
            || peak_next_token().type == TOKEN_SYMBOL_DIVIDE_ASSIGNMENT
            || peak_next_token().type == TOKEN_SYMBOL_MODULO_ASSIGNMENT) {
            consume();

            type_t type = Expr();

            if (global) {
                if (global_variables[name]) {
                    return global_variables[name];
                } else {
                    global_variables[name] = type;
                    return type;
                }
            } else {
                if (local_variables[name]) {
                    return local_variables[name];
                } else {
                    local_variables[name] = type;
                    return type;
                }
            }
        }

        // Handle declaring variables in global or local scope.
        if (global) {
            if (global_variables[name] == UNDEFINED) {
                for (auto &function: functions) {
                    if (function.name == name) {
                        return function.return_type;
                    }
                }
            }

            return global_variables[name];
        } else {
            if (local_variables[name] == UNDEFINED) {
                for (auto &function: functions) {
                    if (function.name == name) {
                        return function.return_type;
                    }
                }
            }

            return local_variables[name];
        }
    }

    // Unary operators.
    if (peak_next_token().type == TOKEN_SYMBOL_MINUS_SIGN
        || peak_next_token().type == TOKEN_SYMBOL_EXCLAMATION_MARK
        || peak_next_token().type == TOKEN_SYMBOL_TILDE) {
        consume();

        return Expr();
    }

    // TODO: Not sure this is correct.
    // Ternary operator.
    if (peak_next_token().type == TOKEN_SYMBOL_QUESTION_MARK) {
        consume();

        type_t type1 = Expr();

        if (peak_next_token().type == TOKEN_SYMBOL_COLON) {
            consume();

            type_t type2 = Expr();

            // TODO: Handle Type Checking for Ternary Operator.

            return type1;
        } else {
            error(":",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }
    }

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        consume();

        if (peak_next_token().type == TOKEN_TYPE) {
            Type();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                type_t type = Expr();

                return type;
            }
        } else {
            type_t type = Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
                    peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                    error(";",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }

                return type;
            } else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }

    // TODO: Do we need this or should the ComputeExpression() do this?
    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        consume();

        if (peak_next_token().type == TOKEN_TYPE) {
            Type();
        } else {
            type_t type = Expr();
        }

        if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
            error(")",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        } else {
            consume();

            type_t type = Expr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            } else {
                consume();

                return type;
            }
        }
    }

    return UNDEFINED;
}

// <Type> := int | double | char | void | string
parser::type_t parser::Type() {
    type_t type = type_t::UNDEFINED;

    if (peak_next_token().type == TOKEN_TYPE) {
        if (peak_next_token().value == "int") {
            type = type_t::INTEGER;
        } else if (peak_next_token().value == "double") {
            type = type_t::DOUBLE;
        } else if (peak_next_token().value == "float") {
            type = type_t::FLOAT;
        } else if (peak_next_token().value == "char") {
            type = type_t::CHAR;
        } else if (peak_next_token().value == "void") {
            type = type_t::VOID;
        } else if (peak_next_token().value == "string") {
            type = type_t::STRING;
        } else {
            error("<Type>", peak_next_token().value, peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        return type;
    } else {
        error("<Type>", peak_next_token().value, peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    return UNDEFINED;
}

// <Id> := [_a-zA-Z][_a-zA-Z0-9]*
std::string parser::Id() {
    if (!match(TOKEN_IDENTIFIER)) {
        error("<Identifier>", peak_next_token().value, peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    std::string id = peak_next_token().value;

    consume();

    return id;
}