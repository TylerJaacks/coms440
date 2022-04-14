#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "parser.h"

#include <utility>

void parser::error(const std::string& expectedToken, const std::string& tokenValue, const std::string& fileName, int lineNumber) {
    std::fprintf(stderr, R"(ERROR: Expected '%s' but got '%s' instead. FILE: %s LINE: %d)", expectedToken.c_str(),
                 tokenValue.c_str(), fileName.c_str(), lineNumber);
    exit(EXIT_FAILURE);
}

void parser::unexpected_token_error(const std::string& tokenValue, const std::string& fileName, int lineNumber) {
    std::fprintf(stderr, R"(ERROR: Unexpected Token '%s' instead. FILE: %s LINE: %d)",
                 tokenValue.c_str(), fileName.c_str(), lineNumber);
    exit(EXIT_FAILURE);
}

token parser::peak_next_token() {
    return tokens[currentTokenIndex + 1];
}

token parser::consume() {
    token token;

    if (currentTokenIndex != -1) {
        token = tokens[currentTokenIndex];
    }

    currentTokenIndex += 1;

    return token;
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

// Program := Decl DecList | Decl
void parser::Program() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    Decl();
    DeclList();
}

// <Decl> := <VarDecl> | <FuncProto> | <FuncDef>
void parser::Decl() {
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
            }
            else if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON || TOKEN_SYMBOL_COMMA) {
                rollback();
                rollback();

                VarDecl();

                return;
            }
        }
    }
    else {
        error("<VarDecl> or <FuncProto>",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
}

// <DeclList> := <Decl> <DeclList>
void parser::DeclList() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    Decl();
    DeclList();
}

// <VarDecl> := <Type> <VarIdList> ';'
void parser::VarDecl() {
    type_t declType = Type();

    VarDeclList(declType);

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return;
    }
    else {
        error(";",
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
    }
    else {
        consume();
    }

    FuncDeclParamList(func_params);

    function_t function = { .name = id, .return_type = type, .parameters = *func_params };

    functions.push_back(function);

    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
    else {
        consume();
    }

    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
        consume();

        return;
    }
    else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACE) {
        Block();

        return;
    }
    else {
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

        function_parameter_t param = { .type = type, .name = id };

        function_parameters->push_back(param);

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                FuncDeclParamList(function_parameters);
            }
            else {
                error("<Type>",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }
    else {
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
        } 
        else {
            error("}",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        global = true;

        local_variables.clear();
    }
    else {
        error("{",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
}

void parser::Stmt() {
    // <Stmt> := ';'
    if (peak_next_token().type == TOKEN_SYMBOL_SEMICOLON) {
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
        Expr();

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
            }
            else {
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
                        unexpected_token_error(peak_next_token().value,
                                               peak_next_token().fileName,
                                               peak_next_token().lineNumber);
                    }
                }
                else if (peak_next_token().type == TOKEN_KEYWORD_IF) {
                    goto IfLoop;
                }
            }
        }
        else {
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

            VarDecl();
            Expr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
            else {
                consume();
            }

            Expr();

            if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
                error(";",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
            else {
                consume();
            }

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            }
            else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Block();
        }
        else {
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

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            }
            else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            Block();
        }
        else {
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
                }
                else {
                    error(")",
                          peak_next_token().value,
                          peak_next_token().fileName,
                          peak_next_token().lineNumber);
                }
            }
            else {
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

    else {
        Expr();

        if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
            error(";",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }

        consume();

        return;
    }
}

// StmtList := Stmt | Stmt StmtList
void parser::StmtList() {
    if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE) {
        return;
    }

    Stmt();
    StmtList();
}

// <FuncCallParamList> := e | <Expr> | Expr ',' <FuncDeclParamList>
void parser::FuncCallParamList(std::string funcName, std::vector<type_t> *types) {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        type_t type = Expr();

        types->push_back(type);

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                FuncCallParamList(std::move(funcName), types);
            }
            else {
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
        if (!precedenceMap[peak_next_token().value] || precedenceMap[peak_next_token().value] < precedence) {
            break;
        }
        else if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN 
                || peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
            return type;
        }

        int _precedence = precedenceMap[peak_next_token().value];

        associativity_t associativity = associativityMap[peak_next_token().value];

        if (associativity == RIGHT_TO_LEFT) {
            nextPrecedenceLevel  = _precedence;
        }
        else {
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
        case TOKEN_LITERAL_CHAR:
            consume();
            return type_t::CHAR;
        case TOKEN_LITERAL_STRING:
            consume();
            return type_t::STRING;
        default:
            break;
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
            }
            else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
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
            }
            else {
                // Type checking function parameters.
                std::vector<type_t> expectedTypes;
                type_t expectedReturnType;

                for (auto & function : functions) {
                    if (name == function.name) {
                        expectedReturnType = function.return_type;

                        for (auto & parameter : function.parameters) {
                            expectedTypes.push_back(parameter.type);
                        }

                        break;
                    }
                }

                if (expectedTypes.size() != types.size()) {
                    fprintf(stderr, "TYPE MISMATCH: Expected %lu arguments, got %lu\n", expectedTypes.size(), types.size());
                    exit(-1);
                }
                else {
                    for (int i = 0; i < expectedTypes.size(); i++) {
                        if (expectedTypes[i] != types[i]) {
                            fprintf(stderr, "TYPE MISMATCH: Expected %s, got %s\n", "<Type1>", "<Type2>");
                            exit(-1);
                        }
                    }
                }

                return expectedReturnType;
            }
        }

        // An l-value with left and right brackets.
        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            type_t type = Expr();

            // Make sure that the type of the expression is an integer.
            if (type != INTEGER) {
                unexpected_token_error("<Integer>",
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
                consume();
            }
            else if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_BRACKET) {
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
            else {
                if (global) {
                    return global_variables[name];
                } else {
                    return local_variables[name];
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
                    if (global_variables[name] == type) {
                        return global_variables[name];
                    } else {
                        fprintf(stderr, "TYPE MISMATCH: Expected %s, got %s\n", "<Type1>", "<Type2>");
                        exit(-1);
                    }
                }
                else {
                    global_variables[name] = type;
                    return type;
                }
            } else {
                if (local_variables[name]) {
                    if (local_variables[name] == type) {
                        return local_variables[name];
                    } else {
                        fprintf(stderr, "TYPE MISMATCH: Expected %s, got %s\n", "<Type1>", "<Type2>");
                        exit(-1);
                    }
                }
                else {
                    local_variables[name] = type;
                    return type;
                }
            }
        }

        if (global) {
            return global_variables[name];
        } else {
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

    // Ternary operator.
    if (peak_next_token().type == TOKEN_SYMBOL_QUESTION_MARK) {
        consume();

        type_t type1 = Expr();

        if (peak_next_token().type == TOKEN_SYMBOL_COLON) {
            consume();

            type_t type2 = Expr();

            // TODO: Check that the types are the same.
            return type1;
        }
        else {
            error(":",
                  peak_next_token().value,
                  peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }
    }

    // Casting and Parenthesis wrapped expressions.
    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        type_t type;

        consume();

        if (peak_next_token().type == TOKEN_TYPE) {
            if (peak_next_token().value == "int") {
                type = INTEGER;
            }
            else if (peak_next_token().value == "double") {
                type = DOUBLE;
            }
            else if (peak_next_token().value == "char") {
                type = CHAR;
            }
            else if (peak_next_token().value == "void") {
                type = VOID;
            }
            else if (peak_next_token().value == "string") {
                type = STRING;
            }
            else {
                error("<Type>",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
            }
            else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }

            return Expr();
        }
        else {
            type = Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                return type;
            }
            else {
                error(")",
                      peak_next_token().value,
                      peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }
}

// <Type> := int | double | char | void | string
parser::type_t parser::Type() {
    type_t type = type_t::UNDEFINED;

    if (peak_next_token().type == TOKEN_TYPE) {
        if (peak_next_token().value == "int") {
            type = type_t::INTEGER;
        }
        else if (peak_next_token().value == "double") {
            type = type_t::DOUBLE;
        }
        else if (peak_next_token().value == "char") {
            type = type_t::CHAR;
        }
        else if (peak_next_token().value == "void") {
            type = type_t::VOID;
        }
        else if (peak_next_token().value == "string") {
            type = type_t::STRING;
        }
        else {
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