//
// Created by Tyler Jaacks on 5/11/22.
//

#ifndef COMS440_AST_H
#define COMS440_AST_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "types.h"

typedef enum {
    Undefined,
    Program,
    DeclList,
    VarDecl,
    FuncProto,
    FuncDef,
    FuncBody,
    FuncCall,
    Block,
    EmptyStmt,
    StmtList,
    ExprStmt,
    IfStmt,
    WhileStmt,
    DoWhileStmt,
    ForStmt,
    ReturnStmt,
    BreakStmt,
    ContinueStmt,
    ExprOp,
    ExprLiteral,
    ExprVar,
} ASTNodeType;

class ASTNode {
public:
    ASTNode(ASTNodeType nodeType, ASTNode *parent, ASTNode *leftChild, ASTNode *rightChild);

    ASTNode(ASTNodeType param);

    ASTNodeType nodeType = Undefined;

    type_t nodeValueType = type_t::UNDEFINED;
    std::shared_ptr<ASTNode> parent, leftChild, rightChild;
    std::vector<std::string> nodeValues;
    std::map<std::string, type_t> nodeMap;

public:
    ASTNode(ASTNodeType nodeType,
            std::shared_ptr<ASTNode> parent,
            std::shared_ptr<ASTNode> leftChild,
            std::shared_ptr<ASTNode> rightChild);

    ~ASTNode() = default;

    ASTNodeType getNodeType() { return nodeType; }

    std::vector<std::string> getNodeValues() { return nodeValues; }

    type_t getNodeValueType() { return nodeValueType; }

    ASTNode *getParent() { return parent.get(); }

    ASTNode *getLeftChild() { return leftChild.get(); }

    ASTNode *getRightChild() { return rightChild.get(); }

    void setNodeType(ASTNodeType nodeType) { this->nodeType = nodeType; }

    void setNodeValues(std::vector<std::string> nodeValues) { this->nodeValues = nodeValues; }

    void setNodeValueType(type_t valueType) { this->nodeValueType = valueType; }

    void setParent(std::shared_ptr<ASTNode> parentNode) { this->parent = std::move(parentNode); }

    void setLeftChild(std::shared_ptr<ASTNode> leftChildNode) { this->leftChild = std::move(leftChildNode); }

    void setRightChild(std::shared_ptr<ASTNode> rightChildNode) { this->rightChild = std::move(rightChildNode); }

    static void PrintTree(std::shared_ptr<ASTNode> node);
};

#endif //COMS440_AST_H
