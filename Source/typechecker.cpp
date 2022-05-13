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
#include "AST.h"

#include <utility>

// Program := Decl DecList | Decl
std::shared_ptr<ASTNode> typechecker::Program() {
    if (peak_next_token().type == TOKEN_EOF)
        return nullptr;

    std::shared_ptr<ASTNode> program = std::make_shared<ASTNode>(ASTNodeType::Program);

    std::shared_ptr<ASTNode> decl = Decl();
    std::shared_ptr<ASTNode> declList = DeclList();

    if (decl != nullptr) {
        program->leftChild = decl;
        program->rightChild = declList;
    }

    return program;
}

// <Decl> := <VarDecl> | <FuncProto> | <FuncDef>
std::shared_ptr<ASTNode> typechecker::Decl() {
    if (peak_next_token().type == TOKEN_EOF) {
        return nullptr;
    }

    // <Decl> := <VarDecl> | <FuncDecl>
    if (peak_next_token().type == TOKEN_TYPE) {
        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
                rollback();
                rollback();

                return FuncDecl();
            } else if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON || TOKEN_SYMBOL_COMMA) {
                rollback();
                rollback();

                return VarDecl();
            }
        }
    } else {
        error("<VarDecl> or <FuncProto>",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    return nullptr;
}

// <DeclList> := <Decl> <DeclList>
std::shared_ptr<ASTNode> typechecker::DeclList() {
    if (peak_next_token().type == TOKEN_EOF)
        return nullptr;

    std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>(
            ASTNodeType::DeclList,
            nullptr,
            Decl(),
            DeclList());

    return node;
}

// <VarDecl> := <Type> <VarIdList> ';'
std::shared_ptr<ASTNode> typechecker::VarDecl() {
    type_t declType = Type();

    std::shared_ptr<ASTNode> varDeclNode = std::make_shared<ASTNode>(ASTNodeType::VarDecl);

    varDeclNode->setNodeValueType(declType);

    std::vector<std::string> ids = VarDeclList(declType);

    for (auto &id: ids) {
        varDeclNode->nodeValues.push_back(id);
    }

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return varDeclNode;
    } else {
        error_no_quotes("',' or ';'",
                        peak_next_token().value,
                        peak_next_token().fileName,
                        peak_next_token().lineNumber);
    }

    return varDeclNode;
}

// <VarDeclIdList> := <VarDeclId> | <VarDeclId> ',' <VarDeclIdList>
std::vector<std::string> typechecker::VarDeclList(type_t type) {
    std::vector<std::string> result;

    std::string id = VarDeclId(type);

    result.push_back(id);

    if (match(TOKEN_SYMBOL_COMMA)) {
        consume();

        std::vector<std::string> ids = VarDeclList(type);

        for (auto &i: ids) {
            result.push_back(i);
        }
    }

    return result;
}

// <VarDeclId> := <Id> | <Id> '[' <NumberLiteral>']'
std::string typechecker::VarDeclId(type_t type) {
    std::string name = peak_next_token().value;
    std::string id = Id();

    if (type == VOID) {
        auto s1 = string_format("Type checking error in file %s line %i\n",
                                peak_next_token().fileName.c_str(),
                                peak_next_token().lineNumber);
        auto s2 = string_format("\tVariable '%s' has type void\n", id.c_str());

        type_error_list.push_back(s1);
        type_error_list.push_back(s2);

        return id;
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
            default:
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

    return id;
}

// <FuncDecl> ::= <Type> <Identifier> <FuncParams> <FuncBody> | <Type> <Identifier> <FuncParams>
std::shared_ptr<ASTNode> typechecker::FuncDecl() {
    type_t type = Type();
    std::string id = Id();

    std::shared_ptr<ASTNode> func_proto_node = std::make_shared<ASTNode>(ASTNodeType::FuncProto);
    std::shared_ptr<ASTNode> func_def_node = std::make_shared<ASTNode>(ASTNodeType::FuncDef);

    std::shared_ptr<ASTNode> func_body_node;

    func_proto_node->nodeValueType = type;
    func_proto_node->nodeValues.push_back(id);

    func_def_node->nodeValueType = type;
    func_def_node->nodeValues.push_back(id);

    if (peak_next_token().type != TOKEN_SYMBOL_LEFT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    } else {
        consume();
    }


    // Handle Function Parameters
    auto *func_params = new std::vector<function_parameter_t>();

    FuncDeclParamList(func_params);

    std::map<std::string, type_t> param_map;

    for (auto &param: *func_params) {
        param_map[param.name] = param.type;
    }

    func_proto_node->nodeMap = param_map;
    func_def_node->nodeMap = param_map;

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

            return nullptr;
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

        functions.push_back(function);

        local_variables.clear();

        return func_proto_node;
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

        functions.push_back(function);
        type_decl_list.push_back(decl2);

        if (!function.parameters.empty()) {
            for (int i = 0; i <= function.parameters.size() - 1; i++) {
                std::string s2;

                if (!is_array_type(function.parameters[i].type)) {
                    s2 = string_format("    Line   %i: parameter %s %s\n",
                                       tokens[currentTokenIndex].lineNumber,
                                       type_to_string(function.parameters[i].type).c_str(),
                                       function.parameters[i].name.c_str());
                } else {
                    s2 = string_format("    Line   %i: parameter %s\n",
                                       tokens[currentTokenIndex].lineNumber,
                                       type_to_string2(function.parameters[i].type,
                                                       function.parameters[i].name).c_str());
                }

                type_decl_list.push_back(s2);
            }
        }

        is_in_function = true;
        function_name = id;
        function_return_type = type;
        found_return_statement = false;

        func_body_node = Block();
        func_def_node->setRightChild(std::move(func_body_node));

        is_in_function = false;
        function_name = "";
        function_return_type = type_t::UNDEFINED;
        found_return_statement = false;

        return func_def_node;
    } else {
        error("{' or ';",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    return nullptr;
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

            // FIXME: Fix this.
            // Expr();

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
std::shared_ptr<ASTNode> typechecker::Block() {
    std::shared_ptr<ASTNode> block_node;

    if (match(TOKEN_SYMBOL_LEFT_BRACE)) {
        consume();

        global = false;

        block_node = std::make_shared<ASTNode>(ASTNode(ASTNodeType::Block));

        std::shared_ptr<ASTNode> stmt_node = Stmt();
        std::shared_ptr<ASTNode> stmt_list_node = StmtList();

        block_node->setLeftChild(std::move(stmt_node));
        block_node->setRightChild(std::move(stmt_list_node));

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

    return block_node;
}

// TODO: Finish Loops
std::shared_ptr<ASTNode> typechecker::Stmt() {
    // <Stmt> := ';'
    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return std::make_shared<ASTNode>(ASTNodeType::EmptyStmt);
    }

    // <Stmt> := 'break' ';' | 'continue' ';'
    if (peak_next_token().type == TOKEN_KEYWORD_BREAK || peak_next_token().type == TOKEN_KEYWORD_CONTINUE) {
        token t = peak_next_token();

        consume();

        if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
            error(";",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        if (t.type == TOKEN_KEYWORD_BREAK) {
            return std::make_shared<ASTNode>(ASTNodeType::BreakStmt);
        } else if (t.type == TOKEN_KEYWORD_CONTINUE) {
            return std::make_shared<ASTNode>(ASTNodeType::ContinueStmt);
        } else {
            return nullptr;
        }

        return nullptr;
    }

    // <Stmt> := 'return' Expr ';' | 'return' ';'
    if (peak_next_token().type == TOKEN_KEYWORD_RETURN) {
        auto return_stmt = std::make_shared<ASTNode>(ASTNodeType::ReturnStmt);

        if (is_in_function && !function_name.empty() && function_return_type != type_t::UNDEFINED) {
            consume();

            if (function_return_type == type_t::VOID) {
                if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    consume();

                    return return_stmt;
                }

                std::shared_ptr<ASTNode> node = Expr();
                type_t type = node->getNodeValueType();

                return_stmt->setRightChild(std::move(node));

                if (type == VOID && peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    return std::make_shared<ASTNode>(ASTNodeType::ReturnStmt);
                } else if (type == UNDEFINED && peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
                    return std::make_shared<ASTNode>(ASTNodeType::ReturnStmt);
                } else {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            function_name.c_str(),
                                            peak_next_token().lineNumber);
                    auto s2 = string_format("\tReturn type mismatch: was %s, expected %s\n",
                                            type_to_string2(type, "").c_str(),
                                            type_to_string2(type_t::VOID, "").c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return nullptr;
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

                    return nullptr;
                }

                std::shared_ptr<ASTNode> node = Expr();
                type_t type = node->getNodeValueType();

                std::shared_ptr<ASTNode> expr = std::make_shared<ASTNode>(ASTNodeType::ExprStmt);

                expr->setRightChild(std::move(node));
                return_stmt->setRightChild(std::move(expr));

                if (type != function_return_type) {
                    auto s1 = string_format("Type checking error in file %s line %i\n",
                                            peak_next_token().fileName.c_str(),
                                            peak_next_token().lineNumber);
                    auto s2 = string_format("\tReturn type mismatch: was %s, expected %s\n",
                                            type_to_string(type).c_str(),
                                            type_to_string(function_return_type).c_str());

                    type_error_list.push_back(s1);
                    type_error_list.push_back(s2);

                    return nullptr;
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

            return return_stmt;
        }
    }

    // TODO: Loops and shit
    /*
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

    */

    // <Expr> := <AssignExpr> | <Expr>
    if (peak_next_token().type == TOKEN_TYPE) {
        consume();

        if (peak_next_token().type == TOKEN_IDENTIFIER) {
            consume();

            rollback();
            rollback();

            return VarDecl();
        }
    } else {
        if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE) {
            return std::make_shared<ASTNode>(ASTNodeType::EmptyStmt);
        }

        std::shared_ptr<ASTNode> expr = Expr();

        if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
            consume();
        }

        return expr;
    }

    return nullptr;
}

// StmtList := Stmt | Stmt StmtList
std::shared_ptr<ASTNode> typechecker::StmtList() {
    std::shared_ptr<ASTNode> block = std::make_shared<ASTNode>(ASTNodeType::StmtList);

    if (peak_next_token().type == TOKEN_EOF) {
        error_no_quotes("identifier (within expression)",
                        peak_next_token().value,
                        peak_next_token().fileName,
                        peak_next_token().lineNumber);
    }

    if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE) {
        return nullptr;
    }

    std::shared_ptr<ASTNode> stmt = Stmt();
    std::shared_ptr<ASTNode> stmtList = StmtList();

    stmtList->setNodeType(ASTNodeType::StmtList);

    block->setLeftChild(std::move(stmt));
    block->setRightChild(std::move(stmtList));

    return block;
}

//// <FuncCallParamList> := e | <Expr> | Expr ',' <FuncDeclParamList>
//void typechecker::FuncCallParamList(std::string funcName, std::vector<type_t> *types) {
//    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
//        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
//            error(")",
//                  peak_next_token().value,
//                  peak_next_token().fileName,
//                  peak_next_token().lineNumber);
//        }
//
//        type_t type = Expr();
//
//        types->push_back(type);
//
//        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
//            consume();
//
//            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
//                FuncCallParamList(std::move(funcName), types);
//            } else {
//                error("<Expr>",
//                      peak_next_token().value,
//                      peak_next_token().fileName,
//                      peak_next_token().lineNumber);
//            }
//        }
//    }
//}

std::shared_ptr<ASTNode> typechecker::Expr() {
    std::shared_ptr<ASTNode> expr = ComputeExpression(1);

    return expr;
}

/*
 *        1 + 2 + 3
 *
 *
 *            +
 *           / \
 *          /   \
 *         1     +
 *              / \
 *             /   \
 *            2     3
 */

std::shared_ptr<ASTNode> typechecker::ComputeExpression(int precedence) {
    std::shared_ptr<ASTNode> expr = ComputeTerm();

    std::string op;

    while (true) {
        if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
            return expr;
        }

        if ((precedenceMap.find(peak_next_token().value) != precedenceMap.end()) &&
            (precedenceMap[peak_next_token().value] < precedence))
            return expr;

        op = peak_next_token().value;

        std::shared_ptr<ASTNode> b;

        if (associativityMap[peak_next_token().value] == RIGHT_TO_LEFT) {
            consume();
            b = ComputeExpression(precedenceMap[peak_next_token().value]);
        } else {
            consume();
            b = ComputeExpression(precedenceMap[peak_next_token().value] + 1);
        }

        if (expr != nullptr) {
            std::shared_ptr<ASTNode> newNode = std::make_shared<ASTNode>(ASTNodeType::ExprOp);

            expr->setNodeType(ASTNodeType::ExprOp);
            expr->getNodeValues().push_back(op);
            expr->setLeftChild(expr);
            expr->setRightChild(b);
        }
    }

    return expr;
}

std::shared_ptr<ASTNode> typechecker::ComputeTerm() {
    // Handle Literals
    if (peak_next_token().type == TOKEN_LITERAL_NUMBER) {
        auto node_int = std::make_shared<ASTNode>(ASTNodeType::ExprLiteral);

        node_int->setNodeValueType(INTEGER);
        node_int->nodeValues.push_back(peak_next_token().value);

        consume();

        return node_int;
    } else if (peak_next_token().type == TOKEN_LITERAL_REAL) {
        auto node_real = std::make_shared<ASTNode>(ASTNodeType::ExprLiteral);

        node_real->setNodeValueType(DOUBLE);
        node_real->getNodeValues().push_back(peak_next_token().value);

        consume();

        return node_real;
    } else if (peak_next_token().type == TOKEN_LITERAL_FLOAT) {
        auto node_float = std::make_shared<ASTNode>(ASTNodeType::ExprLiteral);

        node_float->setNodeValueType(FLOAT);
        node_float->getNodeValues().push_back(peak_next_token().value);

        consume();

        return node_float;
    } else if (peak_next_token().type == TOKEN_LITERAL_CHAR) {
        auto node_char = std::make_shared<ASTNode>(ASTNodeType::ExprLiteral);

        node_char->setNodeValueType(CHAR);
        node_char->getNodeValues().push_back(peak_next_token().value);

        consume();

        return node_char;
    } else if (peak_next_token().type == TOKEN_LITERAL_STRING) {
        auto node_string = std::make_shared<ASTNode>(ASTNodeType::ExprLiteral);

        node_string->setNodeValueType(STRING);
        node_string->getNodeValues().push_back(peak_next_token().value);

        consume();

        return node_string;
    }

    return nullptr;
}

// <Type> := int | double | char | void | string
type_t typechecker::Type() {
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