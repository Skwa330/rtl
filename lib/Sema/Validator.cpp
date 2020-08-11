#include "rtl/Sema/Validator.h"

namespace rtl {
    namespace sema {
        Validator::Validator(std::vector<std::shared_ptr<parser::ASTNode>> &nodes, std::vector<core::Error> &errors) : nodes(nodes), errors(errors) {
        }

        std::string Validator::unqualifyName(const std::shared_ptr<ASTNode> &name) {
            std::string result;

            if (name->getType() == ASTType::Literal) {
                auto literal = std::reinterpret_pointer_cast<ASTLiteral>(name);
                result = literal->getString();
            } else if (name->getType() == ASTType::BinaryOperator) {
                auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(name);
                result = unqualifyName(binop->left) + unqualifyName(binop->right);
            }

            return result;
        }

        bool Validator::compareQualifiedNames(const std::shared_ptr<ASTNode> &left, const std::shared_ptr<ASTNode> &right) {
            if (left->getType() != right->getType()) return false;
            auto ty = left->getType();

            if (ty == ASTType::Literal) {
                auto leftLiteral = std::reinterpret_pointer_cast<ASTLiteral>(left);
                auto rightLiteral = std::reinterpret_pointer_cast<ASTLiteral>(right);

                return leftLiteral->hash == rightLiteral->hash;
            } else if (ty == ASTType::BinaryOperator) {
                auto leftBinop = std::reinterpret_pointer_cast<ASTBinaryOperator>(left);
                auto rightBinop = std::reinterpret_pointer_cast<ASTBinaryOperator>(right);

                return compareQualifiedNames(leftBinop->left, rightBinop->left) && compareQualifiedNames(leftBinop->right, rightBinop->right);
            }

            return false;
        }

        std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>> Validator::isRepeatDeclaration(const std::shared_ptr<ASTVariableDeclaration> &decl, const std::shared_ptr<ASTBlock> &block) {
            for (auto &node : block->nodes) {
                if (node->getType() == ASTType::VariableDeclaration) {
                    auto decl2 = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);
                    if (decl == decl2) break;

                    if (compareQualifiedNames(decl->name, decl2->name)) {
                        return std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>>(true, decl2);
                    }
                }
            }

            return std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>>(false, {});
        }

        void Validator::validateFunction(const std::shared_ptr<ASTFunctionHeader> &function) {
            auto lastFunction = currentFunction;
            currentFunction = function;

            if ((function->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) && (function->flags & (std::uint32_t)ASTFunctionHeader::Flags::Extern)) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin, function->end, "a function can't be external and foreign.");
            }

            if ((function->flags & (std::uint32_t)ASTFunctionHeader::Flags::CCall) && (function->flags & (std::uint32_t)ASTFunctionHeader::Flags::FastCall)) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin, function->end, "a function can't have multiple calling conventions.");
            }

            if (function->body) {
                validateBlock(function->body->block);
            }

            currentFunction = lastFunction;
        }

        void Validator::validateBlock(const std::shared_ptr<ASTBlock> &block) {
            auto lastBlock = currentBlock;
            currentBlock = block;

            for (auto &node : block->nodes) {
                using Ty = ASTType;
                switch (node->getType()) {
                    case Ty::VariableDefinition: {
                        validateVariableDefinition(std::reinterpret_pointer_cast<ASTVariableDefinition>(node));
                        break;
                    }
                    case Ty::VariableDeclaration: {
                        validateVariableDeclaration(std::reinterpret_pointer_cast<ASTVariableDeclaration>(node));
                        break;
                    }

                    case Ty::BinaryOperator: {
                        validateBinop();
                        auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(node);

                        if (binop->binopType == ASTBinaryOperator::Type::Assign) {
                            validateExpression(binop->left);
                            validateExpression(binop->right);
                        }
                    }
                }
            }

            currentBlock = lastBlock;
        }

        void Validator::validateVariableDeclaration(const std::shared_ptr<parser::ASTVariableDeclaration> &decl) {
            if (std::pair<bool, std::shared_ptr<parser::ASTVariableDeclaration>> result; (result = isRepeatDeclaration(decl, block)).second) {
                errors.emplace_back(core::Error::Type::Semantic, decl->begin, decl->end, fmt::format("redeclaration of variable '{}'; previous declaration occurred at: {}:{}:{}.", unqualifyName(decl->name), result.second->begin.moduleName, result.second->begin.line, result.second->begin.lexpos));
            }
        }

        void Validator::validateVariableDefinition(const std::shared_ptr<parser::ASTVariableDefinition> &defn) {
            validateVariableDeclaration(defn->decl);
        }

        void Validator::validate() {
            for (auto &node : nodes) {
                if (node->getType() == ASTType::FunctionHeader) {
                    validateFunction(std::reinterpret_pointer_cast<ASTFunctionHeader>(node));
                }
            }
        }
    }
}