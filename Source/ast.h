//
// Created by Tyler Jaacks on 3/10/22.
//

#ifndef COMS440_AST_H
#define COMS440_AST_H

#include <string>
#include <vector>

class ASTNode {
};

class ASTProgram : public ASTNode {
public:
    std::vector<ASTDeclNode> *declList{};

    ASTProgram(std::vector<ASTDeclNode> *declList) {
        this->declList = declList;
    }
};

class ASTDeclNode : public ASTNode {
public:
    std::vector<VarDeclNode> *varDeclList;
    std::vector<FuncDeclNode> *funcDeclList;
    st::vector<FuncPrototypeNode> *funcProtoList;

    ASTDeclNode() {
        varDeclList = new std::vector<VarDeclNode>();
        funcDeclList = new std::vector<FuncDeclNode>();
    }

    ASTDeclNode(std::vector<VarDeclNode> *varDeclList, std::vector<FuncDeclNode> *funcDeclList) {
        this->varDeclList = varDeclList;
        this->funcDeclList = funcDeclList;
    }
};

class VarDeclNode : public ASTNode {
public:
    type_t type;
    std::vector<VarIdNode> *varIdList;
};

class FuncPrototypeNode : public ASTNode {
public:
    type_t returnType;
    std::string name;
    std::vector<VarDeclParmNode> *paramList;
};

class VarDeclParamNode : public ASTNode {
public:
    type_t type;
    std::string name;
};

class FuncDeclNode : public ASTNode {
public:
    type_t returnType;
    std::string name;
    std::vector<VarDeclParmNode> *paramList;
    BodyNode *body;
};

class BodyNode : public ASTNode {
public:
    std::vector<StmtNode> *stmtList;
};

class StmtNode : public ASTNode {
public:
    ReturnStmtNode returnStmtNode;
    ContinueStmtNode continueStmtNode;
    BreakStmtNode breakStmtNode;
    IfStmtNode ifStmtNode;
    WhileStmtNode whileStmtNode;
    ForStmtNode forStmtNode;
    DoWhileStmtNode doWhileStmtNode;
    ExprStmtNode exprStmtNode;
};

class ReturnStmtNode : public StmtNode {
public:
    ExprNode *expr;
};

class ContinueStmtNode : public StmtNode {
public:
    ContinueStmtNode();
};

class BreakStmtNode : public StmtNode {
public:
    BreakStmtNode();
};

class IfStmtNode : public StmtNode {
public:
    ExprNode *expr;
    BodyNode *body;
    std::vector<IfStmtNode> *elseIfList;
    BodyNode *elseList;
};

class ForStmtNode : public StmtNode {
public:
    ExprNode *init;
    ExprNode *cond;
    ExprNode *incr;
    BlockNode *body;
};

class WhileStmtNode : public StmtNode {
public:
    ExprNode *expr;
    BlockNode *body;
};

class DoWhileStmtNode : public StmtNode {
public:
    ExprNode *expr;
    BlockNode *body;
};

class FuncCallParamsNode : public ASTNode {
public:
    std::vector<ExprNode> *exprList;
};

class ExprNode : public ASTNode {
public:
    LiteralNode literalNode;
    BinOpNode binOpNode;
    UnaryOpNode unaryOpNode;
    IdentifierNode identifierNode;
    GroupingSymbolNode groupingSymbolNode;
    FuncCallNode funcCallNode;
};

class FuncCallNode : public ExprNode {
    std::string name;
    FuncCallParamsNode *params;
};

class GroupingSymbolNode : public ExprNode {
public:
    char symbol;
};

class IdentifierNode : public ExprNode {
public:
    type_t type;
    std::string name;
};

class LiteralNode : public ExprNode {
public:
    type_t type;
    int intLiteral;
    float floatLiteral;
    std::string stringLiteral;
    bool boolLiteral;
};

#endif //COMS440_AST_H
