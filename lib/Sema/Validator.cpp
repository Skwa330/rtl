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

            auto tag = left->decl->getTag();

            if (tag == TypeDeclaration::Tag::FunctionPrototype) {
                FunctionPrototype &lpr = std::get<FunctionPrototype>(left->decl->info);
                FunctionPrototype &rpr = std::get<FunctionPrototype>(right->decl->info);

                if (!compareTypes(lpr.rt, rpr.rt)) {
                    return false;
                }

                if (lpr.paramTypes.size() != rpr.paramTypes.size()) {
                    return false;
                }

                auto npt = lpr.paramTypes.size();

                for (std::size_t i = 0; i < npt; i++) {
                    if (!compareTypes(lpr.paramTypes[i], rpr.paramTypes[i])) {
                        return false;
                    }
                }
            }

            return true;
        }

        std::shared_ptr<parser::ASTRef> Validator::findQualified(const std::shared_ptr<ASTNode> &qlf) {
            if (currentFunction) {
                for (auto &pd : currentFunction->paramDecls) {
                    if (compareQualifiedNames(pd->name, qlf)) {
                        auto result = std::make_shared<ASTRef>(pd);
                        result->evaluatedType = pd->targetTy.evaluatedType;
                        return result;
                    }
                }
            }

            if (currentBlock) {
                // Try to find variable in local scope

                for (auto &node : currentBlock->nodes) {
                    if (node == currentStatement) break;

                    if (node->getType() == ASTType::VariableDeclaration) {
                        auto decl = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);
                        auto name = decl->name;

                        if (compareQualifiedNames(name, qlf)) {
                            auto result = std::make_shared<ASTRef>(node);
                            result->evaluatedType = decl->targetTy.evaluatedType;
                            return result;
                        }
                    } else if (node->getType() == ASTType::VariableDefinition) {
                        auto defn = std::reinterpret_pointer_cast<ASTVariableDefinition>(node);
                        auto decl = defn->decl;
                        auto name = decl->name;

                        if (compareQualifiedNames(name, qlf)) {
                            auto result = std::make_shared<ASTRef>(node);
                            result->evaluatedType = decl->targetTy.evaluatedType;
                            return result;
                        }
                    }
                }

                if (currentBlock->parent) {
                    auto last = currentBlock;
                    currentBlock = currentBlock->parent;
                    auto lastStatement = currentStatement;
                    currentStatement = currentBlock;
                    auto result = findQualified(qlf);
                    currentStatement = lastStatement;
                    currentBlock = last;
                    return result;
                }
            }

            for (auto &node : nodes) {
                if (node->getType() == ASTType::FunctionHeader) {
                    auto function = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);
                    auto name = function->name;

                    // We don't have global variables yet, but we will; I don't want to add this later...
                    if (compareQualifiedNames(name, qlf)) {
                        auto result = std::make_shared<ASTRef>(node);
                        result->evaluatedType = function->prototype;
                        return result;
                    }
                } else if (node->getType() == ASTType::VariableDeclaration) {
                    auto decl = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);
                    auto name = decl->name;

                    if (compareQualifiedNames(name, qlf)) {
                        auto result = std::make_shared<ASTRef>(node);
                        result->evaluatedType = decl->targetTy.evaluatedType;
                        return result;
                    }
                } else if (node->getType() == ASTType::VariableDefinition) {
                    auto defn = std::reinterpret_pointer_cast<ASTVariableDefinition>(node);
                    auto decl = defn->decl;
                    auto name = decl->name;

                    if (compareQualifiedNames(name, qlf)) {
                        auto result = std::make_shared<ASTRef>(node);
                        result->evaluatedType = decl->targetTy.evaluatedType;
                        return result;
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
                currentStatement = node;
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

            if (!defn->decl->targetTy.evaluatedType || (defn->decl->targetTy.baseType->getType() == ASTType::BuiltinType && std::reinterpret_pointer_cast<ASTBuiltinType>(defn->decl->targetTy.baseType)->builtinType == ASTBuiltinType::Type::Auto)) {
                defn->decl->targetTy.evaluatedType = std::reinterpret_pointer_cast<ASTExpression>(defn->expr)->evaluatedType;
            }

            if (!compareTypes(defn->decl->targetTy.evaluatedType, std::reinterpret_pointer_cast<ASTExpression>(defn->expr)->evaluatedType)) {
                errors.emplace_back(core::Error::Type::Semantic, defn->begin.source, defn->begin, defn->end, "assigned value doesn't match type of l-value.");
            }
        }

        void Validator::validateIf(const std::shared_ptr<ASTIf> &ifStatement) {
            ifStatement->condition = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(ifStatement->condition));
            validateNode(ifStatement->statement);
        }

        void Validator::validateFor(const std::shared_ptr<ASTFor> &forStatement) {
            validateRange(std::reinterpret_pointer_cast<ASTRange>(forStatement->expr));
            validateNode(forStatement->statement);
        }

        void Validator::validateRange(const std::shared_ptr<ASTRange> &range) {
            range->lower = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(range->lower));
            range->upper = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(range->upper));

            auto isRangable = [](const std::shared_ptr<Type> &ty) {
                return ty->decl->getTag() != TypeDeclaration::Tag::Bool && ty->decl->getTag() != TypeDeclaration::Tag::Structure && ty->decl->getTag() != TypeDeclaration::Tag::Union &&  ty->decl->getTag() != TypeDeclaration::Tag::Enumeration;
            };

            if (!isRangable(std::reinterpret_pointer_cast<ASTExpression>(range->lower)->evaluatedType)) {
                errors.emplace_back(core::Error::Type::Semantic, range->lower->begin.source, range->lower->begin, range->lower->end, "value is not of rangable type.");
            }

            if (!isRangable(std::reinterpret_pointer_cast<ASTExpression>(range->upper)->evaluatedType)) {
                errors.emplace_back(core::Error::Type::Semantic, range->upper->begin.source, range->upper->begin, range->upper->end, "value is not of rangable type.");
            }
        }

        void Validator::validateWhile(const std::shared_ptr<ASTWhile> &whileStatement) {
            whileStatement->condition = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(whileStatement->condition));
            validateNode(whileStatement->statement);
        }

        void Validator::validateContinue(const std::shared_ptr<ASTContinue> &continueStatement) {

        }

        void Validator::validateBreak(const std::shared_ptr<ASTBreak> &breakStatement) {

        }

        void Validator::validateReturn(const std::shared_ptr<ASTReturn> &returnStatement) {
            returnStatement->expr = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(returnStatement->expr));
        }

        std::shared_ptr<ASTNode> Validator::validateExpression(const std::shared_ptr<ASTExpression> &expr) {
            auto result = expr;

            using Ty = ASTExpression::Type;
            if (expr->getExprType() == Ty::BinaryOperator) {
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
                    binop->left = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->left));
                    binop->right = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->right));
                }

                if (!expr->evaluatedType) typer->typeExpression(expr);

                if (binop->binopType == ASTBinaryOperator::Type::Assign) {
                    auto lhs = binop->left;
                    if (lhs->getType() == ASTType::Expression) {
                        auto expr = std::reinterpret_pointer_cast<ASTExpression>(lhs);

                        if (expr->getExprType() == ASTExpression::Type::Ref) {
                            auto ref = std::reinterpret_pointer_cast<ASTRef>(expr);
                            auto node = ref->node;

                            if (node->getType() == ASTType::VariableDeclaration && (std::reinterpret_pointer_cast<ASTVariableDeclaration>(node)->flags & (std::uint32_t)ASTVariableDeclaration::Flags::Constant)) {
                                errors.emplace_back(core::Error::Type::Semantic, binop->begin.source, binop->begin, binop->end, "cannot assign to constant data.");
                            } else if (node->getType() == ASTType::VariableDefinition && (std::reinterpret_pointer_cast<ASTVariableDefinition>(node)->decl->flags & (std::uint32_t)ASTVariableDeclaration::Flags::Constant)) {
                                errors.emplace_back(core::Error::Type::Semantic, binop->begin.source, binop->begin, binop->end, "cannot assign to constant data.");
                            } else {
                                errors.emplace_back(core::Error::Type::Semantic, node->begin.source, node->begin, node->end, "left-hand operand must be a valid l-value.");
                            }
                        } else {
                            errors.emplace_back(core::Error::Type::Semantic, lhs->begin.source, lhs->begin, lhs->end, "left-hand operand must be a valid l-value.");
                        }
                    } else {
                        errors.emplace_back(core::Error::Type::Semantic, lhs->begin.source, lhs->begin, lhs->end, "left-hand operand must be a valid l-value.");
                    }

                }

                if (!compareTypes(std::reinterpret_pointer_cast<ASTExpression>(binop->left)->evaluatedType, std::reinterpret_pointer_cast<ASTExpression>(binop->right)->evaluatedType)) {
                    errors.emplace_back(core::Error::Type::Semantic, binop->begin.source, binop->begin, binop->end, "cannot implicitly convert types between left and right expressions."); // Todo(Sean): Make this error message show the actual name of the type.
                }
            } else if (expr->getExprType() == Ty::Call) {
                auto call = std::reinterpret_pointer_cast<ASTCall>(expr);

                bool found = false;

                auto compareNode = [&](const std::shared_ptr<ASTNode> &node) {
                    if (node->getType() == ASTType::FunctionHeader) {
                        auto function = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);

                        if (!function->prototype) {
                            validateFunction(function);
                        }

                        if (compareQualifiedNames(function->name, call->called)) {
                            // Todo(Sean): Check for Variadic Arguments here eventually
                            bool failed = false;
                            if (call->callArgs.size() == function->paramDecls.size()) {

                                for (std::size_t i = 0; i < call->callArgs.size(); i++) {
                                    auto callArg = call->callArgs[i] = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(call->callArgs[i]));
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
                                        failed = true;
                                        break;
                                    }
                                }
                            } else {
                                failed = true;
                            }

                            if (!failed) {
                                call->called = function;
                                found = true;
                            }
                        }
                    } else if (node->getType() == ASTType::VariableDeclaration || node->getType() == ASTType::VariableDefinition) {
                        std::shared_ptr<ASTVariableDeclaration> decl;

                        if (node->getType() == ASTType::VariableDefinition) {
                            decl = std::reinterpret_pointer_cast<ASTVariableDefinition>(node)->decl;
                        } else {
                            decl = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);
                        }

                        if (compareQualifiedNames(decl->name, call->called)) {
                            if (!decl->targetTy.evaluatedType) validateNode(decl);
                            call->called = decl;

                            if (decl->targetTy.evaluatedType->decl->getTag() != TypeDeclaration::Tag::FunctionPrototype) {
                                throw core::Error(core::Error::Type::Semantic, call->begin.source, call->begin, call->end, fmt::format("attempt to call uncallable: '{}'.", unqualifyName(call->called)));
                            }

                            bool valargs = false;

                            if (call->callArgs.size() != std::get<FunctionPrototype>(decl->targetTy.evaluatedType->decl->info).paramTypes.size()) {
                                errors.emplace_back(core::Error::Type::Semantic, call->begin.source, call->begin, call->end, fmt::format("function prototype: '{}' expects {} argument{}, but was given {}.", unqualifyName(std::reinterpret_pointer_cast<ASTVariableDeclaration>(call->called)->name), std::get<FunctionPrototype>(decl->targetTy.evaluatedType->decl->info).paramTypes.size(), std::get<FunctionPrototype>(decl->targetTy.evaluatedType->decl->info).paramTypes.size() > 1 ? "s" : "", call->callArgs.size()));

                                valargs = true; // We validate the arguments here because there might be more arguments in the call than there are in the prototype declaration.

                                for (auto &arg : call->callArgs) {
                                    arg = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(arg));
                                }
                            }

                            auto count = std::min(call->callArgs.size(), std::get<FunctionPrototype>(decl->targetTy.evaluatedType->decl->info).paramTypes.size());
                            for (std::size_t i = 0; i < count; i++) {
                                if (!valargs) call->callArgs[i] = validateExpression(std::reinterpret_pointer_cast<ASTExpression>(call->callArgs[i]));
                                auto callArg = call->callArgs[i];
                                auto paramType = std::get<FunctionPrototype>(decl->targetTy.evaluatedType->decl->info).paramTypes[i];

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

                                if (!compareTypes(callArgTy, paramType)) {
                                    errors.emplace_back(core::Error::Type::Semantic, callArg->begin.source, callArg->begin, callArg->end, "argument type mismatch in function prototype invokation.");
                                }
                            }

                            found = true;
                        }
                    }

                    return found;
                };

                if (currentFunction) {
                    for (auto &pd : currentFunction->paramDecls) {
                        if (compareNode(pd)) break;
                    }
                }

                if (!found && currentBlock) {
                    auto lastStatement = currentStatement;
                    for (auto block = currentBlock; block; block = currentBlock->parent) {
                        for (auto &node : block->nodes) {
                            currentStatement = node;
                            if (compareNode(node)) break;
                        }
                    }
                    currentStatement = lastStatement;
                }

                if (!found) {
                    for (auto &node : nodes) {
                        if (compareNode(node)) break;
                    }
                }

                if (!found) {
                    std::shared_ptr<ASTNode> name;

                    if (call->called->getType() == ASTType::FunctionHeader) {
                        name = std::reinterpret_pointer_cast<ASTFunctionHeader>(call->called)->name;
                    } else if (call->called->getType() == ASTType::VariableDeclaration) {
                        name = std::reinterpret_pointer_cast<ASTFunctionHeader>(call->called)->name;
                    } else if (call->called->getType() == ASTType::Expression) {
                        auto expr = std::reinterpret_pointer_cast<ASTExpression>(call->called);
                        if (expr->getExprType() == ASTExpression::Type::Literal) {
                            name = std::reinterpret_pointer_cast<ASTLiteral>(expr);
                        }
                    }

                    throw core::Error(core::Error::Type::Semantic, call->begin.source, call->begin, call->end, fmt::format("no matching declaration to call of '{}'.", unqualifyName(name)));
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

                case Ty::Range: {
                    validateRange(std::reinterpret_pointer_cast<ASTRange>(node));
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

                case Ty::Return: {
                    validateReturn(std::reinterpret_pointer_cast<ASTReturn>(node));
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