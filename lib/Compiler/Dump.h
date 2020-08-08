#ifndef LANG_COMPILER_DUMP_H
#define LANG_COMPILER_DUMP_H

#include "lang/Parser/AST.h"

namespace lang {
    namespace compiler {
        void dumpNode(const std::shared_ptr<lang::parser::ASTNode>& node);
    }
}

#endif /* LANG_COMPILER_DUMP_H */