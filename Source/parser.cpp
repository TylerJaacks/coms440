#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

#include "parser.h"

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
    Type();
    Id();

    if (peak_next_token().type != TOKEN_SYMBOL_LEFT_PAREN) {
        error("(",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
    else {
        consume();
    }

    FuncDeclParamList();

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
void parser::FuncDeclParamList() {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        Type();
        Id();

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
                FuncDeclParamList();
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
    if (peak_next_token().type == TOKEN_KEYWORD_BREAK || peak_next_token().type == TOKEN_KEYWORD_CONTINUE) {
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
    if (peak_next_token().type == TOKEN_KEYWORD_RETURN) {
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
    if (peak_next_token().type == TOKEN_KEYWORD_IF) {
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
    if (peak_next_token().type == TOKEN_KEYWORD_FOR) {
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
    if (peak_next_token().type == TOKEN_KEYWORD_DO) {
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


    // TODO: Look at this later.
    Expr();

    if (peak_next_token().type != TOKEN_SYMBOL_SEMICOLON) {
        error(";",
              peak_next_token().value,
              peak_next_token().fileName,
              peak_next_token().lineNumber);
    }
    else {
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
    if (peak_next_token().type == TOKEN_IDENTIFIER) {
        type_t identType;

        identType = global_variables[peak_next_token().value];

        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            Expr();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();
                return identType;
            }
            else if (peak_next_token().type == TOKEN_LITERAL_STRING
                || peak_next_token().type == TOKEN_LITERAL_CHAR) {

                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
                    consume();

                    Expr();
                }

                if (match(TOKEN_SYMBOL_RIGHT_PAREN)) {
                    consume();
                    return identType;
                }
                else {
                    error(")", peak_next_token().value, peak_next_token().fileName, peak_next_token().lineNumber);
                }
            }
        }
        else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
            consume();

            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT
                || peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                consume();
            }

            Expr();

            if (match(TOKEN_SYMBOL_RIGHT_BRACKET)) {
                consume();

                if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT
                    || peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                    consume();
                }
                else {
                    return identType;
                }
            } else {
                error("]", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
        else {
            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
                peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                consume();
            }
            else {
                return identType;
            }
        }
    }
    // TODO: Handle Typing for Casting.
    else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
        consume();

        if (peak_next_token().type == TOKEN_TYPE) {
            consume();

            if (match(TOKEN_SYMBOL_RIGHT_PAREN)) {
                consume();

                Expr();

                return UNDEFINED;
            }
            else {
                error(")", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
        // TODO: Handle Typing for Expr();
        else {
            Expr();

            if (match(TOKEN_SYMBOL_RIGHT_PAREN)) {
                consume();

                return UNDEFINED;
            }
            else {
                error(")", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }
    else if (peak_next_token().type == TOKEN_LITERAL_NUMBER ||
        peak_next_token().type == TOKEN_LITERAL_STRING ||
        peak_next_token().type == TOKEN_LITERAL_CHAR ||
        peak_next_token().type == TOKEN_LITERAL_REAL) {

        type_t literalType = UNDEFINED;

        if (peak_next_token().type == TOKEN_LITERAL_NUMBER) {
            literalType = INTEGER;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_STRING) {
            literalType = STRING;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_CHAR) {
            literalType = CHAR;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_REAL) {
            literalType = DOUBLE;
        }

        consume();

        return literalType;
    }
    else if (peak_next_token().type == TOKEN_SYMBOL_EXCLAMATION_MARK ||
        peak_next_token().type == TOKEN_SYMBOL_TILDE ||
        peak_next_token().type == TOKEN_SYMBOL_INCREMENT ||
        peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
        consume();

        if (match(TOKEN_IDENTIFIER)) {
            consume();

            if (global) {
                return global_variables[peak_next_token().value];
            }
            else {
                return local_variables[peak_next_token().value];
            }
        }
        else {
            error("<Identifier>", peak_next_token().value, peak_next_token().fileName,
                  peak_next_token().lineNumber);
        }
    }
    else if (peak_next_token().type == TOKEN_SYMBOL_MINUS_SIGN) {
        while (peak_next_token().type == '-') {
            consume();
        }

        if (peak_next_token().type == TOKEN_LITERAL_NUMBER ||
            peak_next_token().type == TOKEN_LITERAL_REAL) {
            consume();

            if (peak_next_token().type == TOKEN_LITERAL_NUMBER) {
                return INTEGER;
            }
            else if (peak_next_token().type == TOKEN_LITERAL_REAL) {
                return DOUBLE;
            }
        }
        else {
            if (match(TOKEN_IDENTIFIER)) {
                consume();

                if (global) {
                    return global_variables[peak_next_token().value];
                }
                else {
                    return local_variables[peak_next_token().value];
                }
            }
            else {
                error("<Identifier>", peak_next_token().value, peak_next_token().fileName,
                      peak_next_token().lineNumber);
            }
        }
    }
    else {
        error("<Term>", peak_next_token().value, peak_next_token().fileName,
              peak_next_token().lineNumber);
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
void parser::Id() {
    if (!match(TOKEN_IDENTIFIER)) {
        error("<Identifier>", peak_next_token().value, peak_next_token().fileName,
              peak_next_token().lineNumber);
    }

    consume();
}