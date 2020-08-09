#ifndef RLT_PARSER_AST_H
#define RLT_PARSER_AST_H

#include <string_view>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <vector>

#include <cstdint>

#include "SourceLocation.h"

namespace rlt {
    namespace parser {
        class TypeDeclaration {
        public:
            enum class Tag {
                DtBoolean,

                DtInt8,
                DtInt16,
                DtInt32,
                DtInt64,

                DtUInt8,
                DtUInt16,
                DtUInt32,
                DtUInt64,

                DtFloat32,
                DtFloat64,

                TtStruct,
                TtEnum,
                TtUnion
            };
        private:
            Tag tag;
            // This should have a std::variant< ASTStructDescription, ASTEnumDescription, ASTUnionDescription> to store information about the declared type.
        public:
            TypeDeclaration(Tag tag);

            Tag getTag() const;
        };

        class Type {
        private:
            std::shared_ptr<TypeDeclaration> declaration;
            std::size_t pointer;
        public:
            Type() = default;
            Type(const std::shared_ptr<TypeDeclaration>& declaration, std::size_t pointer);

            const std::shared_ptr<TypeDeclaration>& getDeclaration() const;
            std::size_t getPointer() const;
        };

        enum class ASTType {
            VariableDeclaration,
            Block,
            FunctionHeader,
            FunctionBody,
            While,
            For,
            If,
            Call,
            Subscript,
            Literal,
            UnaryOperator,
            BinaryOperator
        };

        class ASTNode {
        public:
            SourceLocation location;

            virtual ASTType getType() const = 0;
        };

        class ASTVariableDeclaration : public ASTNode {
        private:
            std::shared_ptr<ASTNode> target;
        public:
            ASTVariableDeclaration(const std::shared_ptr<ASTNode>& target);

            const std::shared_ptr<ASTNode> getTarget() const;

            ASTType getType() const;
        };

        class ASTBlock : public ASTNode {
        private:
            std::shared_ptr<ASTBlock> parent;
            std::vector<std::shared_ptr<ASTNode>> nodes;
        public:
            ASTBlock(const std::shared_ptr<ASTBlock>& parent, const std::vector<std::shared_ptr<ASTNode>>& nodes);

            const std::shared_ptr<ASTBlock>& getParent() const;
            const std::vector<std::shared_ptr<ASTNode>> getNodes() const;

            ASTType getType() const;
        };

        class ASTFunctionBody;
        class ASTFunctionHeader : public ASTNode {
        public:
            enum class Flags : std::uint32_t {
                Public = 0x1,
                Private = 0x2,
                Foreign = 0x4,
            };
        private:
            std::shared_ptr<ASTNode> target;
            std::shared_ptr<ASTFunctionBody> body;
            std::vector<std::shared_ptr<ASTVariableDeclaration>> paramDecls;
            Type rt;

            std::uint32_t flags;
        public:
            ASTFunctionHeader() = default;
            ASTFunctionHeader(const std::shared_ptr<ASTNode>& target, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt);

            void setBody(const std::shared_ptr<ASTFunctionBody> body);
            void setFlags(std::uint32_t flags);

            const std::shared_ptr<ASTFunctionBody>& getBody() const;
            const std::vector<std::shared_ptr<ASTVariableDeclaration>>& getParamDecls() const;
            const std::shared_ptr<ASTNode>& getRT() const;
            std::uint32_t getFlags() const;

            ASTType getType() const;
        };

        class ASTFunctionBody : public ASTNode {
        private:
            std::shared_ptr<ASTFunctionHeader> header;

            std::shared_ptr<ASTBlock> block;
        public:
            ASTFunctionBody() = default;
            ASTFunctionBody(const std::shared_ptr<ASTBlock>& block);

            const std::shared_ptr<ASTBlock>& getBlock() const;

            ASTType getType() const;
        };

        class ASTIf : public ASTNode {
        private:
            std::shared_ptr<ASTNode> condition;
            std::shared_ptr<ASTBlock> block;
            std::shared_ptr<ASTIf> elif;
            std::shared_ptr<ASTBlock> elseBlock;
        public:
            ASTIf(const std::shared_ptr<ASTNode>& condition, const std::shared_ptr<ASTBlock>& block, const std::shared_ptr<ASTIf>& elif, const std::shared_ptr<ASTBlock>& elseBlock);

            const std::shared_ptr<ASTNode>& getCondition() const;
            const std::shared_ptr<ASTBlock>& getBlock() const;
            const std::shared_ptr<ASTIf>& getElif() const;
            const std::shared_ptr<ASTBlock> getElseBlock() const;

            ASTType getType() const;
        };

        class ASTCall : public ASTNode {
        private:
            std::shared_ptr<ASTNode> called;
            std::vector<std::shared_ptr<ASTNode>> callArgs;
        public:
            ASTCall(const std::shared_ptr<ASTNode>& called, const std::vector<std::shared_ptr<ASTNode>>& callArgs);

            const std::shared_ptr<ASTNode>& getCalled() const;
            const std::vector<std::shared_ptr<ASTNode>> getCallArgs() const;

            ASTType getType() const;
        };

        class ASTSubscript : public ASTNode {
        private:
            std::shared_ptr<ASTNode> indexed;
            std::shared_ptr<ASTNode> index;
        public:
            ASTSubscript(const std::shared_ptr<ASTNode>& indexed, const std::shared_ptr<ASTNode> index);

            const std::shared_ptr<ASTNode>& getIndexed() const;
            const std::shared_ptr<ASTNode>& getIndex() const;

            ASTType getType() const;
        };

        class ASTLiteral : public ASTNode {
        public:
            enum class Type {
                Integer,
                Decimal,
                Name,
                String,
                Character,
                Bool,
            };
        private:
            Type literalType;
            std::variant<std::uint64_t, double, std::string, char, bool> value;
        public:
            ASTLiteral(std::uint64_t value);
            ASTLiteral(double value);
            ASTLiteral(const std::string_view &value, Type ty = Type::String);
            ASTLiteral(char value);
            ASTLiteral(bool value);

            ~ASTLiteral();

            Type getLiteralType() const;

            std::uint64_t getInteger() const;
            double getDecimal() const;
            const std::string &getString() const;
            char getCharacter() const;
            bool getBool() const;

            ASTType getType() const;
        };

        class ASTUnaryOperator : public ASTNode {
        public:
            enum class Type {
                LogicalNot,
                BitNot,

                Minus,

                Dereference,
                AddressOf
            };
        private:
            Type unopType;
            std::shared_ptr<ASTNode> node;
        public:
            ASTUnaryOperator(Type unopType, const std::shared_ptr<ASTNode>& node);

            Type getUnopType() const;

            const std::shared_ptr<ASTNode>& getNode() const;

            ASTType getType() const;
        };

        class ASTBinaryOperator : public ASTNode {
        public:
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
        private:
            Type binopType;
            std::shared_ptr<ASTNode> left, right;
        public:
            ASTBinaryOperator(Type binopType, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right);

            Type getBinopType() const;

            const std::shared_ptr<ASTNode> getLeft();
            const std::shared_ptr<ASTNode> getRight();

            ASTType getType() const;
        };
    }
}

#endif /* RLT_PARSER_AST_H */
