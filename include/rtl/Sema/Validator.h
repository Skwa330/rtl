#ifndef RTL_SEMA_VALIDATOR_H
#define RTL_SEMA_VALIDATOR_H

#include "rtl/Parser/AST.h"

#include "Sema.h"

namespace rtl {
    namespace sema {
        class Validator {
        private:
            std::vector<std::shared_ptr<parser::ASTNode>> &nodes;
            std::vector<core::Error> &errors;

            std::shared_ptr<parser::ASTFunctionHeader> currentFunction;
            std::shared_ptr<parser::ASTBlock> currentBlock;

            std::string unqualifyName(const std::shared_ptr<parser::ASTNode> &name);
            bool compareQualifiedNames(const std::shared_ptr<parser::ASTNode> &left, const std::shared_ptr<parser::ASTNode> &right);
            std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>> isRepeatDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl, const std::shared_ptr<parser::ASTBlock> &block);
        public:
            Validator(std::vector<std::shared_ptr<parser::ASTNode>> &nodes, std::vector<core::Error> &errors);

            void validateFunction(const std::shared_ptr<parser::ASTFunctionHeader> &header);
            void validateBlock(const std::shared_ptr<parser::ASTBlock> &block);

            void validateVariableDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl);
            void validateVariableDefinition(const std::shared_ptr<parser::ASTVariableDefinition> &defn);

            void validate();
        };
    }
}

#endif /* RTL_SEMA_VALIDATOR_H */