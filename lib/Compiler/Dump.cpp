#include "Dump.h"

#include <fmt/format.h>

namespace lang {
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

        void dumpNode(const std::shared_ptr<lang::parser::ASTNode>& node) {
            if (!node) return;

            std::putchar('(');

            if (node->getType() == lang::parser::ASTType::Call) {
                auto call = std::reinterpret_pointer_cast<lang::parser::ASTCall>(node);

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
            } else if (node->getType() == lang::parser::ASTType::Subscript) {
                auto sub = std::reinterpret_pointer_cast<lang::parser::ASTSubscript>(node);

                dumpNode(sub->getIndexed());
                std::putchar('[');
                dumpNode(sub->getIndex());
                std::putchar(']');
            } else if (node->getType() == lang::parser::ASTType::Literal) {
                auto literal = std::reinterpret_pointer_cast<lang::parser::ASTLiteral>(node);

                switch (literal->getLiteralType()) {
                    case lang::parser::ASTLiteral::Type::Integer: {
                        fmt::print("{}", literal->getInteger());
                        break;
                    }

                    case lang::parser::ASTLiteral::Type::Decimal: {
                        fmt::print("{}", literal->getDecimal());
                        break;
                    }

                    case lang::parser::ASTLiteral::Type::Name: {
                        fmt::print("{}", literal->getString());
                        break;
                    }

                    case lang::parser::ASTLiteral::Type::String: {
                        fmt::print("\"{}\"", escapeString(literal->getString()));
                        break;
                    }

                    case lang::parser::ASTLiteral::Type::Character: {
                        fmt::print("'{}'", literal->getCharacter());
                        break;
                    }

                    case lang::parser::ASTLiteral::Type::Bool: {
                        fmt::print("{}", literal->getBool());
                        break;
                    }
                }
            } else if (node->getType() == lang::parser::ASTType::UnaryOperator) {
                auto unop = std::reinterpret_pointer_cast<lang::parser::ASTUnaryOperator>(node);

                const char *opname;

                switch (unop->getUnopType()) {
                    case lang::parser::ASTUnaryOperator::Type::LogicalNot: {
                        opname = "!";
                        break;
                    }

                    case lang::parser::ASTUnaryOperator::Type::BitNot: {
                        opname = "~";
                        break;
                    }

                    case lang::parser::ASTUnaryOperator::Type::Minus: {
                        opname = "-";
                        break;
                    }

                    case lang::parser::ASTUnaryOperator::Type::Dereference: {
                        opname = "*";
                        break;
                    }

                    case lang::parser::ASTUnaryOperator::Type::AddressOf: {
                        opname = "^";
                        break;
                    }
                }

                fmt::print("{}", opname);
                dumpNode(unop->getNode());
            } else if (node->getType() == lang::parser::ASTType::BinaryOperator) {
                auto binop = std::reinterpret_pointer_cast<lang::parser::ASTBinaryOperator>(node);

                const char *opname;

                switch (binop->getBinopType()) {
                    case lang::parser::ASTBinaryOperator::Type::Add: {
                        opname = "+";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::Subtract: {
                        opname = "-";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::Modulo: {
                        opname = "%";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::Multiply: {
                        opname = "*";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::Divide: {
                        opname = "/";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::BitShiftLeft: {
                        opname = "<<";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::BitShiftRight: {
                        opname = ">>";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalLessThan: {
                        opname = "<";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalLessThanEqual: {
                        opname = "<=";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalGreaterThan: {
                        opname = ">";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalGreaterThanEqual: {
                        opname = ">=";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalEqual: {
                        opname = "==";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalNotEqual: {
                        opname = "!=";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::BitAnd: {
                        opname = "&";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::BitXor: {
                        opname = "^";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::BitOr: {
                        opname = "|";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalAnd: {
                        opname = "&&";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::LogicalOr: {
                        opname = "||";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::NamespaceResolution: {
                        opname = "::";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::MemberResolution: {
                        opname = ".";
                        break;
                    }

                    case lang::parser::ASTBinaryOperator::Type::Assign: {
                        opname = "=";
                        break;
                    }
                }

                dumpNode(binop->getLeft());
                if (binop->getBinopType() != lang::parser::ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != lang::parser::ASTBinaryOperator::Type::MemberResolution) fmt::print(" {} ", opname);
                else fmt::print("{}", opname);
                dumpNode(binop->getRight());
            }

            std::putchar(')');
        }
    }
}