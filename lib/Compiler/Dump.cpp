#include "Dump.h"

#include <fmt/format.h>

// Right now every { and } get their own line because that's the easiest way to dump it LMAO... I'm not lazy... you are.

namespace rtl {
    namespace compiler {
        std::string escapeString(const std::string_view &s) {
            std::string result;

            for (char c : s) {
                switch (c) {
                    case '\r': {
                        result += "\\r";
                        break;
                    }

                    case '\f': {
                        result += "\\f";
                        break;
                    }

                    case '\t': {
                        result += "\\t";
                        break;
                    }

                    case '\v': {
                        result += "\\v";
                        break;
                    }

                    case '\n': {
                        result += "\\n";
                        break;
                    }

                    default: {
                        result += c;
                        break;
                    }
                }
            }

            return result;
        }

        using namespace rtl::parser;

        std::string dumpType(const Type &ty) {
            std::string result;

            for (std::size_t i = 0; i < ty.pointer; i++) {
                result += "^";
            }

            if (ty.baseType->getType() == ASTType::BuiltinType) {
                auto builtinType = std::reinterpret_pointer_cast<ASTBuiltinType>(ty.baseType);

                using Bt = ASTBuiltinType::Type;

                switch (builtinType->builtinType) {
                    case Bt::Auto: {
                        result += "auto";
                        break;
                    }

                    case Bt::None: {
                        result += "none";
                        break;
                    }

                    case Bt::Bool: {
                        result += "bool";
                        break;
                    }

                    case Bt::I8: {
                        result += "i8";
                        break;
                    }

                    case Bt::I16: {
                        result += "i16";
                        break;
                    }

                    case Bt::I32: {
                        result += "i32";
                        break;
                    }

                    case Bt::I64: {
                        result += "i64";
                        break;
                    }

                    case Bt::U8: {
                        result += "u8";
                        break;
                    }

                    case Bt::U16: {
                        result += "u16";
                        break;
                    }

                    case Bt::U32: {
                        result += "u32";
                        break;
                    }

                    case Bt::U64: {
                        result += "u64";
                        break;
                    }

                    case Bt::F32: {
                        result += "f32";
                        break;
                    }

                    case Bt::F64: {
                        result += "f64";
                        break;
                    }

                    case Bt::FunctionPrototype: {
                        result += "(";

                        bool first = true;
                        for (auto &pty : builtinType->fpData.paramTypes) {
                            if (first) first = false;
                            else result += ", ";

                            result += dumpType(pty);
                        }

                        result += ") -> ";
                        result += dumpType(builtinType->fpData.rt);
                    }
                }
            } else if (ty.baseType->getType() == ASTType::Expression) {
                result += dumpNode(ty.baseType);
            } else {
                result += "$UNKNOWN";
            }

            return result;
        }

        std::string dumpNode(const std::shared_ptr<ASTNode> &node, std::size_t ind) {
            std::string result;

            for (std::size_t i = 0; i < ind; i++) result += "    ";
            if (!node) return result;

            if (node->getType() == ASTType::Return) {
                auto returnStatement = std::reinterpret_pointer_cast<ASTReturn>(node);

                result += fmt::format("return {}", dumpNode(returnStatement->expr));
            } else if (node->getType() == ASTType::Range) {
                auto range = std::reinterpret_pointer_cast<ASTRange>(node);

                result += fmt::format("{}..{}", dumpNode(range->lower), dumpNode(range->upper));
            } else if (node->getType() == ASTType::For) {
                auto forStatement = std::reinterpret_pointer_cast<ASTFor>(node);

                result += fmt::format("for {}\n", dumpNode(forStatement->expr));
                if (forStatement->statement->getType() == ASTType::Block) {
                    result += dumpNode(forStatement->statement, ind);
                } else {
                    result += dumpNode(forStatement->statement, ind + 1);
                }
            } else if (node->getType() == ASTType::While) {
                auto whileStatement = std::reinterpret_pointer_cast<ASTWhile>(node);

                result += fmt::format("while {}\n", dumpNode(whileStatement->condition));
                if (whileStatement->statement->getType() == ASTType::Block) {
                    result += dumpNode(whileStatement->statement, ind);
                } else {
                    result += dumpNode(whileStatement->statement, ind + 1);
                }
            } else if (node->getType() == ASTType::If) {
                auto ifStatement = std::reinterpret_pointer_cast<ASTIf>(node);

                result += fmt::format("if {}\n", dumpNode(ifStatement->condition));
                if (ifStatement->statement->getType() == ASTType::Block) {
                    result += dumpNode(ifStatement->statement, ind);
                } else {
                    result += dumpNode(ifStatement->statement, ind + 1);
                }

                for (auto &elif : ifStatement->elifs) {
                    result += "\n";
                    for (std::size_t i = 0; i < ind; i++) result += "    ";
                    result += fmt::format("elif {}\n", dumpNode(elif.first));
                    if (elif.second->getType() == ASTType::Block) {
                        result += dumpNode(elif.second, ind);
                    } else {
                        result += dumpNode(elif.second, ind + 1);
                    }
                }

                if (ifStatement->elseStatement) {
                    result += " else\n";
                    if (ifStatement->elseStatement->getType() == ASTType::Block) {
                        result += dumpNode(ifStatement->elseStatement, ind);
                    } else {
                        result += dumpNode(ifStatement->elseStatement, ind + 1);
                    }
                }
            } else if (node->getType() == ASTType::Block) {
                auto block = std::reinterpret_pointer_cast<ASTBlock>(node);

                result += "{\n";
                for (auto &node : block->nodes) {
                    result += dumpNode(node, ind + 1) + "\n";
                }

                for (std::size_t i = 0; i < ind; i++) {
                    result += "    ";
                }

                result += "}";
            } else if (node->getType() == ASTType::VariableDeclaration) {
                auto decl = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);

                result += fmt::format("{} {}: {}", decl->flags & (std::uint32_t)ASTVariableDeclaration::Flags::Constant ? "val" : "var", dumpNode(decl->name), dumpType(decl->targetTy));
            } else if (node->getType() == ASTType::VariableDefinition) {
                auto defn = std::reinterpret_pointer_cast<ASTVariableDefinition>(node);

                result += fmt::format("{} = {}", dumpNode(defn->decl), dumpNode(defn->expr));
            } else if (node->getType() == ASTType::FunctionHeader) {
                auto header = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);

                if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Public) {
                    result += "pub ";
                }

                result += "fun ";
                result += dumpNode(header->name);
                result += " (";

                bool first = true;
                for (auto &decl : header->paramDecls) {
                    if (first) {
                        first = false;
                    } else {
                        result += ", ";
                    }

                    result += dumpNode(decl);
                }

                result += ") ";

                if (header->flags) {
                    result += "[";

                    if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) {
                        result += "$foreign";
                    }

                    if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Extern) {
                        if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) result += " ";
                        result += "$extern";
                    }

                    if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::CCall) {
                        if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Extern || header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) result += " ";
                        result += "$ccall";
                    }

                    if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::FastCall) {
                        if (header->flags & (std::uint32_t)ASTFunctionHeader::Flags::CCall || header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Extern || header->flags & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) result += " ";
                        result += "$fastcall";
                    }

                    result += "] ";
                }

                result += fmt::format("-> {}", dumpType(header->rt));

                if (header->body) {
                    result += fmt::format("\n{}", dumpNode(header->body->block));
                }
            } else if (node->getType() == ASTType::FunctionBody) {
                auto body = std::reinterpret_pointer_cast<ASTFunctionBody>(node);

                result += dumpNode(body->block, ind);
            } else if (node->getType() == ASTType::Expression) {
                auto expr = std::reinterpret_pointer_cast<ASTExpression>(node);

                if (expr->getExprType() == ASTExpression::Type::Call) {
                    auto call = std::reinterpret_pointer_cast<ASTCall>(node);

                    result += dumpNode(call->called);
                    result += "(";
                    bool first = true;
                    for (auto &node : call->callArgs) {
                        if (first) {
                            first = false;
                        } else {
                            result += ", ";
                        }

                        result += dumpNode(node);
                    }
                    result += ")";
                } else if (expr->getExprType() == ASTExpression::Type::Subscript) {
                    auto sub = std::reinterpret_pointer_cast<ASTSubscript>(node);

                    result += fmt::format("{}[{}]", dumpNode(sub->indexed), dumpNode(sub->index));
                } else if (expr->getExprType() == ASTExpression::Type::Literal) {
                    auto literal = std::reinterpret_pointer_cast<ASTLiteral>(node);

                    switch (literal->literalType) {
                        case ASTLiteral::Type::Integer: {
                            result += fmt::format("{}", literal->getInteger());
                            break;
                        }

                        case ASTLiteral::Type::Decimal: {
                            result += fmt::format("{}", literal->getDecimal());
                            break;
                        }

                        case ASTLiteral::Type::Name: {
                            result += fmt::format("{}", literal->getString());
                            break;
                        }

                        case ASTLiteral::Type::String: {
                            result += fmt::format("\"{}\"", escapeString(literal->getString()));
                            break;
                        }

                        case ASTLiteral::Type::Character: {
                            result += fmt::format("'{}'", literal->getString());
                            break;
                        }

                        case ASTLiteral::Type::Bool: {
                            result += fmt::format("{}", literal->getBool());
                            break;
                        }
                    }
                } else if (expr->getExprType() == ASTExpression::Type::Conversion) {
                    auto conversion = std::reinterpret_pointer_cast<ASTConversion>(node);

                    result += "(";
                    result += dumpNode(conversion->from);
                    result += " as ";

                    for (std::size_t i = 0; i < conversion->to.pointer; i++) {
                        result += "^";
                    }

                    result += fmt::format("{})", dumpType(conversion->to));
                } else if (expr->getExprType() == ASTExpression::Type::UnaryOperator) {
                    auto unop = std::reinterpret_pointer_cast<ASTUnaryOperator>(node);

                    const char *opname;

                    switch (unop->unopType) {
                        case ASTUnaryOperator::Type::LogicalNot: {
                            opname = "!";
                            break;
                        }

                        case ASTUnaryOperator::Type::BitNot: {
                            opname = "~";
                            break;
                        }

                        case ASTUnaryOperator::Type::Minus: {
                            opname = "-";
                            break;
                        }

                        case ASTUnaryOperator::Type::Dereference: {
                            opname = "*";
                            break;
                        }

                        case ASTUnaryOperator::Type::AddressOf: {
                            opname = "^";
                            break;
                        }
                    }

                    result += fmt::format("{}{}", opname, dumpNode(unop->node));
                } else if (expr->getExprType() == ASTExpression::Type::BinaryOperator) {
                    auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(node);

                    const char *opname;

                    switch (binop->binopType) {
                        case ASTBinaryOperator::Type::Add: {
                            opname = "+";
                            break;
                        }

                        case ASTBinaryOperator::Type::Subtract: {
                            opname = "-";
                            break;
                        }

                        case ASTBinaryOperator::Type::Modulo: {
                            opname = "%";
                            break;
                        }

                        case ASTBinaryOperator::Type::Multiply: {
                            opname = "*";
                            break;
                        }

                        case ASTBinaryOperator::Type::Divide: {
                            opname = "/";
                            break;
                        }

                        case ASTBinaryOperator::Type::BitShiftLeft: {
                            opname = "<<";
                            break;
                        }

                        case ASTBinaryOperator::Type::BitShiftRight: {
                            opname = ">>";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalLessThan: {
                            opname = "<";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalLessThanEqual: {
                            opname = "<=";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalGreaterThan: {
                            opname = ">";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalGreaterThanEqual: {
                            opname = ">=";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalEqual: {
                            opname = "==";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalNotEqual: {
                            opname = "!=";
                            break;
                        }

                        case ASTBinaryOperator::Type::BitAnd: {
                            opname = "&";
                            break;
                        }

                        case ASTBinaryOperator::Type::BitXor: {
                            opname = "^";
                            break;
                        }

                        case ASTBinaryOperator::Type::BitOr: {
                            opname = "|";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalAnd: {
                            opname = "&&";
                            break;
                        }

                        case ASTBinaryOperator::Type::LogicalOr: {
                            opname = "||";
                            break;
                        }

                        case ASTBinaryOperator::Type::NamespaceResolution: {
                            opname = "::";
                            break;
                        }

                        case ASTBinaryOperator::Type::MemberResolution: {
                            opname = ".";
                            break;
                        }

                        case ASTBinaryOperator::Type::Assign: {
                            opname = "=";
                            break;
                        }
                    }

                    if (binop->binopType != ASTBinaryOperator::Type::NamespaceResolution && binop->binopType != ASTBinaryOperator::Type::MemberResolution) result += "(";
                    result += dumpNode(binop->left);
                    if (binop->binopType != ASTBinaryOperator::Type::NamespaceResolution && binop->binopType != ASTBinaryOperator::Type::MemberResolution) result += fmt::format(" {} ", opname);
                    else result += opname;
                    result += dumpNode(binop->right);
                    if (binop->binopType != ASTBinaryOperator::Type::NamespaceResolution && binop->binopType != ASTBinaryOperator::Type::MemberResolution) result += ")";
                }
            }

            return result;
        }
    }
}