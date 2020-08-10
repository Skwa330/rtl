#include "Dump.h"

#include <fmt/format.h>

// Right now every { and } get their own line because that's the easiest way to dump it LMAO... I'm not lazy... you are.

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

        using namespace rlt::parser;

        std::string dumpType(const Type &ty) {
            std::string result;

            for (std::size_t i = 0; i < ty.getPointer(); i++) {
                result += "^";
            }

            if (ty.getBaseType()->getType() == ASTType::BuiltinType) {
                auto builtinType = std::reinterpret_pointer_cast<ASTBuiltinType>(ty.getBaseType());

                using Bt = ASTBuiltinType::Type;

                switch (builtinType->getBuiltinType()) {
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
                }
            } else if (ty.getBaseType()->getType() == ASTType::Literal || (ty.getBaseType()->getType() == ASTType::BinaryOperator && ((std::reinterpret_pointer_cast<ASTBinaryOperator>(ty.getBaseType()))->getBinopType() == ASTBinaryOperator::Type::NamespaceResolution))) {
                result += dumpNode(ty.getBaseType());
            } else {
                result += "$UNKNOWN";
            }

            return result;
        }

        std::string dumpNode(const std::shared_ptr<ASTNode> &node, std::size_t ind) {
            std::string result;

            for (std::size_t i = 0; i < ind; i++) result += "    ";
            if (!node) return result;

            if (node->getType() == ASTType::If) {
                auto ifStatement = std::reinterpret_pointer_cast<ASTIf>(node);

                result += fmt::format("if {}\n", dumpNode(ifStatement->getCondition()));
                if (ifStatement->getStatement()->getType() == ASTType::Block) {
                    result += dumpNode(ifStatement->getStatement(), ind);
                } else {
                    result += dumpNode(ifStatement->getStatement(), ind + 1);
                }

                for (auto &elif : ifStatement->getElifs()) {
                    result += fmt::format("\nelif {}\n", dumpNode(elif.first));
                    if (elif.second->getType() == ASTType::Block) {
                        result += dumpNode(elif.second, ind);
                    } else {
                        result += dumpNode(elif.second, ind + 1);
                    }
                }

                if (ifStatement->getElseStatement()) {
                    result += "else\n";
                    if (ifStatement->getElseStatement()->getType() == ASTType::Block) {
                        result += dumpNode(ifStatement->getElseStatement(), ind);
                    } else {
                        result += dumpNode(ifStatement->getElseStatement(), ind + 1);
                    }
                }
            } else if (node->getType() == ASTType::Block) {
                auto block = std::reinterpret_pointer_cast<ASTBlock>(node);

                result += "{\n";
                for (auto &node : block->getNodes()) {
                    result += dumpNode(node, ind + 1) + "\n";
                }

                for (std::size_t i = 0; i < ind; i++) {
                    result += "    ";
                }

                result += "}";
            } else if (node->getType() == ASTType::VariableDeclaration) {
                auto decl = std::reinterpret_pointer_cast<ASTVariableDeclaration>(node);

                result += fmt::format("var {}: {}", dumpNode(decl->getName()), dumpType(decl->getTargetTy()));
            } else if (node->getType() == ASTType::VariableDefinition) {
                auto defn = std::reinterpret_pointer_cast<ASTVariableDefinition>(node);

                result += fmt::format("{} = {}", dumpNode(defn->getDecl()), dumpNode(defn->getExpr()));
            } else if (node->getType() == ASTType::FunctionHeader) {
                auto header = std::reinterpret_pointer_cast<ASTFunctionHeader>(node);

                result += "fun ";
                result += dumpNode(header->getName());
                result += " (";

                bool first = true;
                for (auto &decl : header->getParamDecls()) {
                    if (first) {
                        first = false;
                    } else {
                        result += ", ";
                    }

                    result += dumpNode(decl);
                }

                result += ") ";

                if (header->getFlags()) {
                    result += "[";

                    if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Private) {
                        result += "$private";
                    }

                    if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Foreign) {
                        if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Private) result += " ";
                        result += "$foreign";
                    }

                    if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Extern) {
                        if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Foreign || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Private) result += " ";
                        result += "$extern";
                    }

                    if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::CCall) {
                        if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Extern || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Foreign || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Private) result += " ";
                        result += "$ccall";
                    }

                    if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::FastCall) {
                        if (header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::CCall || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Extern || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Foreign || header->getFlags() & (std::uint32_t)ASTFunctionHeader::Flags::Private) result += " ";
                        result += "$fastcall";
                    }

                    result += "] ";
                }

                result += fmt::format("-> {}", dumpType(header->getRT()));

                if (header->getBody()) {
                    result += fmt::format("\n{}", dumpNode(header->getBody()->getBlock()));
                }
            } else if (node->getType() == ASTType::FunctionBody) {
                auto body = std::reinterpret_pointer_cast<ASTFunctionBody>(node);

                result += dumpNode(body->getBlock(), ind);
            } else if (node->getType() == ASTType::Call) {
                auto call = std::reinterpret_pointer_cast<ASTCall>(node);

                result += dumpNode(call->getCalled());
                result += "(";
                bool first = true;
                for (auto &node : call->getCallArgs()) {
                    if (first) {
                        first = false;
                    } else {
                        result += ", ";
                    }

                    result += dumpNode(node);
                }
                result += ")";
            } else if (node->getType() == ASTType::Subscript) {
                auto sub = std::reinterpret_pointer_cast<ASTSubscript>(node);

                result += fmt::format("{}[{}]", dumpNode(sub->getIndexed()), dumpNode(sub->getIndex()));
            } else if (node->getType() == ASTType::Literal) {
                auto literal = std::reinterpret_pointer_cast<ASTLiteral>(node);

                switch (literal->getLiteralType()) {
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
                        result += fmt::format("'{}'", literal->getCharacter());
                        break;
                    }

                    case ASTLiteral::Type::Bool: {
                        result += fmt::format("{}", literal->getBool());
                        break;
                    }
                }
            } else if (node->getType() == ASTType::Conversion) {
                auto conversion = std::reinterpret_pointer_cast<ASTConversion>(node);

                result += dumpNode(conversion->getFrom());
                result += " as (";

                for (std::size_t i = 0; i < conversion->getTo().getPointer(); i++) {
                    result += "^";
                }

                result += fmt::format("{})", dumpType(conversion->getTo()));
            } else if (node->getType() == ASTType::UnaryOperator) {
                auto unop = std::reinterpret_pointer_cast<ASTUnaryOperator>(node);

                const char *opname;

                switch (unop->getUnopType()) {
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

                result += fmt::format("{}{}", opname, dumpNode(unop->getNode()));
            } else if (node->getType() == ASTType::BinaryOperator) {
                auto binop = std::reinterpret_pointer_cast<ASTBinaryOperator>(node);

                const char *opname;

                switch (binop->getBinopType()) {
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

                if (binop->getBinopType() != ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != ASTBinaryOperator::Type::MemberResolution) result += "(";
                result += dumpNode(binop->getLeft());
                if (binop->getBinopType() != ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != ASTBinaryOperator::Type::MemberResolution) result += fmt::format(" {} ", opname);
                else result += opname;
                result += dumpNode(binop->getRight());
                if (binop->getBinopType() != ASTBinaryOperator::Type::NamespaceResolution && binop->getBinopType() != ASTBinaryOperator::Type::MemberResolution) result += ")";
            }

            return result;
        }
    }
}