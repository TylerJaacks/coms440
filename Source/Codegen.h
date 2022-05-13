//
// Created by Tyler Jaacks on 5/11/22.
//

#ifndef COMS440_CODEGEN_H
#define COMS440_CODEGEN_H

#include <memory>
#include <string>

#include "AST.h"

class Codegen {
private:
    std::shared_ptr<ASTNode> programNode;

public:
    explicit Codegen(std::shared_ptr<ASTNode> programNode) : programNode(std::move(programNode)) {}

    ~Codegen() = default;

public:
    std::vector<std::string> GenerateCode();
};


#endif //COMS440_CODEGEN_H
