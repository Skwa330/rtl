#include "Dump.h"

#include <fmt/format.h>

namespace rlt {
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

        void dumpNode(const std::shared_ptr<rlt::parser::ASTNode>& node) {
            if (!node) return;

            std::putchar('(');

            if (node->getType() == rlt::parser::ASTType::Call) {
                auto call = std::reinterpret_pointer_cast<rlt::parser::ASTCall>(node);

                dumpNode(call->getCalled());
                std::putchar('(');
                bool first = true;
                for (auto &node : call->getCallArgs()) {
                    if (first) {
                        first = false;
                    } else {
                        fmt::print(", ");
                    }

                    dumpNode(node);
                }
                std::putchar(')');
            } else if (node->getType() == rlt::parser::ASTType::Subscript) {
                auto sub = std::reinterpret_pointer_cast<rlt::parser::ASTSubscript>(node);

                dumpNode(sub->getIndexed());
                std::putchar('[');
                dumpNode(sub->getIndex());
                std::putchar(']');
            } else if (node->getType() == rlt::parser::ASTType::Literal) {
                auto literal = std::reinterpret_pointer_cast<rlt::parser::ASTLiteral>(node);

                switch (literal->getLiteralType()) {
                    case rlt::parser::ASTLiteral::Type::Integer: {
                        fmt::print("{}", literal->getInteger());
                        break;
                    }

                    case rlt::parser::ASTLiteral::Type::Decimal: {
                        fmt::print("{}", literal->getDecimal());
                        break;
                    }

                    case rlt::parser::ASTLiteral::Type::Name: {
                        fmt::print("{}", literal->getString());
                        break;
                    }

                    case rlt::parser::ASTLiteral::Type::String: {
                        fmt::print("\"{}\"", escapeString(literal->getString()));
                        break;
                    }

                    case rlt::parser::ASTLiteral::Type::Character: {
                        fmt::print("'{}'", literal->getCharacter());
                        break;
                    }

                    case rlt::parser::ASTLiteral::Type::Bool: {
                        fmt::print("{}", literal->getBool());
                        break;
                    }
                }
            } else if (node->getType() == rlt::parser::ASTType::UnaryOperator) {
                auto unop = std::reinterpret_pointer_cast<rlt::parser::ASTUnaryOperator>(node);

                const char *opname;

                switch (unop->getUnopType()) {
                    case rlt::parser::ASTUnaryOperator::Type::LogicalNot: {
                        opname = "!";
                        break;
                    }

                    case rlt::parser::ASTUnaryOperator::Type::BitNot: {
                        opname = "~";
                        break;
                    }

                    case rlt::parser::ASTUnaryOperator::Type::Minus: {
                        opname = "-";
                        break;
                    }

                    case rlt::parser::ASTUnaryOperator::Type::Dereference: {
                        opname = "*";
                        break;
                    }

                    case rlt::parser::ASTUnaryOperator::Type::AddressOf: {
                        opname = "^";
                        break;
                    }
                }

                fmt::print("{}", opname);
                dumpNode(unop->getNode());
            } else if (node->getType() == rlt::parser::ASTType::BinaryOperator) {
                auto binop = std::reinterpret_pointer_cast<rlt::parser::ASTBinaryOperator>(node);

                const char *opname;

                switch (binop->getBinopType()) {
                    case rlt::parser::ASTBinaryOperator::Type::Add: {
                        opname = "+";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::Subtract: {
                        opname = "-";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::Modulo: {
                        opname = "%";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::Multiply: {
                        opname = "*";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::Divide: {
                        opname = "/";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::BitShiftLeft: {
                        opname = "<<";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::BitShiftRight: {
                        opname = ">>";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalLessThan: {
                        opname = "<";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalLessThanEqual: {
                        opname = "<=";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalGreaterThan: {
                        opname = ">";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalGreaterThanEqual: {
                        opname = ">=";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalEqual: {
                        opname = "==";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalNotEqual: {
                        opname = "!=";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::BitAnd: {
                        opname = "&";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::BitXor: {
                        opname = "^";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::BitOr: {
                        opname = "|";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalAnd: {
                        opname = "&&";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::LogicalOr: {
                        opname = "||";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::NamespaceResolution: {
                        opname = "::";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::MemberResolution: {
                        opname = ".";
                        break;
                    }

                    case rlt::parser::ASTBinaryOperator::Type::Assign: {
                        opname = "=";
                        break;
                    }
                }

                dumpNode(binop->getLeft());
                if (binop->getBinopType() != rlt::parser::ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != rlt::parser::ASTBinaryOperator::Type::MemberResolution) fmt::print(" {} ", opname);
                else fmt::print("{}", opname);
                dumpNode(binop->getRight());
            }

            std::putchar(')');
        }
    }
}