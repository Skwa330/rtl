#include "rtl/Sema/Validator.h"

#include <fmt/format.h>

using namespace rtl::parser;

namespace rtl {
    namespace sema {
        Validator::Validator(std::shared_ptr<BuiltinTypes> builtinTypes, std::vector<std::shared_ptr<ASTNode>> &nodes, std::vector<core::Error> &errors) : builtinTypes(builtinTypes), nodes(nodes), errors(errors) {
            typer = std::make_shared<Typer>(builtinTypes, nodes, errors);
        }

        std::string Validator::unqualifyName(const std::shared_ptr<ASTNode> &name) {
            std::string result;

            if (name->getType() == ASTType::Expression) {
                auto expr = std::reinterpret_pointer_cast<ASTExpression>(name);

                if (expr->getExprType() == ASTExpression::Type::Literal) {
                    auto literal = std::reinterpret_pointer_cast<ASTLiteral>(expr);
                    result = literal->getString();
                } else if (expr->getExprType() == ASTExpression::Type::BinaryOperator) {
                    auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(expr);
                    result = unqualifyName(binop->left) + unqualifyName(binop->right);
                }
            }

            return result;
        }

        bool Validator::compareQualifiedNames(const std::shared_ptr<ASTNode> &left, const std::shared_ptr<ASTNode> &right) {
            if (left->getType() != right->getType()) return false;

            if (left->getType() == ASTType::Expression) {
                auto ty = std::reinterpret_pointer_cast<ASTExpression>(left)->getExprType();

                if (ty == ASTExpression::Type::Literal) {
                    auto leftLiteral = std::reinterpret_pointer_cast<ASTLiteral>(left);
                    auto rightLiteral = std::reinterpret_pointer_cast<ASTLiteral>(right);

                    return leftLiteral->hash == rightLiteral->hash;
                } else if (ty == ASTExpression::Type::BinaryOperator) {
                    auto leftBinop = std::reinterpret_pointer_cast<ASTBinaryOperator>(left);
                    auto rightBinop = std::reinterpret_pointer_cast<ASTBinaryOperator>(right);

                    return compareQualifiedNames(leftBinop->left, rightBinop->left) && compareQualifiedNames(leftBinop->right, rightBinop->right);
                }
            }

            return false;
        }

        std::pair<bool, std::shared_ptr<ASTFunctionHeader>> Validator::isRepeatFunctionDeclaration(const std::shared_ptr<ASTFunctionHeader> &decl) {
            if (!currentFunction) return std::make_pair(false, std::shared_ptr<ASTFunctionHeader> {});

            for (auto &node : nodes) {
                if (node->getType() == ASTType::FunctionHeader) {
                    auto function = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);
                    if (decl == function) break;

                    if (compareQualifiedNames(currentFunction->name, function->name)) {
                        if (currentFunction->paramDecls.size() == function->paramDecls.size()) {
                            bool verbatim = true;

                            for (std::size_t i = 0; i < currentFunction->paramDecls.size(); i++) {
                                auto p1 = currentFunction->paramDecls[i];
                                auto p2 = function->paramDecls[i];

                                if (!compareTypes(p1->targetTy.evaluatedType, p2->targetTy.evaluatedType)) {
                                    verbatim = false;
                                    break;
                                }
                            }

                            if (verbatim) {
                                return std::make_pair(true, function);
                            }
                        }
                    }
                }
            }

            return std::make_pair(false, std::shared_ptr<ASTFunctionHeader> {});
        }

        std::pair<bool, std::shared_ptr<ASTVariableDeclaration>> Validator::isRepeatDeclaration(const std::shared_ptr<ASTVariableDeclaration> &decl) {
            if (!currentBlock) return std::make_pair(false, std::shared_ptr<ASTVariableDeclaration> {});

            auto block = currentBlock;

            for (auto &node : block->nodes) {
                if (node->getType() == ASTType::VariableDeclaration) {
                    auto decl2 = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);
                    if (decl == decl2) break;

                    if (compareQualifiedNames(decl->name, decl2->name)) {
                        return std::make_pair(true, decl2);
                    }
                }
            }

            if (currentFunction) {
                for (auto &paramDecl : currentFunction->paramDecls) {
                    if (paramDecl->getType() == ASTType::VariableDeclaration) {
                        auto decl2 = std::reinterpret_pointer_cast<ASTVariableDeclaration>(paramDecl);

                        if (compareQualifiedNames(decl->name, decl2->name)) {
                            return std::make_pair(true, decl2);
                        }
                    }
                }
            }

            return std::make_pair(false, std::shared_ptr<ASTVariableDeclaration> {});
        }

        bool Validator::isImplicitlyConvertible(const std::shared_ptr<Type> &left, const std::shared_ptr<Type> &right) {
            return false;
        }

        bool Validator::compareTypes(const std::shared_ptr<Type> &left, const std::shared_ptr<Type> &right) {
            if (left->decl->getTag() != right->decl->getTag()) return false;
            if (left->getPointer() != right->getPointer()) return false;
            // Eventually check if they're the same structure, enumeration, or union.

            return true;
        }

        std::shared_ptr<parser::ASTNode> Validator::findQualified(const std::shared_ptr<parser::ASTNode> &qlf) {
            if (currentBlock) {
                // Try to find variable in local scope

                for (auto &node : currentBlock->nodes) {
                    if (node->getType() == ASTType::VariableDeclaration) {
                        auto name = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node)->name;

                        if (compareQualifiedNames(name, qlf)) {
                            return node;
                        }
                    } else if (node->getType() == ASTType::VariableDefinition) {
                        auto name = std::reinterpret_pointer_cast<ASTVariableDefinition>(node)->decl->name;

                        if (compareQualifiedNames(name, qlf)) {
                            return node;
                        }
                    }
                }

                if (currentBlock->parent) {
                    auto last = currentBlock;
                    currentBlock = currentBlock->parent;
                    auto result = findQualified(qlf);
                    currentBlock = last;
                    return result;
                }
            }

            for (auto &node : nodes) {
                if (node->getType() == ASTType::FunctionHeader) {
                    auto name = std::reinterpret_pointer_cast<ASTFunctionHeader>(node)->name;

                    // We don't have global variables yet, but we will; I don't want to add this later...
                    if (compareQualifiedNames(name, qlf)) {
                        return node;
                    } else if (node->getType() == ASTType::VariableDeclaration) {
                        auto name = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node)->name;

                        if (compareQualifiedNames(name, qlf)) {
                            return node;
                        }
                    } else if (node->getType() == ASTType::VariableDefinition) {
                        auto name = std::reinterpret_pointer_cast<ASTVariableDefinition>(node)->decl->name;

                        if (compareQualifiedNames(name, qlf)) {
                            return node;
                        }
                    }
                }
            }

            return {};
        }

        void Validator::validateFunction(const std::shared_ptr<ASTFunctionHeader> &function) {
            auto lastFunction = currentFunction;
            currentFunction = function;

            typer->typeFunction(function);

            // Todo(Sean): Check for overloads
            if (std::pair<bool, std::shared_ptr<ASTFunctionHeader>> result; (result = isRepeatFunctionDeclaration(function)).second) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin.source, function->begin, function->end, fmt::format("redeclaration of function '{}'; previous declaration occured at: {}:{}:{}.", unqualifyName(function->name), result.second->begin.moduleName, result.second->begin.line, result.second->begin.lexpos));
                return;
            }

            if ((function->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) && (function->flags & (std::uint32_t)ASTFunctionHeader::Flags::Extern)) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin.source, function->begin, function->end, "a function can't be external and foreign.");
            }

            if ((function->flags & (std::uint32_t)ASTFunctionHeader::Flags::CCall) && (function->flags & (std::uint32_t)ASTFunctionHeader::Flags::FastCall)) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin.source, function->begin, function->end, "a function can't have multiple calling conventions.");
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
                validateNode(node);
            }

            currentBlock = lastBlock;
        }

        void Validator::validateVariableDeclaration(const std::shared_ptr<ASTVariableDeclaration> &decl) {
            if (!currentBlock) throw std::runtime_error("INVALID SCOPE IN rtl::sema::Validator::validateVariableDeclaration");

            typer->typeVariableDeclaration(decl);

            if (std::pair<bool, std::shared_ptr<ASTVariableDeclaration>> result; (result = isRepeatDeclaration(decl)).second) {
                errors.emplace_back(core::Error::Type::Semantic, decl->begin.source, decl->begin, decl->end, fmt::format("redeclaration of variable '{}'; previous declaration occurred at: {}:{}:{}.", unqualifyName(decl->name), result.second->begin.moduleName, result.second->begin.line, result.second->begin.lexpos));
            }
        }

        void Validator::validateVariableDefinition(const std::shared_ptr<ASTVariableDefinition> &defn) {
            validateVariableDeclaration(defn->decl);
            defn->expr = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(defn->expr));
        }

        void Validator::validateIf(const std::shared_ptr<ASTIf> &ifStatement) {
            ifStatement->condition = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(ifStatement->condition));
            validateNode(ifStatement->statement);
        }

        void Validator::validateFor(const std::shared_ptr<ASTFor> &forStatement) {
            forStatement->expr = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(forStatement->expr));
            validateNode(forStatement->statement);
        }

        void Validator::validateWhile(const std::shared_ptr<ASTWhile> &whileStatement) {
            whileStatement->condition = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(whileStatement->condition));
            validateNode(whileStatement->statement);
        }

        void Validator::validateContinue(const std::shared_ptr<ASTContinue> &continueStatement) {

        }

        void Validator::validateBreak(const std::shared_ptr<ASTBreak> &breakStatement) {

        }

        std::shared_ptr<ASTNode> Validator::validateExpression(const std::shared_ptr<ASTExpression> &expr) {
            auto result = expr;

            using Ty = ASTExpression::Type;
            if (expr->getExprType() == Ty::BinaryOperator) {
                if (!expr->evaluatedType) typer->typeExpression(expr);

                auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(expr);

                auto lhs = std::reinterpret_pointer_cast<ASTExpression>(binop->left);
                auto lty = lhs->getExprType();

                auto rhs = std::reinterpret_pointer_cast<ASTExpression>(binop->right);
                auto rty = rhs->getExprType();

                if (binop->binopType == ASTBinaryOperator::Type::MemberResolution) {
                    if ((lty == ASTExpression::Type::BinaryOperator && std::reinterpret_pointer_cast<ASTBinaryOperator>(lhs)->binopType != ASTBinaryOperator::Type::NamespaceResolution) || lty != ASTExpression::Type::Literal) {
                        errors.emplace_back(core::Error::Type::Semantic, lhs->begin.source, lhs->begin, lhs->end, "expected left-hand operand of type name or namespace resolution.");
                    } else if (lty == ASTExpression::Type::Literal) {
                        auto lit = std::reinterpret_pointer_cast<ASTLiteral>(lhs);
                        if (lit->literalType != ASTLiteral::Type::Name) {
                            errors.emplace_back(core::Error::Type::Semantic, lhs->begin.source, lhs->begin, lhs->end, "expected left-hand operand of type name.");
                        }
                    }

                    if (rty == ASTExpression::Type::Literal) {
                        auto lit = std::reinterpret_pointer_cast<ASTLiteral>(lhs);
                        if (lit->literalType != ASTLiteral::Type::Name) {
                            errors.emplace_back(core::Error::Type::Semantic, rhs->begin.source, rhs->begin, rhs->end, "expected right-hand operand of type name.");
                        }
                    } else {
                        errors.emplace_back(core::Error::Type::Semantic, rhs->begin.source, rhs->begin, rhs->end, "expected right-hand operand of type name.");
                    }
                }

                if (binop->binopType == ASTBinaryOperator::Type::NamespaceResolution) {
                    // We can't split this because we need to make sure that we are finding the name in the namespace.
                    auto node = findQualified(binop); // Find qualified global or local otherwise error.

                    if (!node) {
                        throw core::Error(core::Error::Type::Semantic, binop->begin.source, binop->begin, binop->end, fmt::format("undeclared reference to '{}'.", unqualifyName(binop)));
                    }

                    return node;
                } else {
                    validateExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->left));
                    validateExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->right));
                }

                if (!compareTypes(std::reinterpret_pointer_cast<ASTExpression>(binop->left)->evaluatedType, std::reinterpret_pointer_cast<ASTExpression>(binop->right)->evaluatedType)) {
                    errors.emplace_back(core::Error::Type::Semantic, binop->begin.source, binop->begin, binop->end, "cannot implicitly convert types between left and right expressions."); // Todo(Sean): Make this error message show the actual name of the type.
                }
            } else if (expr->getExprType() == Ty::Call) {
                auto call = std::reinterpret_pointer_cast<ASTCall>(expr);

                bool found = false;

                for (auto &arg : call->callArgs) {
                    arg = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(arg));
                }

                for (auto &node : nodes) {
                    if (node->getType() == ASTType::FunctionHeader) {
                        auto function = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);

                        if (!function->rt.evaluatedType) { // This is a bit sketchy lol
                            validateFunction(function);
                        }

                        if (compareQualifiedNames(function->name, call->called)) {
                            // Todo(Sean): Check for Variadic Arguments here eventually
                            bool failed = false;
                            if (call->callArgs.size() == function->paramDecls.size()) {
                                for (std::size_t i = 0; i < call->callArgs.size(); i++) {
                                    auto callArg = call->callArgs[i];
                                    auto paramDecl = function->paramDecls[i];

                                    std::shared_ptr<Type> callArgTy;

                                    if (callArg->getType() == ASTType::Expression) {
                                        callArgTy = std::reinterpret_pointer_cast<ASTExpression>(callArg)->evaluatedType;
                                    } else if (callArg->getType() == ASTType::VariableDeclaration) {
                                        callArgTy = std::reinterpret_pointer_cast<ASTVariableDeclaration>(callArg)->targetTy.evaluatedType;
                                    } else if (callArg->getType() == ASTType::VariableDefinition) {
                                        callArgTy = std::reinterpret_pointer_cast<ASTVariableDefinition>(callArg)->decl->targetTy.evaluatedType;
                                    } else {
                                        errors.emplace_back(core::Error::Type::Semantic, callArg->begin.source, callArg->begin, callArg->end, "unsupported call param");
                                        callArgTy = std::make_shared<Type>(builtinTypes->noneType, 0);
                                    }

                                    if (!compareTypes(callArgTy, paramDecl->targetTy.evaluatedType)) {
                                        errors.emplace_back(core::Error::Type::Semantic, callArg->begin.source, callArg->begin, callArg->end, "argument type mismatch"); // Change this to expected '...', but received '...'
                                        failed = true;
                                    }
                                }
                            } else {
                                failed = true;
                            }

                            if (!failed) {
                                call->called = function;
                                found = true;
                                break;
                            }
                        }
                    }
                }

                if (!found) {
                    throw core::Error(core::Error::Type::Semantic, call->begin.source, call->begin, call->end, fmt::format("no matching declaration to call of function '{}'.", unqualifyName(call->called)));
                }
            } else if (expr->getExprType() == Ty::Literal) {
                auto lit = std::reinterpret_pointer_cast<ASTLiteral>(expr);

                if (lit->literalType == ASTLiteral::Type::Name) {
                    auto node = findQualified(lit); // Find qualified global or local otherwise error.

                    if (!node) {
                        throw core::Error(core::Error::Type::Semantic, lit->begin.source, lit->begin, lit->end, fmt::format("undeclared reference to '{}'.", unqualifyName(lit)));
                    }

                    return node;
                }
            }

            // Come up with a way to validate names like functions variables

            if (!expr->evaluatedType) {
                typer->typeExpression(expr);
            }

            return result;
        }

        void Validator::validateNode(const std::shared_ptr<ASTNode> &node) {
            using Ty = ASTType;
            switch (node->getType()) {
                case Ty::FunctionHeader: {
                    validateFunction(std::reinterpret_pointer_cast<ASTFunctionHeader>(node));
                    break;
                }

                case Ty::Block: {
                    validateBlock(std::reinterpret_pointer_cast<ASTBlock>(node));
                    break;
                }

                case Ty::VariableDeclaration: {
                    validateVariableDeclaration(std::reinterpret_pointer_cast<ASTVariableDeclaration>(node));
                    break;
                }

                case Ty::VariableDefinition: {
                    validateVariableDefinition(std::reinterpret_pointer_cast<ASTVariableDefinition>(node));
                    break;
                }

                case Ty::If: {
                    validateIf(std::reinterpret_pointer_cast<ASTIf>(node));
                    break;
                }

                case Ty::For: {
                    validateFor(std::reinterpret_pointer_cast<ASTFor>(node));
                    break;
                }

                case Ty::While: {
                    validateWhile(std::reinterpret_pointer_cast<ASTWhile>(node));
                    break;
                }

                case Ty::Continue: {
                    validateContinue(std::reinterpret_pointer_cast<ASTContinue>(node));
                    break;
                }

                case Ty::Break: {
                    validateBreak(std::reinterpret_pointer_cast<ASTBreak>(node));
                    break;
                }

                case Ty::Expression: {
                    validateExpression(std::reinterpret_pointer_cast<ASTExpression>(node));
                    break;
                }
            }
        }

        void Validator::validate() {
            for (auto &node : nodes) {
                validateNode(node);
            }
        }
    }
}