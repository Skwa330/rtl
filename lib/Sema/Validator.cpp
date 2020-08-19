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

        std::pair<bool, std::shared_ptr<parser::ASTFunctionHeader>> Validator::isRepeatFunctionDeclaration(const std::shared_ptr<parser::ASTFunctionHeader> &decl) {
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

        void Validator::validateFunction(const std::shared_ptr<ASTFunctionHeader> &function) {
            auto lastFunction = currentFunction;
            currentFunction = function;

            typer->typeFunction(function);

            // Todo(Sean): Check for overloads
            if (std::pair<bool, std::shared_ptr<ASTFunctionHeader>> result; (result = isRepeatFunctionDeclaration(function)).second) {
                errors.emplace_back(core::Error::Type::Semantic, function->begin, function->end, fmt::format("redeclaration of function '{}'; previous declaration occured at: {}:{}:{}.", unqualifyName(function->name), result.second->begin.moduleName, result.second->begin.line, result.second->begin.lexpos));
                return;
            }

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

                    case Ty::If: {
                        // validateIf(std::reinterpret_pointer_cast<ASTIf>(node));
                        break;
                    }

                    case Ty::For: {
                        // validateFor(std::reinterpret_pointer_cast<ASTFor>(node));
                        break;
                    }

                    case Ty::While: {
                        // validateWhile(std::reinterpret_pointer_cast<ASTWhile>(node));
                        break;
                    }

                    case Ty::Continue: {
                        // validateContinue(std::reinterpret_pointer_cast<ASTContinue>(node));
                        break;
                    }

                    case Ty::Break: {
                        // validateBreak(std::reinterpret_pointer_cast<ASTBreak>(node));
                        break;
                    }

                    case Ty::Expression: {
                        validateExpression(std::reinterpret_pointer_cast<ASTExpression>(node));
                        break;
                    }
                }
            }

            currentBlock = lastBlock;
        }

        void Validator::validateVariableDeclaration(const std::shared_ptr<ASTVariableDeclaration> &decl) {
            if (!currentBlock) throw std::runtime_error("INVALID SCOPE IN rtl::sema::Validator::validateVariableDeclaration");

            if (std::pair<bool, std::shared_ptr<ASTVariableDeclaration>> result; (result = isRepeatDeclaration(decl)).second) {
                errors.emplace_back(core::Error::Type::Semantic, decl->begin, decl->end, fmt::format("redeclaration of variable '{}'; previous declaration occurred at: {}:{}:{}.", unqualifyName(decl->name), result.second->begin.moduleName, result.second->begin.line, result.second->begin.lexpos));
            }
        }

        void Validator::validateVariableDefinition(const std::shared_ptr<ASTVariableDefinition> &defn) {
            validateVariableDeclaration(defn->decl);
            validateExpression(std::reinterpret_pointer_cast<ASTExpression>(defn->expr));
        }

        void Validator::validateExpression(std::shared_ptr<ASTExpression> &expr) {
            using Ty = ASTExpression::Type;
            if (expr->getExprType() == Ty::BinaryOperator) {
                if (!expr->evaluatedType) typer->typeExpression(expr);

                auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(expr);
                validateExpression(binop->left);
                validateExpression(binop->right);

                if (compareTypes(std::reinterpret_pointer_cast<ASTExpression>(binop->left)->evaluatedType, std::reinterpret_pointer_cast<ASTExpression>(binop->right)->evaluatedType)) {
                    errors.emplace_back(core::Error::Type::Semantic, binop->begin, binop->end, "cannot implicitly convert types between left and right expressions."); // Todo(Sean): Make this error message show the actual name of the type.
                }
            } else if (expr->getExprType() == Ty::Call) {
                auto call = std::reinterpret_pointer_cast<ASTCall>(expr);

                bool found = false;

                for (auto &arg : call->callArgs) {
                    validateExpression(std::reinterpret_pointer_cast<ASTExpression>(arg));
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
                                    auto callArg = std::reinterpret_pointer_cast<ASTExpression>(call->callArgs[i]);
                                    auto paramDecl = function->paramDecls[i];

                                    if (!compareTypes(callArg->evaluatedType, paramDecl->targetTy.evaluatedType)) {
                                        errors.emplace_back(core::Error::Type::Semantic, callArg->begin, callArg->end, "argument type mismatch"); // Change this to expected '...', but received '...'
                                        fmt::print("failed.\n");
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
                    throw core::Error(core::Error::Type::Semantic, call->begin, call->end, fmt::format("no matching declaration to call of function '{}'.", unqualifyName(call->called)));
                }
            }

            // Come up with a way to validate names like functions variables

            if (!expr->evaluatedType) {
                typer->typeExpression(expr);
            }
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