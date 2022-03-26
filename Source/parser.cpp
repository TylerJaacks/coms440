#include "parser.h"

void parser::error(std::string expectedToken, std::string tokenValue, std::string fileName, int lineNumber) {
    std::fprintf(stderr, R"(ERROR: Expected '%s' but got '%s' instead. FILE: %s LINE: %d)", expectedToken.c_str(),
                 tokenValue.c_str(), fileName.c_str(), lineNumber);
    exit(EXIT_FAILURE);
}

token parser::peak_next_token() {
    return tokens[currentTokenIndex + 1];
}

token parser::peak(int n) {
    return tokens[currentTokenIndex + n];
}

token parser::consume() {
    token token;

    if (currentTokenIndex != -1) {
        token = tokens[currentTokenIndex];
    }

    currentTokenIndex += 1;

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

// Decl :=  VarDecl | FuncDecl
void parser::Decl() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    // Check to see if our next token is function declaration or variable declaration.
    if (peak(3).type == TOKEN_SYMBOL_LEFT_PAREN) {
        FuncDecl();
    } else {
        VarDecl();
    }
}

// DeclList :=  Decl DeclList
void parser::DeclList() {
    if (peak_next_token().type == TOKEN_EOF)
        return;

    Decl();
    DeclList();
}

// VarDecl := Type VarDeclList ';' | Type VarDeclList '=' Expr ';'
void parser::VarDecl() {
    Type();
    VarDeclList();

    // Check to see if this is a variable declaration with or without assignment.
    if (match(TOKEN_SYMBOL_EQUAL_SIGN)) {
        consume();

        Expr();

        // ERROR: If we don't terminate variable declaration with ';'.
        if (match(TOKEN_SYMBOL_SEMICOLON)) {
            consume();
            return;
        }
        else {
            error(";", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
        }
    }
    else if (match(TOKEN_SYMBOL_SEMICOLON)) {
        consume();
    }
    // ERROR: If we don't terminate variable declaration with ';'.
    else {
        error(";", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
    }
}

// VarDeclList := VarDeclId | VarDeclId ',' VarDeclList
void parser::VarDeclList() {
    VarDeclId();

    if (match(TOKEN_SYMBOL_COMMA)) {
        consume();

        VarDeclList();
    }
}

// VarDeclId := Id | Id '[' IntegerConstant ']'
void parser::VarDeclId() {
    Id();

    // Check to see if we have an array or not.
    if (match(TOKEN_SYMBOL_LEFT_BRACKET)) {
        consume();

        IntegerConstant();

        // ERROR: If we have seen a left bracket then we need it to be closed with a right one/
        if (!match(TOKEN_SYMBOL_RIGHT_BRACKET)) {
            error("]", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
                  peak_next_token().lineNumber);
        }
    }
}

// FuncDecl := Type Id '(' FuncDeclParams ')' ';' | Type Id '(' FuncDeclParams ')' Block
void parser::FuncDecl() {
    Type();
    Id();

    // ERROR: If we don't close out the function declaration with a closing parenthesis.
    if (!match(TOKEN_SYMBOL_LEFT_PAREN)) {
        error("(", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
    }

    consume();

    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        FuncDeclParams();
    }

    if (!match(TOKEN_SYMBOL_RIGHT_PAREN)) {
        error(")", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
    }

    consume();

    // TODO: Note this will not catch errors with missing semicolons.
    if (match(TOKEN_SYMBOL_SEMICOLON)) consume();
    else if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACE) Block();
    else {
        error(R"('{' or ';')", peak_next_token().value, peak_next_token().fileName, peak_next_token().lineNumber);
    }
}

// FuncDeclParams := FuncDeclParamList
void parser::FuncDeclParams() {
    FuncDeclParamList();
}

// FuncDeclParamList := e | FuncDeclParam | FuncDeclParam ',' FuncDeclParamList
void parser::FuncDeclParamList() {
    if (!match(TOKEN_SYMBOL_LEFT_PAREN)) {
        FuncDeclParam();

        if (match(TOKEN_SYMBOL_COMMA)) {
            consume();

            FuncDeclParamList();
        }
    }
}

// FuncDeclParam := Type Id
void parser::FuncDeclParam() {
    Type();
    Id();
}

// Block := '{' Stmt StmtList '}'
void parser::Block() {
    if (match(TOKEN_SYMBOL_LEFT_BRACE)) {
        consume();

        if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_BRACE) {
            Stmt();
            StmtList();
        }

        if (match(TOKEN_SYMBOL_RIGHT_BRACE)) {
            consume();
        } else {
            // ERROR: If we don't close the code block with a closing curly brace.
            error("}", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
                  peak_next_token().lineNumber);
        }
    } else {
        // ERROR: If we don't start the block with an opening curly brace.
        error("{", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
    }
}

// Stmt := e | VarDecl | FuncCall | IfStmt | ElseStmt | ElseIfStmt | ForStmt | WhileStmt | DoWhileStmt | ReturnStmt | BreakStmt | AssignStmt
void parser::Stmt() {
    if (peak_next_token().type == TOKEN_TYPE_INTEGER ||
            peak_next_token().type == TOKEN_TYPE_DOUBLE ||
            peak_next_token().type == TOKEN_TYPE_CHAR) {
        VarDecl();
    }
    else {
        Expr();

        if (match(TOKEN_SYMBOL_SEMICOLON)) {
            consume();
            return;
        }
        else {
            error(";", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);
        }
    }
}

// Expr :=
//  ✅ Literal |
//  ✅ Id '(' ExprList ')' |
//  UnaryOp Expr |
//  ✅ LValue |
//  ✅ LValue AssignOperator Expr |
//  ✅ LValue '--' |
//  ✅ LValue '++' |
//  Expr BinaryOp Expr
//  Expr '?' Expr ':' Expr
//  ✅ '(' Type ')' Expr
//  ✅ '(' Expr ')'
void parser::Expr() {
    // Make sure we aren't preforming a function call instead.
    if (peak(2).type != TOKEN_SYMBOL_LEFT_PAREN) {

        if (peak_next_token().type == TOKEN_LITERAL_NUMBER) {
            ParseExpression();

            consume();

            return;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_REAL) {
            consume();

            return;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_CHAR) {
            consume();

            return;
        }
        else if (peak_next_token().type == TOKEN_LITERAL_STRING) {
            consume();

            return;
        }
    }

    if (peak_next_token().type == TOKEN_IDENTIFIER) {
        consume();

        if (peak_next_token().type == TOKEN_SYMBOL_LEFT_PAREN) {
            consume();

            ExprList();

            if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_PAREN) {
                consume();

                return;
            }
        }
        else {
            LValue();

            if (AssignOp()) {
                Expr();

                return;
            }

            if (peak_next_token().type == TOKEN_SYMBOL_INCREMENT) {
                consume();

                return;
            }

            if (peak_next_token().type == TOKEN_SYMBOL_DECREMENT) {
                consume();

                return;
            }
        }
    }

    bool has_seen_type;

    if (match(TOKEN_SYMBOL_LEFT_PAREN)) {
        consume();

        if (match(TOKEN_TYPE_INTEGER)) {
            has_seen_type = true;
            consume();
        }
        else if (match(TOKEN_TYPE_VOID)) {
            has_seen_type = true;
            consume();
        }
        else if (match(TOKEN_TYPE_CHAR)) {
            has_seen_type = true;
            consume();
        }
        else if (match(TOKEN_TYPE_DOUBLE)) {
            has_seen_type = true;
            consume();
        }
        else if (match(TOKEN_TYPE_CHAR)) {
            has_seen_type = true;
            consume();
        }
        else {
            has_seen_type = false;
            Expr();
        }

        if (match(TOKEN_SYMBOL_RIGHT_PAREN)) {
            consume();

            if (has_seen_type) {
                Expr();

                return;
            }
        }
        else {
            error(")", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(), peak_next_token().lineNumber);

        }

        return;
    }
}

// TODO: Finish this.
bool parser::UnaryOp() {
    if (match(TOKEN_SYMBOL_EXCLAMATION_MARK) && peak(2).type != TOKEN_SYMBOL_EQUAL_SIGN) {
        consume();

        return true;
    }
    else if (match(TOKEN_SYMBOL_MINUS_SIGN)) {
        if (peak(2).type != TOKEN_SYMBOL_EQUAL_SIGN || peak(2).type != TOKEN_SYMBOL_MINUS_SIGN) {
            consume();

            return true;
        }

        return false;
    }
    else if (match(TOKEN_SYMBOL_TILDE)) {
        consume();

        return true;
    }

    return false;
}

// TODO: Finish this.
bool parser::BinaryOp()
{

}

// AssignOp :=
bool parser::AssignOp() {
    if (match(TOKEN_SYMBOL_PLUS_ASSIGNMENT)) {
        consume();
    }
    else if (match(TOKEN_SYMBOL_MINUS_ASSIGNMENT)) {
        consume();
    }
    else if (match(TOKEN_SYMBOL_MULT_ASSIGNMENT)) {
        consume();
    }
    else if (match(TOKEN_SYMBOL_DIVIDE_ASSIGNMENT)) {
        consume();
    }
    else if (match(TOKEN_SYMBOL_MODULO_ASSIGNMENT)) {
        consume();
    }

    return false;
}

void parser::ParseExpression() {
    token lhs = peak_next_token();
    consume();

    ParseExpression2(lhs, 0);
}

void parser::ParseExpression2(token lhs, int minPrecedence) {
    
}

// ExprList := Expr | Expr ',' ExprList
void parser::ExprList() {
    if (peak_next_token().type != TOKEN_SYMBOL_RIGHT_PAREN) {
        Expr();

        if (peak_next_token().type == TOKEN_SYMBOL_COMMA) {
            consume();

            ExprList();
        }

        return;
    }

    return;
}

// LValue := Id | Id '[' Expr ']'
void parser::LValue() {
    if (peak_next_token().type == TOKEN_SYMBOL_LEFT_BRACKET) {
        consume();

        Expr();

        if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACKET) {
            return;
        }
    }

    return;
}

// StmtList :=
void parser::StmtList() {
    if (peak_next_token().type == TOKEN_SYMBOL_RIGHT_BRACE) {
        return;
    }

    Stmt();
    StmtList();
}

// Type := INTEGER | DOUBLE | CHAR | VOID
void parser::Type() {
    if (peak_next_token().type == TOKEN_TYPE_INTEGER || peak_next_token().type == TOKEN_TYPE_DOUBLE ||
        peak_next_token().type == TOKEN_TYPE_CHAR || peak_next_token().type == TOKEN_TYPE_VOID) {
        consume();
    } else {
        error("<Type>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }
}

// Id := RegEx for Identifier
void parser::Id() {
    if (!match(TOKEN_IDENTIFIER)) {
        error("<Identifier>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }

    consume();
}

// Constant := IntegerConstant | DoubleConstant | CharConstant | StringConstant
void parser::Constant() {
    IntegerConstant();
    DoubleConstant();
    CharConstant();
    StringConstant();
}

void parser::IntegerConstant() {
    if (!match(TOKEN_LITERAL_NUMBER)) {
        error("<Integer Constant>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }

    consume();
}

void parser::DoubleConstant() {
    if (!match(TOKEN_LITERAL_REAL)) {
        error("<Double Constant>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }

    consume();
}

void parser::CharConstant() {
    if (!match(TOKEN_LITERAL_CHAR)) {
        error("<Char Constant>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }

    consume();
}

void parser::StringConstant() {
    if (!match(TOKEN_LITERAL_STRING)) {
        error("<String Constant>", peak_next_token().value.c_str(), peak_next_token().fileName.c_str(),
              peak_next_token().lineNumber);
    }

    consume();
}