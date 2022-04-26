//
// Created by Tyler Jaacks on 4/20/22.
//

#ifndef COMS440_CODEGEN_H
#define COMS440_CODEGEN_H

#include <map>
#include <string>
#include <vector>

#include "node.h"

class codegen {
    std::string generate_code(node *root);
};


#endif //COMS440_CODEGEN_H
