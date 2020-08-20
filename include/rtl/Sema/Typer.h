#ifndef RTL_SEMA_TYPER_H
#define RTL_SEMA_TYPER_H

#include "Sema.h"

#include "rtl/Core/Error.h"
#include "rtl/Parser/AST.h"

namespace rtl {
    namespace sema {
        // This is the part of semantic-analysis which decides what type each node is.
        class Typer {
        private:
            std::vector<std::shared_ptr<parser::ASTNode>> &nodes;
            std::vector<core::Error> &errors;

            std::shared_ptr<BuiltinTypes> builtinTypes;
            std::vector<std::shared_ptr<TypeDeclaration>> typeDeclarations;

            std::shared_ptr<TypeDeclaration> getTypeDeclaration(const std::shared_ptr<parser::ASTNode> &identifier);
            std::shared_ptr<Type> mapType(const parser::Type &type);

            std::shared_ptr<parser::ASTFunctionHeader> currentFunction;
            std::shared_ptr<parser::ASTBlock> currentBlock;
        public:
            Typer(const std::shared_ptr<BuiltinTypes> &builtinTypes, std::vector<std::shared_ptr<parser::ASTNode>> &nodes, std::vector<core::Error> &errors);

            void typeFunction(const std::shared_ptr<parser::ASTFunctionHeader> &function); // When typing the block if we find a function call that isn't typed we type it.
            void typeVariableDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl);
            void typeVariableDefinition(const std::shared_ptr<parser::ASTVariableDefinition> &defn);
            void typeExpression(const std::shared_ptr<parser::ASTExpression> &expr);

            void typeType(parser::Type &type);

            void typeNode(const std::shared_ptr<parser::ASTNode> &node);
        };
    }
}

#endif /* RTL_SEMA_TYPER_H */