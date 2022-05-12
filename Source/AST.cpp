//
// Created by Tyler Jaacks on 5/11/22.
//

#include "AST.h"

void PrintTreeUtil(const std::string &prefix, const std::unique_ptr<ASTNode> node, bool isLeft) {
    if (node != nullptr) {
        printf("%s", prefix.c_str());

        printf((isLeft ? "├──" : "└──"));

        switch (node->nodeType) {
            case ASTNodeType::Undefined:
                printf("Undefined");
                break;
            case ASTNodeType::Program:
                printf("Program");
                break;
            case ASTNodeType::DeclList:
                printf("DeclList");
                break;
            case ASTNodeType::VarDecl:
                printf("VarDecl");
                break;
            case ASTNodeType::FuncProto:
                printf("FuncProto");
                break;
            case ASTNodeType::FuncDef:
                printf("FuncDef");
                break;
            case ASTNodeType::FuncBody:
                printf("FuncBody");
                break;
            case ASTNodeType::FuncCall:
                printf("FuncCall");
                break;
            case ASTNodeType::Block:
                printf("Block");
                break;
            case ASTNodeType::EmptyStmt:
                printf("EmptyStmt");
                break;
            case ASTNodeType::ExprStmt:
                printf("ExprStmt");
                break;
            case ASTNodeType::ExprLiteral:
                printf("ExprLiteral");
                break;
            case ASTNodeType::IfStmt:
                printf("IfStmt");
                break;
            case ASTNodeType::WhileStmt:
                printf("WhileStmt");
                break;
            case ASTNodeType::DoWhileStmt:
                printf("DoWhileStmt");
                break;
            case ASTNodeType::ForStmt:
                printf("ForStmt");
                break;
            case ASTNodeType::ReturnStmt:
                printf("ReturnStmt");
                break;
            case ASTNodeType::BreakStmt:
                printf("BreakStmt");
                break;
            case ASTNodeType::ContinueStmt:
                printf("ContinueStmt");
                break;
        }

        printf("\n");

        PrintTreeUtil(prefix + (isLeft ? "│   " : "    "), std::move(node->leftChild), true);
        PrintTreeUtil(prefix + (isLeft ? "│   " : "    "), std::move(node->rightChild), false);
    }
}

ASTNode::ASTNode(ASTNodeType nodeType,
                 std::unique_ptr<ASTNode> parent,
                 std::unique_ptr<ASTNode> leftChild,
                 std::unique_ptr<ASTNode> rightChild) {
    this->nodeType = nodeType;
    this->parent = std::move(parent);
    this->leftChild = std::move(leftChild);
    this->rightChild = std::move(rightChild);
}

void ASTNode::PrintTree(std::unique_ptr<ASTNode> root) {
    ::PrintTreeUtil("", std::move(root), false);
}

ASTNode::ASTNode(ASTNodeType param) {
    this->nodeType = param;
}

