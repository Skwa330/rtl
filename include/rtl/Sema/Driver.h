#ifndef RTL_SEMA_DRIVER_H
#define RTL_SEMA_DRIVER_H

#include "rtl/Parser/AST.h"
#include "rtl/Core/Error.h"

#include "Sema.h"
#include "Type.h"

namespace rtl {
    namespace sema {
        // The Driver is the main class in semantic analysis and it first validates the nodes, then types, and type checks them.
        class Driver {
        private:
            std::vector<std::shared_ptr<parser::ASTNode>> &nodes;
            std::vector<core::Error> &errors;

            BuiltinTypes builtinTypes;
        public:
            Driver(std::vector<std::shared_ptr<parser::ASTNode>> &nodes, std::vector<core::Error> &errors);

            void run();
        };
    }
}

#endif /* RTL_SEMA_DRIVER_H */