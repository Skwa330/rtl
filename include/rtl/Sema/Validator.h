#ifndef RTL_SEMA_VALIDATOR_H
#define RTL_SEMA_VALIDATOR_H

#include "rtl/Parser/AST.h"
#include "rtl/Core/Error.h"

#include "Typer.h"
#include "Sema.h"

namespace rtl {
    namespace sema {
        class Validator {
        private:
            std::shared_ptr<Typer> typer;

            std::shared_ptr<BuiltinTypes> builtinTypes;
            std::vector<std::shared_ptr<parser::ASTNode>> &nodes;
            std::vector<core::Error> &errors;

            std::shared_ptr<parser::ASTFunctionHeader> currentFunction {};
            std::shared_ptr<parser::ASTBlock> currentBlock {};
            std::shared_ptr<parser::ASTNode> currentStatement {};

            std::shared_ptr<parser::ASTFor> currentFor {};
            std::shared_ptr<parser::ASTWhile> currentWhile {};

            std::string unqualifyName(const std::shared_ptr<parser::ASTNode> &name);
            bool compareQualifiedNames(const std::shared_ptr<parser::ASTNode> &left, const std::shared_ptr<parser::ASTNode> &right);
            std::pair<bool, std::shared_ptr<parser::ASTFunctionHeader>> isRepeatFunctionDeclaration(const std::shared_ptr<parser::ASTFunctionHeader> &decl);
            std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>> isRepeatDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl);

            bool isImplicitlyConvertible(const std::shared_ptr<Type> &left, const std::shared_ptr<Type> &right);
            bool compareTypes(const std::shared_ptr<Type> &left, const std::shared_ptr<Type> &right);

            std::shared_ptr<parser::ASTRef> findQualified(const std::shared_ptr<parser::ASTNode> &qlf);
        public:
            Validator(std::shared_ptr<BuiltinTypes> builtinTypes, std::vector<std::shared_ptr<parser::ASTNode>> &nodes, std::vector<core::Error> &errors);

            void validateFunction(const std::shared_ptr<parser::ASTFunctionHeader> &header);
            void validateBlock(const std::shared_ptr<parser::ASTBlock> &block);

            void validateVariableDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl);
            void validateVariableDefinition(const std::shared_ptr<parser::ASTVariableDefinition> &defn);

            void validateIf(const std::shared_ptr<parser::ASTIf> &ifStatement);

            void validateFor(const std::shared_ptr<parser::ASTFor> &forStatement);
            void validateRange(const std::shared_ptr<parser::ASTRange> &range);
            void validateWhile(const std::shared_ptr<parser::ASTWhile> &whileStatement);

            void validateContinue(const std::shared_ptr<parser::ASTContinue> &continueStatement);
            void validateBreak(const std::shared_ptr<parser::ASTBreak> &breakStatement);

            void validateReturn(const std::shared_ptr<parser::ASTReturn> &returnStatement);

            std::shared_ptr<parser::ASTNode> validateExpression(const std::shared_ptr<parser::ASTExpression> &expr);

            void validateNode(std::shared_ptr<parser::ASTNode> &node);

            void validate();
        };
    }
}

#endif /* RTL_SEMA_VALIDATOR_H */