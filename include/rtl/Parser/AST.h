#ifndef RTL_PARSER_AST_H
#define RTL_PARSER_AST_H

#include <string_view>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <vector>

#include <cstdint>

#include "rtl/Core/SourceLocation.h"

namespace rtl {
    namespace sema {
        class Type;
    }

    namespace parser {
        // VariableReference, and FunctionReference aren't used by the parser; they are for translating names -> declarations (e.g., name(println) -> funref(fundecl(println)))
        enum class ASTType {
            BuiltinType,
            VariableDeclaration,
            VariableDefinition,
            Return,
            Block,
            FunctionHeader,
            FunctionBody,
            While,
            For,
            Range,
            Continue,
            Break,
            If,
            Expression,
            StructureDescription
        };

        struct ASTNode {
            core::SourceLocation begin {}, end {};

            virtual ~ASTNode() = default;

            virtual ASTType getType() const = 0;
        };

        struct Type {
            std::shared_ptr<ASTNode> baseType;
            std::uint32_t pointer;

            std::shared_ptr<sema::Type> evaluatedType;

            Type() = default;
            Type(const std::shared_ptr<ASTNode> &baseType, std::uint32_t pointer);
        };

        struct ASTBuiltinType : public ASTNode {
            enum class Type {
                Auto,
                None,
                Bool,
                I8,
                I16,
                I32,
                I64,
                U8,
                U16,
                U32,
                U64,
                F32,
                F64,
                FunctionPrototype
            };

            Type builtinType;

            struct {
                parser::Type rt;
                std::vector<parser::Type> paramTypes;
            } fpData; // Function-Prototype Type data

            ASTBuiltinType(Type builtinType);

            ASTType getType() const;
        };

        struct ASTVariableDeclaration : public ASTNode {
            enum class Flags : std::uint32_t {
                Public = 0x1,
                Constant = 0x2
            };

            std::shared_ptr<ASTNode> name;
            Type targetTy;
            std::uint32_t flags = 0;

            ASTVariableDeclaration(const std::shared_ptr<ASTNode>& name, const Type &targetTy);

            ASTType getType() const;
        };

        struct ASTVariableDefinition : public ASTNode {
            std::shared_ptr<ASTVariableDeclaration> decl;
            std::shared_ptr<ASTNode> expr;

            ASTVariableDefinition(const std::shared_ptr<ASTVariableDeclaration> &decl, const std::shared_ptr<ASTNode> &expr);

            ASTType getType() const;
        };

        struct ASTReturn : public ASTNode {
            std::shared_ptr<ASTNode> expr;

            ASTReturn(const std::shared_ptr<ASTNode> &expr);

            ASTType getType() const;
        };

        struct ASTBlock : public ASTNode {
            std::shared_ptr<ASTBlock> parent;
            std::vector<std::shared_ptr<ASTNode>> nodes;

            ASTBlock(const std::vector<std::shared_ptr<ASTNode>>& nodes);

            ASTType getType() const;
        };

        struct ASTFunctionBody;
        struct ASTFunctionHeader : public ASTNode {
            enum class Flags : std::uint32_t {
                Public = 0x1,
                Foreign = 0x2,
                Extern = 0x4,
                CCall = 0x8,
                FastCall = 0x10,
            };

            std::shared_ptr<ASTNode> name;
            std::shared_ptr<ASTFunctionBody> body;
            std::vector<std::shared_ptr<ASTVariableDeclaration>> paramDecls;
            Type rt;
            std::shared_ptr<sema::Type> prototype; // Function Prototype

            std::uint32_t flags = 0;

            ASTFunctionHeader() = default;
            ASTFunctionHeader(const std::shared_ptr<ASTNode>& name, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt);

            ASTType getType() const;
        };

        struct ASTFunctionBody : public ASTNode {
            std::shared_ptr<ASTFunctionHeader> header;
            std::shared_ptr<ASTBlock> block;

            ASTFunctionBody() = default;
            ASTFunctionBody(const std::shared_ptr<ASTBlock>& block);

            ASTType getType() const;
        };

        struct ASTWhile : public ASTNode {
            std::shared_ptr<ASTNode> condition;
            std::shared_ptr<ASTNode> statement;

            ASTWhile(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement);

            ASTType getType() const;
        };

        struct ASTFor : public ASTNode {
            std::shared_ptr<ASTNode> expr;
            std::shared_ptr<ASTNode> statement;

            ASTFor(const std::shared_ptr<ASTNode> &expr, const std::shared_ptr<ASTNode> &statement);

            ASTType getType() const;
        };

        struct ASTRange : public ASTNode {
            std::shared_ptr<ASTNode> lower;
            std::shared_ptr<ASTNode> upper;

            ASTRange(const std::shared_ptr<ASTNode> &lower, const std::shared_ptr<ASTNode> &upper);

            ASTType getType() const;
        };

        struct ASTContinue : public ASTNode {
            ASTType getType() const;
        };

        struct ASTBreak : public ASTNode {
            ASTType getType() const;
        };

        struct ASTIf : public ASTNode {
            std::shared_ptr<ASTNode> condition;
            std::shared_ptr<ASTNode> statement;
            std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> elifs;
            std::shared_ptr<ASTNode> elseStatement;

            ASTIf(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement, const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> &elifs, const std::shared_ptr<ASTNode> &elseStatement);

            ASTType getType() const;
        };

        struct ASTExpression : public ASTNode {
            enum class Type {
                Ref,
                Call,
                Subscript,
                Literal,
                Conversion,
                UnaryOperator,
                BinaryOperator,
            };

            std::shared_ptr<sema::Type> evaluatedType;

            virtual Type getExprType() const = 0;

            ASTType getType() const;
        };

        // References another node (e.g., when passing a variable to a function call)
        struct ASTRef : public ASTExpression {
            std::shared_ptr<ASTNode> node;

            ASTRef(const std::shared_ptr<ASTNode> &node);

            Type getExprType() const;
        };

        struct ASTCall : public ASTExpression {
            std::shared_ptr<ASTNode> called;
            std::vector<std::shared_ptr<ASTNode>> callArgs;

            ASTCall(const std::shared_ptr<ASTNode>& called, const std::vector<std::shared_ptr<ASTNode>>& callArgs);

            ASTExpression::Type getExprType() const;
        };

        struct ASTSubscript : public ASTExpression {
            std::shared_ptr<ASTNode> indexed;
            std::shared_ptr<ASTNode> index;

            ASTSubscript(const std::shared_ptr<ASTNode>& indexed, const std::shared_ptr<ASTNode> index);

            ASTExpression::Type getExprType() const;
        };

        struct ASTLiteral : public ASTExpression {
            enum class Type {
                Integer,
                Decimal,
                Name,
                String,
                Character,
                Bool,
            };

            Type literalType;
            std::variant<std::uint64_t, double, std::string, char, bool> value;
            std::size_t hash;

            ASTLiteral(std::uint64_t value);
            ASTLiteral(double value);
            ASTLiteral(const std::string_view &value, Type ty = Type::String);
            ASTLiteral(bool value);

            ~ASTLiteral();

            // These are just for ease of use
            std::uint64_t getInteger() const;
            double getDecimal() const;
            const std::string &getString() const;
            bool getBool() const;

            ASTExpression::Type getExprType() const;
        };

        struct ASTConversion : public ASTExpression {
            std::shared_ptr<ASTNode> from;
            rtl::parser::Type to;

            ASTConversion(const std::shared_ptr<ASTNode> &from, const rtl::parser::Type &to);

            ASTExpression::Type getExprType() const;
        };

        struct ASTUnaryOperator : public ASTExpression {
            enum class Type {
                LogicalNot,
                BitNot,

                Minus,

                Dereference,
                AddressOf
            };

            Type unopType;
            std::shared_ptr<ASTNode> node;

            ASTUnaryOperator(Type unopType, const std::shared_ptr<ASTNode>& node);

            ASTExpression::Type getExprType() const;
        };

        struct ASTBinaryOperator : public ASTExpression {
            enum class Type {
                Add,
                Subtract,

                Modulo,
                Multiply,
                Divide,

                BitShiftLeft,
                BitShiftRight,

                LogicalLessThan,
                LogicalLessThanEqual,

                LogicalGreaterThan,
                LogicalGreaterThanEqual,

                LogicalEqual,
                LogicalNotEqual,

                BitAnd,
                BitXor,
                BitOr,

                LogicalAnd,
                LogicalOr,

                NamespaceResolution,
                MemberResolution,

                Assign
            };

            Type binopType;
            std::shared_ptr<ASTNode> left, right;

            ASTBinaryOperator(Type binopType, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

            ASTExpression::Type getExprType() const;
        };

        struct ASTStructureDescription : public ASTNode {
            std::shared_ptr<ASTNode> name;

            std::vector<std::shared_ptr<ASTVariableDeclaration>> members;

            ASTStructureDescription(const std::shared_ptr<ASTNode> &name, const std::vector<std::shared_ptr<ASTVariableDeclaration>> &members);

            ASTType getType() const;
        };
    }
}

#endif /* RTL_PARSER_AST_H */
