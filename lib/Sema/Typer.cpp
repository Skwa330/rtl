#include "rtl/Sema/Typer.h"

#include <fmt/format.h>

using namespace rtl::parser; // So that we don't have to type parser::AST*

namespace rtl {
    namespace sema {
        Typer::Typer(const std::shared_ptr<BuiltinTypes> &builtinTypes, std::vector<std::shared_ptr<ASTNode>> &nodes, std::vector<core::Error> &errors) : builtinTypes(builtinTypes), nodes(nodes), errors(errors) {
        }

        std::shared_ptr<TypeDeclaration> Typer::getTypeDeclaration(const std::shared_ptr<ASTNode> &typeIdentifier) {
            if (typeIdentifier->getType() != ASTType::BuiltinType) {
                throw std::domain_error(fmt::format("{}:{}:{}: custom types are not yet supported :(", typeIdentifier->begin.moduleName, typeIdentifier->begin.line, typeIdentifier->begin.lexpos));
            }

            auto builtin = std::reinterpret_pointer_cast<ASTBuiltinType>(typeIdentifier);

            using Ty = ASTBuiltinType::Type;

            switch (builtin->builtinType) {
                case Ty::None: {
                    return builtinTypes->noneType;
                }

                case Ty::Bool: {
                    return builtinTypes->boolType;
                }

                case Ty::I8: {
                    return builtinTypes->i8Type;
                }

                case Ty::I16: {
                    return builtinTypes->i16Type;
                }

                case Ty::I32: {
                    return builtinTypes->i32Type;
                }

                case Ty::I64: {
                    return builtinTypes->i64Type;
                }

                case Ty::U8: {
                    return builtinTypes->u8Type;
                }

                case Ty::U16: {
                    return builtinTypes->u16Type;
                }

                case Ty::U32: {
                    return builtinTypes->u32Type;
                }

                case Ty::U64: {
                    return builtinTypes->u64Type;
                }

                case Ty::F32: {
                    return builtinTypes->f32Type;
                }

                case Ty::F64: {
                    return builtinTypes->f64Type;
                }

                case Ty::FunctionPrototype: {
                    auto decl = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::FunctionPrototype);

                    typeType(builtin->fpData.rt);
                    std::get<FunctionPrototype>(decl->info).rt = builtin->fpData.rt.evaluatedType;

                    std::get<FunctionPrototype>(decl->info).paramTypes.reserve(builtin->fpData.paramTypes.size());

                    for (auto &pty : builtin->fpData.paramTypes) {
                        typeType(pty);
                        std::get<FunctionPrototype>(decl->info).paramTypes.push_back(pty.evaluatedType);
                    }

                    return decl;
                }
            }

            return {};
        }

        std::shared_ptr<Type> Typer::mapType(const parser::Type &type) {
            auto decl = getTypeDeclaration(type.baseType);
            return std::make_shared<Type>(decl, type.pointer);
        }

        void Typer::typeFunction(const std::shared_ptr<ASTFunctionHeader> &function) {
            auto lastFunction = currentFunction;
            currentFunction = function;

            for (auto &paramDecl : function->paramDecls) {
                typeVariableDeclaration(paramDecl);
            }

            function->rt.evaluatedType = mapType(function->rt);

            // Create prototype
            function->prototype = std::make_shared<Type>(std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::FunctionPrototype), 0);

            std::get<FunctionPrototype>(function->prototype->decl->info).rt = function->rt.evaluatedType;

            std::get<FunctionPrototype>(function->prototype->decl->info).paramTypes.reserve(function->paramDecls.size());

            for (auto &pd : function->paramDecls) {
                std::get<FunctionPrototype>(function->prototype->decl->info).paramTypes.push_back(pd->targetTy.evaluatedType);
            }

            currentFunction = lastFunction;
        }

        void Typer::typeVariableDeclaration(const std::shared_ptr<ASTVariableDeclaration> &decl) {
            decl->targetTy.evaluatedType = mapType(decl->targetTy);
        }

        void Typer::typeVariableDefinition(const std::shared_ptr<ASTVariableDefinition> &defn) {
            typeVariableDeclaration(defn->decl);
            typeExpression(std::reinterpret_pointer_cast<ASTExpression>(defn->expr));

            if (defn->decl->targetTy.baseType->getType() == ASTType::BuiltinType && std::reinterpret_pointer_cast<ASTBuiltinType>(defn->decl->targetTy.baseType)->builtinType == ASTBuiltinType::Type::Auto) {
                defn->decl->targetTy.evaluatedType = std::reinterpret_pointer_cast<ASTExpression>(defn->expr)->evaluatedType;
            }

            if (!defn->decl->targetTy.evaluatedType->decl) {
                if (defn->expr->getType() != ASTType::Expression) {
                    throw std::runtime_error("Invalid expression in variable definition");
                }

                defn->decl->targetTy.evaluatedType->decl = (std::reinterpret_pointer_cast<ASTExpression>(defn->expr))->evaluatedType->decl;
            }
        }

        void Typer::typeExpression(const std::shared_ptr<ASTExpression> &expr) {
            using Ty = ASTExpression::Type;

            if (expr->evaluatedType) return;

            switch (expr->getExprType()) {
                case Ty::Literal: {
                    auto literal = std::reinterpret_pointer_cast<ASTLiteral>(expr);

                    switch (literal->literalType) {
                        case ASTLiteral::Type::Integer: {
                            if (literal->getInteger() > 0xffffffff) {
                                literal->evaluatedType = std::make_shared<Type>(builtinTypes->i64Type, 0);
                            } else {
                                literal->evaluatedType = std::make_shared<Type>(builtinTypes->i32Type, 0);
                            }

                            break;
                        }

                        case ASTLiteral::Type::Decimal: {
                            literal->evaluatedType = std::make_shared<Type>(builtinTypes->f32Type, 0);
                            break;
                        }

                        case ASTLiteral::Type::String: {
                            literal->evaluatedType = std::make_shared<Type>(builtinTypes->u8Type, 1);
                            break;
                        }

                        case ASTLiteral::Type::Character: {
                            literal->evaluatedType = std::make_shared<Type>(builtinTypes->u8Type, 0);
                            break;
                        }

                        case ASTLiteral::Type::Bool: {
                            literal->evaluatedType = std::make_shared<Type>(builtinTypes->boolType, 0);
                            break;
                        }
                    }

                    break;
                }

                case Ty::Conversion: {
                    auto conversion = std::reinterpret_pointer_cast<ASTConversion>(expr);

                    typeExpression(std::reinterpret_pointer_cast<ASTExpression>(conversion->from));
                    typeType(conversion->to);
                    conversion->evaluatedType = conversion->to.evaluatedType;

                    break;
                }

                case Ty::UnaryOperator: {
                    auto unop = std::reinterpret_pointer_cast<ASTUnaryOperator>(expr);

                    typeExpression(std::reinterpret_pointer_cast<ASTExpression>(unop->node));

                    unop->evaluatedType = (std::reinterpret_pointer_cast<ASTExpression>(unop->node))->evaluatedType;
                    break;
                }

                case Ty::BinaryOperator: {
                    auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(expr);

                    typeExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->left));
                    typeExpression(std::reinterpret_pointer_cast<ASTExpression>(binop->right));

                    binop->evaluatedType = (std::reinterpret_pointer_cast<ASTExpression>(binop->right))->evaluatedType;
                    break;
                }

                case Ty::Call: {
                    auto call = std::reinterpret_pointer_cast<ASTCall>(expr);
                    if (call->called->getType() == ASTType::FunctionHeader) {
                        call->evaluatedType = (std::reinterpret_pointer_cast<ASTFunctionHeader>(call->called))->rt.evaluatedType;
                    } else if (call->called->getType() == ASTType::VariableDeclaration) {
                        call->evaluatedType = std::get<FunctionPrototype>((std::reinterpret_pointer_cast<ASTVariableDeclaration>(call->called)->targetTy.evaluatedType->decl->info)).rt;
                    } else {
                        #ifndef STRINGIFY
                        #define STRINGIFY(x) STR(x)
                        #define STR(x) #x
                        #endif

                        throw std::domain_error("unimplemented callable in Ty::Call at " __FILE__ ":" STRINGIFY(__LINE__));
                    }

                    break;
                }
            }
        }

        void Typer::typeType(parser::Type &type) {
            type.evaluatedType = mapType(type);
        }

        void Typer::typeNode(const std::shared_ptr<ASTNode> &node) {
            // Todo(Sean): Check if nodes are typed so that we aren't constantly re-typing them.
            auto type = node->getType();

            if (node->getType() == ASTType::FunctionHeader) {
                typeFunction(std::reinterpret_pointer_cast<ASTFunctionHeader>(node));
            } else if (node->getType() == ASTType::Expression) {
                typeExpression(std::reinterpret_pointer_cast<ASTExpression>(node));
            } else if (node->getType() == ASTType::VariableDeclaration) {
                typeVariableDeclaration(std::reinterpret_pointer_cast<ASTVariableDeclaration>(node));
            } else if (node->getType() == ASTType::VariableDefinition) {
                typeVariableDefinition(std::reinterpret_pointer_cast<ASTVariableDefinition>(node));
            } else if (node->getType() == ASTType::Return) {
                auto returnStatement = std::reinterpret_pointer_cast<ASTReturn>(node);
                typeExpression(std::reinterpret_pointer_cast<ASTExpression>(returnStatement->expr));
            } else {
                throw std::runtime_error("Unhandled typeNode call");
            }
        }
    }
}