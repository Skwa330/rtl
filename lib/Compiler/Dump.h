#ifndef RTL_COMPILER_DUMP_H
#define RTL_COMPILER_DUMP_H

#include "rtl/Parser/AST.h"

namespace rtl {
    namespace compiler {
        std::string dumpNode(const std::shared_ptr<parser::ASTNode> &node, std::size_t ind = 0);
    }
}

#endif /* RTL_COMPILER_DUMP_H */