/**
 * TODO:
 * QUESTIONS:
 * 1. How do I handle Block of code braces i.e. if we have a single statement we don't need to use braces for while loops, for loops, etc. (ite1.c, ite3.c, loop3.c, loop4.c loop6.c loop8.c)?
 * 2. I have an error were the int a; a = 2 : x; where the precedence function allows this (exprs5.c)?
 * 3. Do I need to install the standard library functions?
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "typechecker.h"

#include <utility>

// Program := Decl DecList | Decl
void typechecker::Program() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    Decl();
    DeclList();
}

// <Decl> := <VarDecl> | <FuncProto> | <FuncDef>
void typechecker::Decl() {
    if (peak_next_token().type == TOKEN_EOF) {
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
}

// <DeclList> := <Decl> <DeclList>
void typechecker::DeclList() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    Decl();
    DeclList();
}

// <VarDecl> := <Type> <VarIdList> ';'
void typechecker::VarDecl() {
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
void typechecker::VarDeclList(type_t type) {
    VarDeclId(type);

    if (match(TOKEN_SYMBOL_COMMA)) {
        consume();

        VarDeclList(type);
    }
}

// <VarDeclId> := <Id> | <Id> '[' <NumberLiteral>']'
void typechecker::VarDeclId(type_t type) {
    std::string name = peak_next_token().value;
    std::string id = Id();

    if (type == VOID) {
        auto s1 = string_format("Type checking error in file %s line %i\n",
                                peak_next_token().fileName.c_str(),
                                peak_next_token().lineNumber);
        auto s2 = string_format("\tVariable '%s' has type void\n", id.c_str());

        type_error_list.push_back(s1);
        type_error_list.push_back(s2);

        return;
    }

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
        switch (type) {
            case INTEGER:
                type = INTEGER_ARRAY;
                break;
            case DOUBLE:
                type = DOUBLE_ARRAY;
                break;
            case FLOAT:
                type = FLOAT_ARRAY;
                break;
            case CHAR:
                type = CHAR_ARRAY;
                break;
            case BOOL:
                type = BOOL_ARRAY;
                break;
            case STRING:
                type = STRING_ARRAY;
                break;
        }
    }

    if (global) {
        for (auto const &[key, val]: global_variables) {
            if (key == id) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);

                auto lineNumber = 0;

                // FIXME: HACK TO GET LINE NUMBER. We would need to store the line number of a declared variable in the symbol table.
                for (auto &token: tokens) {
                    if (token.type == TOKEN_IDENTIFIER && token.value == id) {
                        lineNumber = token.lineNumber;
                        break;
                    }
                }
                auto s2 = string_format("\tglobal %s %s duplicates global %s %s on line %i\n",
                                        type_to_string(type).c_str(),
                                        id.c_str(),
                                        type_to_string(val).c_str(),
                                        key.c_str(),
                                        lineNumber);

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);
            }
        }
    } else {
        for (auto const &[key, val]: local_variables) {
            if (key == id) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);

                auto lineNumber = 0;

                // FIXME: HACK TO GET LINE NUMBER. We would need to store the line number of a declared variable in the symbol table.
                for (auto &token: tokens) {
                    if (token.type == TOKEN_IDENTIFIER && token.value == id) {
                        lineNumber = token.lineNumber;
                        break;
                    }
                }

                auto s2 = string_format("\tlocal %s %s duplicates local %s %s on line %i\n",
                                        type_to_string(type).c_str(),
                                        id.c_str(),
                                        type_to_string(val).c_str(),
                                        key.c_str(),
                                        lineNumber);

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);
            }
        }
    }

    if (global) {
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (match(TOKEN_LITERAL_NUMBER)) {
                std::string value = peak_next_token().value;

                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                    consume();

                    global_variables[name] = type;

                    auto s1 = string_format("Line   %i: global %s %s[%s]\n",
                                            tokens[currentTokenIndex].lineNumber,
                                            type_to_string(array_base_type(type)).c_str(),
                                            name.c_str(),
                                            value.c_str(),
                                            tokens[currentTokenIndex].value.c_str());

                    type_decl_list.push_back(s1);
                } else {
                    error("']'", peak_next_token().value, peak_next_token().fileName, peak_next_token().lineNumber);
                }
            } else {
                error("<NumberLiteral>", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        } else {
            global_variables[tokens[currentTokenIndex].value] = type;

            auto s1 = string_format("Line   %i: global %s %s\n",
                                    tokens[currentTokenIndex].lineNumber,
                                    type_to_string(type).c_str(),
                                    tokens[currentTokenIndex].value.c_str());

            type_decl_list.push_back(s1);
        }
    } else {
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (match(TOKEN_LITERAL_NUMBER)) {
                std::string value = peak_next_token().value;

                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                    consume();

                    local_variables[name] = type;

                    auto s1 = string_format("    Line   %i: local %s %s[%s]\n",
                                            tokens[currentTokenIndex].lineNumber,
                                            type_to_string(array_base_type(type)).c_str(),
                                            name.c_str(),
                                            value.c_str()
                    );

                    type_decl_list.push_back(s1);
                } else {
                    error("']'", peak_next_token().value, peak_next_token().fileName, peak_next_token().lineNumber);
                }
            } else {
                error("<NumberLiteral>", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        } else {
            local_variables[name] = type;

            auto s1 = string_format("    Line   %i: local %s %s\n",
                                    tokens[currentTokenIndex].lineNumber,
                                    type_to_string(type).c_str(),
                                    tokens[currentTokenIndex].value.c_str());

            type_decl_list.push_back(s1);
        }
    }
}

void typechecker::FuncDecl() {
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

    std::vector<std::string> scene_list;

    for (auto &func_param: *func_params) {
        if (std::find(scene_list.begin(), scene_list.end(), func_param.name) != scene_list.end()) {
            auto s1 = string_format("Type checking error in file %s line %i\n",
                                    tokens[currentTokenIndex].fileName.c_str(),
                                    tokens[currentTokenIndex].lineNumber);
            auto s2 = string_format("\tparameter %s a duplicates parameter %s %s on line %i\n",
                                    type_to_string(func_param.type).c_str(),
                                    type_to_string(func_param.type).c_str(),
                                    func_param.name.c_str(),
                                    tokens[currentTokenIndex].lineNumber);

            type_error_list.push_back(s1);
            type_error_list.push_back(s2);

            return;
        } else {
            scene_list.push_back(func_param.name);
        }
    }
    for (auto p: function.parameters) {
        local_variables[p.name] = p.type;
    }

    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    } else {
        consume();
    }

    // Function Prototype
    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        for (auto &i: functions) {
            if (i.name == function.name) {
                int lineNumber;

                for (auto &token: tokens) {
                    if (token.type == TOKEN_IDENTIFIER && token.value == function.name) {
                        lineNumber = token.lineNumber;
                        break;
                    }
                }

                auto p1 = function.parameters;
                auto p2 = i.parameters;

                if (p1.size() == p2.size()) {
                    for (int k = 0; k < p1.size(); k++) {
                        if (p1[k].name != p2[k].name) {
                            break;
                        } else {
                            auto q1 = string_format("Type checking error in file %s line %i\n",
                                                    tokens[currentTokenIndex].fileName.c_str(),
                                                    tokens[currentTokenIndex].lineNumber);
                            auto q2 = string_format("\tfunction %s prototype conflicts with earlier one on line 3 %i\n",
                                                    function.name.c_str(),
                                                    lineNumber);

                            type_error_list.push_back(q1);
                            type_error_list.push_back(q2);

                            break;
                        }
                    }

                    break;
                }

                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tfunction %s prototype conflicts with earlier one on line %i\n",
                                        id.c_str(),
                                        lineNumber);

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);
            }
        }

        // Function isn't already defined.
        bool b = false;

        if (!b) {
            functions.push_back(function);
        } else {
            auto error1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);

            type_error_list.push_back(error1);
        }

        local_variables.clear();

        return;
    }
        // Function Definition
    else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACE) {
        // Function isn't already defined.
        for (auto &i: functions) {
            if (i.name == function.name) {
                int lineNumber;

                for (auto &token: tokens) {
                    if (token.type == TOKEN_IDENTIFIER && token.value == function.name) {
                        lineNumber = token.lineNumber;
                        break;
                    }
                }

                auto p1 = function.parameters;
                auto p2 = i.parameters;

                if (p1.size() == p2.size()) {
                    for (int k = 0; k < p1.size(); k++) {
                        if (p1[k].name != p2[k].name) {
                            break;
                        } else {
                            auto q1 = string_format("Type checking error in file %s line %i\n",
                                                    tokens[currentTokenIndex].fileName.c_str(),
                                                    tokens[currentTokenIndex].lineNumber);
                            auto q2 = string_format("\tfunction %s prototype conflicts with earlier one on line %i\n",
                                                    function.name.c_str(),
                                                    lineNumber);

                            type_error_list.push_back(q1);
                            type_error_list.push_back(q2);

                            break;
                        }
                    }

                    break;
                }

                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tfunction %s prototype conflicts with earlier one on line %i\n",
                                        id.c_str(),
                                        lineNumber);

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                if (i.return_type != function.return_type) {
                    lineNumber = 0;

                    for (auto &token: tokens) {
                        if (token.type == TOKEN_IDENTIFIER && token.value == function.name) {
                            lineNumber = token.lineNumber;
                            break;
                        }
                    }

                    auto error1 = string_format("Type checking error in file %s line %i\n",
                                                tokens[currentTokenIndex].fileName.c_str(),
                                                tokens[currentTokenIndex].lineNumber);
                    auto error2 = string_format("\tfunction %s prototype conflicts with earlier one on line %i\n",
                                                id.c_str(),
                                                lineNumber);

                    type_error_list.push_back(error1);
                    type_error_list.push_back(error2);

                    break;
                }
            }
        }

        auto decl2 = string_format("Line   %i: function %s %s\n",
                                   tokens[currentTokenIndex].lineNumber,
                                   type_to_string(type).c_str(),
                                   id.c_str());

        bool b = false;

        if (!b) {
            functions.push_back(function);
            type_decl_list.push_back(decl2);
        } else {
            auto error2 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);

            type_error_list.push_back(error2);
        }

        if (!function.parameters.empty()) {
            for (int i = 0; i <= function.parameters.size() - 1; i++) {
                std::string s2 = "";

                if (!is_array_type(function.parameters[i].type)) {
                    s2 = string_format("    Line   %i: parameter %s %s\n",
                                       tokens[currentTokenIndex].lineNumber,
                                       type_to_string(function.parameters[i].type).c_str(),
                                       function.parameters[i].name.c_str());
                } else {
                    s2 = string_format("    Line   %i: parameter %s\n",
                                       tokens[currentTokenIndex].lineNumber,
                                       type_to_string2(function.parameters[i].type,
                                                       function.parameters[i].name.c_str()).c_str());
                }


                type_decl_list.push_back(s2);
            }
        }

        is_in_function = true;
        function_name = id;
        function_return_type = type;
        found_return_statement = false;

        Block();

        is_in_function = false;
        function_name = "";
        function_return_type = type_t::UNDEFINED;
        found_return_statement = false;

        return;
    } else {
        error("{' or ';",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
}

// FuncDeclParamList := e | FuncDeclParam | FuncDeclParam ',' FuncDeclParamList
void typechecker::FuncDeclParamList(std::vector<function_parameter_t> *function_parameters) {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        type_t type = Type();
        std::string id = Id();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            switch (type) {
                case INTEGER:
                    type = INTEGER_ARRAY;
                    break;
                case CHAR:
                    type = CHAR_ARRAY;
                    break;
                case STRING:
                    type = CHAR_ARRAY;
                    break;
                case BOOL:
                    type = BOOL_ARRAY;
                    break;
                case DOUBLE:
                    type = DOUBLE_ARRAY;
                    break;
                case FLOAT:
                    type = FLOAT_ARRAY;
                    break;
            }

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                consume();
            } else {
                error("']'", peak_next_token().value, peak_next_token().fileName, peak_next_token().lineNumber);
            }
        }

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
void typechecker::Block() {
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

void typechecker::Stmt() {
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
        if (is_in_function && !function_name.empty() && function_return_type != type_t::UNDEFINED) {
            consume();

            if (function_return_type == type_t::VOID) {
                type_t type = Expr();

                if (type == VOID && peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    return;
                } else if (type == UNDEFINED && peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    return;
                } else {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            function_name.c_str(),
                                            peak_next_token().lineNumber);
                    auto s2 = string_format("\tReturn type mismatch: was %s, expected %s\n",
                                            type_to_string2(type, "").c_str(),
                                            type_to_string2(type_t::VOID, "").c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return;
                }

                if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                    error(";",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }
            } else {
                if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            function_name.c_str(),
                                            peak_next_token().lineNumber);
                    auto s2 = string_format("\tReturn type mismatch: was %s, expected %s\n",
                                            type_to_string2(type_t::VOID, "").c_str(),
                                            type_to_string2(function_return_type, "").c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return;
                }

                type_t type = Expr();

                if (type != function_return_type) {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            peak_next_token().fileName.c_str(),
                                            peak_next_token().lineNumber);
                    auto s2 = string_format("\tReturn type mismatch: was %s, expected %s\n",
                                            type_to_string(type).c_str(),
                                            type_to_string(function_return_type).c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return;
                }
            }

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            consume();

            found_return_statement = true;

            return;
        }
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
        type_t type = Expr();

        if (type != UNDEFINED) {
            if (peak_next_token().lineNumber <= 9) {
                auto s1 = string_format("    Line   %i: expression has type %s\n",
                                        tokens[currentTokenIndex].lineNumber,
                                        type_to_string(type).c_str());

                type_decl_list.push_back(s1);
            } else {
                auto s1 = string_format("    Line  %i: expression has type %s\n", tokens[currentTokenIndex].lineNumber,
                                        type_to_string(type).c_str());

                type_decl_list.push_back(s1);
            }
        }

        consume();
    }
}

// StmtList := Stmt | Stmt StmtList
void typechecker::StmtList() {
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
void typechecker::FuncCallParamList(std::string funcName, std::vector<type_t> *types) {
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

typechecker::type_t typechecker::Expr() {
    type_t returnType = ComputeExpression(1);

    return returnType;
}

typechecker::type_t typechecker::ComputeExpression(int precedence) {
    type_t type1 = ComputeTerm();

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
            return type1;
        }

        std::string op = peak_next_token().value;

        int _precedence = precedenceMap[peak_next_token().value];

        associativity_t associativity = associativityMap[peak_next_token().value];

        if (associativity == RIGHT_TO_LEFT) {
            nextPrecedenceLevel = _precedence;
        } else {
            nextPrecedenceLevel = _precedence + 1;
        }

        consume();

        token token = tokens[currentTokenIndex];

        type_t type2 = ComputeExpression(nextPrecedenceLevel);

        if (type1 == VOID && type2 == VOID) {
            auto s1 = string_format("\"Type checking error in file %s line %i\n",
                                    token.fileName.c_str(),
                                    token.lineNumber);
            auto s2 = string_format("\tInvalid operand types %s == %s\n",
                                    type_to_string(type1).c_str(),
                                    type_to_string(type2).c_str());

            type_error_list.push_back(s1);
            type_error_list.push_back(s2);
        }

        if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "&&" ||
            op == "||" || op == "!") {
            if (is_array_type(type1) || is_array_type(type2)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        token.fileName.c_str(),
                                        token.lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %s %s\n",
                                        type_to_string(type1).c_str(),
                                        op.c_str(),
                                        type_to_string(type2).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return {};
            }

            return CHAR;
        } else {
            if (type1 != type2) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %c %s\n", type_to_string(type1).c_str(),
                                        token.value[0],
                                        type_to_string(type2).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);
            } else if (is_array_type(type1) && is_array_type(type2)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %c %s\n", type_to_string(type1).c_str(),
                                        token.value[0],
                                        type_to_string(type2).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);
            }
        }
    }

    return type1;
}

typechecker::type_t typechecker::ComputeTerm() {
    // Handle literals.
    switch (peak_next_token().type) {
        case TOKEN_LITERAL_NUMBER:
            consume();
            return type_t::INTEGER;
        case TOKEN_LITERAL_REAL:
            consume();
            return type_t::FLOAT;
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
    if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT
        || peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
        std::string operator_str;

        if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT) {
            operator_str = "++";
        } else if (peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
            operator_str = "--";
        }

        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            std::string name = peak_next_token().value;

            consume();

            type_t type;

            if (global_variables[name]) {
                type = global_variables[name];
            } else if (local_variables[name]) {
                type = local_variables[name];
            }

            if (is_array_type(type)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tOperator %s needs numeric operand; type was %s\n",
                                        operator_str.c_str(),
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return {};
            }

            return type;

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

        if (local_variables[name]) {
            int lineNumber;

            // FIXME: This is a hack.
            for (auto &token: tokens) {
                if (token.type == TOKEN_IDENTIFIER && token.value == name) {
                    lineNumber = token.lineNumber;

                    break;
                }
            }

            type_t t1 = local_variables[name];
            type_t t2 = type;

            auto s1 = string_format("Type checking error in file %s line %i\n",
                                    peak_next_token().fileName.c_str(),
                                    peak_next_token().lineNumber);
            auto s2 = string_format("\tlocal %s %s duplicates local %s %s on line %i\n",
                                    type_to_string(t1).c_str(),
                                    name.c_str(),
                                    type_to_string(t2).c_str(),
                                    name.c_str(),
                                    lineNumber);

            type_error_list.push_back(s1);
            type_error_list.push_back(s2);

            return type;
        }

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

        // Function Call
        // <Term> := <Ident> '(' <ExprList> ')';
        // <Term> := <Ident> '(' <Expr> ')';
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            std::vector<type_t> types;

            FuncCallParamList(name, &types);

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                for (auto &function: functions) {
                    if (function.name == name) {
                        if (function.parameters.size() != types.size()) {
                            auto s1 = string_format("Type checking error in file %s line %i\n",
                                                    tokens[currentTokenIndex].fileName.c_str(),
                                                    tokens[currentTokenIndex].lineNumber);

                            type_error_list.push_back(s1);

                            std::string functionName = name + "(";

                            for (int i = 0; i < function.parameters.size(); i++) {
                                functionName += type_to_string(function.parameters[i].type);

                                if (i != function.parameters.size() - 1) {
                                    functionName += ", ";
                                }
                            }

                            functionName += ")";

                            auto s2 = string_format("\tParameter mismatch in call to %s \n", functionName.c_str());
                            auto s3 = string_format("\tExpected %i, received %i parameters\n",
                                                    function.parameters.size(), types.size());

                            type_error_list.push_back(s2);
                            type_error_list.push_back(s3);

                            return {};
                        } else if (function.parameters.size() == types.size()) {
                            std::string functionName = name + "(";

                            for (int i = 0; i < function.parameters.size(); i++) {
                                functionName += type_to_string(function.parameters[i].type);

                                if (i != function.parameters.size() - 1) {
                                    functionName += ",";
                                }
                            }

                            functionName += ")";

                            for (int i = 0; i < types.size(); i++) {
                                if (function.parameters[i].type != types[i]) {
                                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                                            tokens[currentTokenIndex].fileName.c_str(),
                                                            tokens[currentTokenIndex].lineNumber);
                                    auto s2 = string_format("\tParameter mismatch in call to %s\n",
                                                            functionName.c_str());
                                    auto s3 = string_format("\tPassed parameter #%i has type %s\n", i + 1,
                                                            type_to_string(types[i]).c_str());

                                    type_error_list.push_back(s1);
                                    type_error_list.push_back(s2);
                                    type_error_list.push_back(s3);

                                    return {};
                                }
                            }
                        }

                        return function.return_type;
                    }
                }

                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tUnknown function '%s'", name.c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return {};
            } else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                error_no_quotes("',' or ')'",
                                peak_next_token().value,
                                peak_next_token().fileName,
                                peak_next_token().lineNumber);
            }

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

            type_t type = Expr();

            if (type != type_t::INTEGER) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\t\tArray index should be integer, was %s\n",
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return {};
            }

            if (peak_next_token().type == TOKEN_LITERAL_NUMBER) {
                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                    consume();
                }
            }

            for (auto &variable: global_variables) {
                if (variable.first == name && variable.second != type_t::UNDEFINED) {
                    if (!is_array_type(variable.second)) {
                        auto s1 = string_format("Type checking error in file %s line %i\n",
                                                tokens[currentTokenIndex].fileName.c_str(),
                                                tokens[currentTokenIndex].lineNumber);
                        auto s2 = string_format("\t\tVariable '%s' is not an array\n", name.c_str());

                        type_error_list.push_back(s1);
                        type_error_list.push_back(s2);

                        return {};
                    }

                    return array_base_type(variable.second);
                }
            }

            for (auto &variable: local_variables) {
                if (variable.first == name && variable.second != type_t::UNDEFINED) {
                    if (!is_array_type(variable.second)) {
                        auto s1 = string_format("Type checking error in file %s line %i\n",
                                                tokens[currentTokenIndex].fileName.c_str(),
                                                tokens[currentTokenIndex].lineNumber);
                        auto s2 = string_format("\t\tVariable '%s' is not an array\n", name.c_str());

                        type_error_list.push_back(s1);
                        type_error_list.push_back(s2);

                        return {};
                    }

                    return array_base_type(variable.second);
                }
            }
        }

        // An l-value with increment or decrement.
        if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
            peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
            std::string operator_str;

            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT) {
                operator_str = "++";
            } else if (peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                operator_str = "--";
            }

            consume();

            type_t type;

            if (global_variables[name]) {
                type = global_variables[name];
            } else if (local_variables[name]) {
                type = local_variables[name];
            }

            if (is_array_type(type)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tOperator %s needs numeric operand; type was %s\n",
                                        operator_str.c_str(),
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return {};
            }

            return type;

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

            type_t type1;

            if (global) {
                type1 = global_variables[name];
            } else {
                type1 = local_variables[name];
            }

            token token = tokens[currentTokenIndex];

            type_t type = Expr();

            if (is_array_type(type1) && is_array_type(type)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %c %s\n", type_to_string(type1).c_str(),
                                        token.value[0],
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return UNDEFINED;
            } else if (is_array_type(type1)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %c %s\n", type_to_string(type1).c_str(),
                                        token.value[0],
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return UNDEFINED;
            } else if (is_array_type(type)) {
                auto s1 = string_format("Type checking error in file %s line %i\n",
                                        tokens[currentTokenIndex].fileName.c_str(),
                                        tokens[currentTokenIndex].lineNumber);
                auto s2 = string_format("\tInvalid operand types %s %c %s\n", type_to_string(type1).c_str(),
                                        token.value[0],
                                        type_to_string(type).c_str());

                type_error_list.push_back(s1);
                type_error_list.push_back(s2);

                return UNDEFINED;
            }

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

        if (local_variables[name]) {
            return local_variables[name];
        } else if (global_variables[name]) {
            return global_variables[name];
        } else if (!local_variables[name]) {
            auto s1 = string_format("Type checking error in file %s line %i\n",
                                    tokens[currentTokenIndex].fileName.c_str(),
                                    tokens[currentTokenIndex].lineNumber);
            auto s2 = string_format("\tUnknown variable '%s'\n", name.c_str());

            type_error_list.push_back(s1);
            type_error_list.push_back(s2);

            return {};
        }
    }

    // Unary operators.
    if (peak_next_token().type == TOKEN_SYMBOL_MINUS_SIGN
        || peak_next_token().type == TOKEN_SYMBOL_EXCLAMATION_MARK
        || peak_next_token().type == TOKEN_SYMBOL_TILDE) {
        token token = peak_next_token();

        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            for (auto &function: functions) {
                if (function.name == peak_next_token().value) {
                    if (function.return_type == VOID) {
                        auto s1 = string_format("Type checking error in file %s line %i\n",
                                                tokens[currentTokenIndex].fileName.c_str(),
                                                tokens[currentTokenIndex].lineNumber);
                        auto s2 = string_format("\toperand for - should be numeric type, was %s\n",
                                                type_to_string(function.return_type).c_str());

                        type_error_list.push_back(s1);
                        type_error_list.push_back(s2);

                        return {};
                    }
                }
            }
        }

        type_t t = Expr();

        if (is_array_type(t)) {
            auto s1 = string_format("Type checking error in file %s line %i\n",
                                    tokens[currentTokenIndex].fileName.c_str(),
                                    tokens[currentTokenIndex].lineNumber);
            auto s2 = string_format("\toperand for %s should be integer type, was %s\n",
                                    token.value.c_str(),
                                    type_to_string(t).c_str());

            type_error_list.push_back(s1);
            type_error_list.push_back(s2);

            return {};
        }

        if (token.value == "!") {
            return CHAR;
        }

        return t;
    }

    // Ternary operator.
    if (peak_next_token().type == TOKEN_SYMBOL_QUESTION_MARK) {
        printf("Ternary operator\n");

        if (tokens[currentTokenIndex - 1].type == TOKEN_SYMBOL_COLON) {
            printf("Ternary operator not supported.\n");
        }

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
            type_t cast_type = Type();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                type_t type = Expr();

                if (is_array_type(type)) {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            tokens[currentTokenIndex].fileName.c_str(),
                                            tokens[currentTokenIndex].lineNumber);
                    auto s2 = string_format("\tCan't cast from %s to %s\n", type_to_string(type).c_str(),
                                            type_to_string(cast_type).c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return {};
                } else if (type == VOID) {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            tokens[currentTokenIndex].fileName.c_str(),
                                            tokens[currentTokenIndex].lineNumber);
                    auto s2 = string_format("\tCan't cast from %s to %s\n", type_to_string(type).c_str(),
                                            type_to_string(cast_type).c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return {};
                }

                return cast_type;
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

    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        consume();

        type_t cast_type;

        if (peak_next_token().type == TOKEN_TYPE) {
            cast_type = Type();
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

                return cast_type;
            }
        }
    }

    return UNDEFINED;
}

// <Type> := int | double | char | void | string
typechecker::type_t typechecker::Type() {
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
std::string typechecker::Id() {
    if (!match(TOKEN_IDENTIFIER)) {
        error("<Identifier>", peak_next_token().value, peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    std::string id = peak_next_token().value;

    consume();

    return id;
}