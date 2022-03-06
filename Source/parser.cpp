//
// Created by Tyler Jaacks on 3/6/22.
//

#include "parser.h"

parser::parser(std::vector<token> tokens) {
    this->tokens = tokens;
    this->currentTokenIndex = 0;
    this->totalTokens = tokens.size();
}

/**
 * GRAMMAR
 *
 * Program := ProgramStmt+
 *
 * ProgramStmt := PreprocessorDirective+ VarDecl+ FuncDecl+ FuncProto+
 *
 * PreprocessorDirective := TODO
 *
 * - TODO: Fix VarDecl in the form int a, b, c = 10; and int a, b, c;
 * VarDecl := Type Identifier ';' | Type Identifier '=' ArithExpr ';' | Type Identifier+ ';' | Type Identifier+ '=' ArithExpr ';'
 *
 * FuncDecl := Type Identifier '(' FuncParamDecl+ ')' '{' Stmt+ '}'
 *
 * - TODO: Fix this so there isn't a comma for the last parameter.
 * FuncParamDecl := Type Identifier ','
 *
 * Stmt := VarDecl | IfStmt | WhileStmt | DoWhileStmt | ForStmt | ElseIfStmt | ElseStmt | FuncCallStmt
 *
 * IfStmt := 'if' '(' BoolCond ')' '{' Stmt+ '}' 
 *  | 'if' '(' BoolCond ')' '{' Stmt+ '}' ElseStmt 
 *  | 'if' '(' BoolCond ')' '{' Stmt+ '}' ElseIfStmt
 *  | 'if' '(' BoolCond ')' '{' Stmt+ '}' ElseIfStmt ElseStmt
 *
 * WhileStmt := 'while' '(' BoolCond ')' '{' Stmt+ '}'
 *
 * DoWhileStmt := do '{' Stmt+ '}' 'while' '(' BoolCond ')' '{' Stmt+ '}'
 *
 * - TODO: Maybe don't have a IncrementStmt by including it in the ArithExpr.
 *  ForStmt := 'for' '(' VarDecl BoolCond ';' IncrementStmt ')' '{' Stmt+ '}'
 * 
 *  ElseStmt := 'else' '{' Stmt+ '}'
 *  
 *  ElseIf := 'else' 'if' '{' Stmt+ '}'
 *
 * - TODO: Not sure if this is correct.
 * IncrementStmt := Identifier++
 *  | Identifier--
 *  | Identifier += ArithExpr
 *  | Identifier -= ArithExpr
 *  | Identifier /= ArithExpr
 *  | Identifier *= ArithExpr
 *
 * FuncCallStmt := Type Identifier '(' FuncParam+ ')'
 *
 * FuncParam := IntegerLiteral, | RealLiteral, | StringLiteral, | CharLiteral,
 *
 * - TODO: Not sure this is correct.
 * BoolCond := BoolCond
 *  | !(BoolCond)
 *  | (BoolCond < BoolCond)
 *  | (BoolCond > BoolCond)
 *  | (BoolCond <= BoolCond)
 *  | (BoolCond >= BoolCond)
 *  | (BoolCond == BoolCond)
 *  | (BoolCond != BoolCond)
 *  | !BoolCond
 *  | BoolCond < BoolCond
 *  | BoolCond > BoolCond
 *  | BoolCond <= BoolCond
 *  | BoolCond >= BoolCond
 *  | BoolCond == BoolCond
 *  | BoolCond != BoolCond
 *  | Identifier
 *  | IntegerLiteral
 *  | ArithExpr
 *
 * - TODO: Handle Precedence
 * ArithExpr :=
 *   ArithExpr ';'
 *  | (ArithExpr + ArithExpr)
 *  | ArithExpr + ArithExpr
 *  | (ArithExpr - ArithExpr)
 *  | ArithExpr - ArithExpr
 *  | ArithExpr * ArithExpr
 *  | (ArithExpr * ArithExpr)
 *  | ArithExpr / ArithExpr
 *  | (ArithExpr / ArithExpr)
 *  | ArithExpr % ArithExpr
 *  | (ArithExpr % ArithExpr)
 *  | Identifier
 *  | IntegerLiteral
 *  | RealLiteral
 */

void parser::parse() {
    for (int i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
        }
    }
}