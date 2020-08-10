#ifndef RLT_COMPILER_DUMP_H
#define RLT_COMPILER_DUMP_H

#include "rlt/Parser/AST.h"

namespace rlt {
    namespace compiler {
        std::string dumpNode(const std::shared_ptr<rlt::parser::ASTNode> &node, std::size_t ind = 0);
    }
}

#endif /* RLT_COMPILER_DUMP_H */