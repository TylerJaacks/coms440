//
// Created by Tyler Jaacks on 3/6/22.
//

#ifndef COMS440_PARSER_H
#define COMS440_PARSER_H

#include <vector>

#include "token.h"

class parser {
public:
    std::vector<token> tokens;

    int currentTokenIndex;

public:
    parser(std::vector<token> tokens) { this->tokens = tokens; currentTokenIndex = 0;}

private:
    /**
     * Gets the next token without consuming the current token.
     * @return the next token.
     */
    token peak_next_token();

    /**
     * Gets the previous token without consuming the previous token.
     * @return the previous token.
     */
    token peak_previous_token();

    /**
     * Gets the current token and advance the current token index.
     * @return the current token.
     */
    token consume_token();

public:
    void S();

    void Program();

    void DeclList();

    void Decl();

    void VarDecl();

    void ScopedVarDecl();

    void VarDeclList();

    void VarDeclInit();

    void VarDeclId();

    void Type();

    void FuncDecl();

    void Params();

    void ParamList();

    void ParamTypeList();

    void ParamIdList();

    void ParamId();

    void Stmt();

    void ExprStmt();

	void CompoundStmt();

    void LocalDecls();

	void ReturnStmt();

	void BreakStmt();

	void Expr();

    void StmtList();

    void IfStmt();

	void WhileStmt();

	void DoWhileStmt();

	void ForStmt();

    void SimpleExpr();

    void AndExpr();

    void UnaryRelExpr();

    void RelExpr();

    void RelOp();

    void SumExpr();

    void SumOp();

    void MultExpr();

    void MultOp();

    void Factor();

    void Mutable();

    void Immutable();

    void Call();

    void Args();

    void ArgsList();

    void Constant();

    void NumberConstant();

    void DoubleConstant();

    void CharConstant();

    void StringConstant();
    void Id();
};

#endif //COMS440_PARSER_H
