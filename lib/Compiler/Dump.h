#ifndef FOSL_COMPILER_DUMP_H
#define FOSL_COMPILER_DUMP_H

#include "fosl/Parser/AST.h"

namespace fosl {
    namespace compiler {
        void dumpNode(const std::shared_ptr<fosl::parser::ASTNode>& node);
    }
}

#endif /* FOSL_COMPILER_DUMP_H */