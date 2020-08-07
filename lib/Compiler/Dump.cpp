#include "Dump.h"

#include <fmt/format.h>

namespace fosl {
    namespace compiler {
        std::string escapeString(const std::string &s) {
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

        void dumpNode(const std::shared_ptr<fosl::parser::ASTNode>& node) {
            std::putchar('(');

            if (node->getType() == fosl::parser::ASTType::Call) {
                auto call = std::reinterpret_pointer_cast<fosl::parser::ASTCall>(node);

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
            } else if (node->getType() == fosl::parser::ASTType::Subscript) {
                auto sub = std::reinterpret_pointer_cast<fosl::parser::ASTSubscript>(node);

                dumpNode(sub->getIndexed());
                std::putchar('[');
                dumpNode(sub->getIndex());
                std::putchar(']');
            } else if (node->getType() == fosl::parser::ASTType::Literal) {
                auto literal = std::reinterpret_pointer_cast<fosl::parser::ASTLiteral>(node);

                switch (literal->getLiteralType()) {
                    case fosl::parser::ASTLiteral::Type::Integer: {
                        fmt::print("{}", literal->getInteger());
                        break;
                    }

                    case fosl::parser::ASTLiteral::Type::Decimal: {
                        fmt::print("{}", literal->getDecimal());
                        break;
                    }

                    case fosl::parser::ASTLiteral::Type::Name: {
                        fmt::print("{}", literal->getString());
                        break;
                    }

                    case fosl::parser::ASTLiteral::Type::String: {
                        fmt::print("\"{}\"", escapeString(literal->getString()));
                        break;
                    }

                    case fosl::parser::ASTLiteral::Type::Character: {
                        fmt::print("'{}'", literal->getCharacter());
                        break;
                    }

                    case fosl::parser::ASTLiteral::Type::Bool: {
                        fmt::print("{}", literal->getBool());
                        break;
                    }
                }
            } else if (node->getType() == fosl::parser::ASTType::UnaryOperator) {
                auto unop = std::reinterpret_pointer_cast<fosl::parser::ASTUnaryOperator>(node);

                const char *opname;

                switch (unop->getUnopType()) {
                    case fosl::parser::ASTUnaryOperator::Type::LogicalNot: {
                        opname = "!";
                        break;
                    }

                    case fosl::parser::ASTUnaryOperator::Type::BitNot: {
                        opname = "~";
                        break;
                    }

                    case fosl::parser::ASTUnaryOperator::Type::Minus: {
                        opname = "-";
                        break;
                    }

                    case fosl::parser::ASTUnaryOperator::Type::Dereference: {
                        opname = "*";
                        break;
                    }

                    case fosl::parser::ASTUnaryOperator::Type::AddressOf: {
                        opname = "^";
                        break;
                    }
                }

                fmt::print("{}", opname);
                dumpNode(unop->getNode());
            } else if (node->getType() == fosl::parser::ASTType::BinaryOperator) {
                auto binop = std::reinterpret_pointer_cast<fosl::parser::ASTBinaryOperator>(node);

                const char *opname;

                switch (binop->getBinopType()) {
                    case fosl::parser::ASTBinaryOperator::Type::Add: {
                        opname = "+";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::Subtract: {
                        opname = "-";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::Modulo: {
                        opname = "%";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::Multiply: {
                        opname = "*";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::Divide: {
                        opname = "/";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::BitShiftLeft: {
                        opname = "<<";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::BitShiftRight: {
                        opname = ">>";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalLessThan: {
                        opname = "<";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalLessThanEqual: {
                        opname = "<=";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalGreaterThan: {
                        opname = ">";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalGreaterThanEqual: {
                        opname = ">=";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalEqual: {
                        opname = "==";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalNotEqual: {
                        opname = "!=";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::BitAnd: {
                        opname = "&";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::BitXor: {
                        opname = "^";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::BitOr: {
                        opname = "|";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalAnd: {
                        opname = "&&";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::LogicalOr: {
                        opname = "||";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::NamespaceResolution: {
                        opname = "::";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::MemberResolution: {
                        opname = ".";
                        break;
                    }

                    case fosl::parser::ASTBinaryOperator::Type::Assign: {
                        opname = "=";
                        break;
                    }
                }

                dumpNode(binop->getLeft());
                if (binop->getBinopType() != fosl::parser::ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != fosl::parser::ASTBinaryOperator::Type::MemberResolution) fmt::print(" {} ", opname);
                else fmt::print("{}", opname);
                dumpNode(binop->getRight());
            }

            std::putchar(')');
        }
    }
}