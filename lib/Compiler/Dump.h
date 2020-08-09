#ifndef RLT_COMPILER_DUMP_H
#define RLT_COMPILER_DUMP_H

#include "rlt/Parser/AST.h"

namespace rlt {
    namespace compiler {
        void dumpNode(const std::shared_ptr<rlt::parser::ASTNode>& node);
    }
}

#endif /* RLT_COMPILER_DUMP_H */