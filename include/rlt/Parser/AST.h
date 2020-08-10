#ifndef RLT_PARSER_AST_H
#define RLT_PARSER_AST_H

#include <string_view>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <vector>

#include <cstdint>

#include "rlt/Core/SourceLocation.h"

namespace rlt {
    namespace parser {
        // Move this to Sema eventually.
        class TypeDeclaration {
        public:
            enum class Tag {
                DtBool,

                DtI8,
                DtI16,
                DtI32,
                DtI64,

                DtU8,
                DtU16,
                DtU32,
                DtU64,

                DtF32,
                DtF64,

                TtStruct,
                TtEnum,
                TtUnion
            };
        private:
            Tag tag;
            std::string name;
            // This should have a std::variant< ASTStructDescription, ASTEnumDescription, ASTUnionDescription> to store information about the declared type.
        public:
            TypeDeclaration(Tag tag, const std::string &name);

            Tag getTag() const;
            const std::string &getName() const;
        };

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
            If,
            Call,
            Subscript,
            Literal,
            Conversion,
            UnaryOperator,
            BinaryOperator
        };

        class ASTNode {
        public:
            core::SourceLocation begin, end;

            virtual ~ASTNode() = default;

            virtual ASTType getType() const = 0;
        };

        class ASTBuiltinType : public ASTNode {
        public:
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
                F64
            };
        private:
            Type builtinType;
        public:
            ASTBuiltinType(Type builtinType);

            Type getBuiltinType() const;

            ASTType getType() const;
        };

        class Type {
        private:
            std::shared_ptr<ASTNode> baseType;
            std::size_t pointer;
        public:
            Type() = default;
            Type(const std::shared_ptr<ASTNode>& baseType, std::size_t pointer);

            const std::shared_ptr<ASTNode>& getBaseType() const;
            std::size_t getPointer() const;
        };

        class ASTVariableDeclaration : public ASTNode {
        public:
            enum class Flags : std::uint32_t {
                Constant = 0x1
            };
        private:
            std::shared_ptr<ASTNode> name;
            Type targetTy;
            std::uint32_t flags = 0;
        public:
            ASTVariableDeclaration(const std::shared_ptr<ASTNode>& name, const Type &targetTy);

            void setFlags(std::uint32_t flags);

            const std::shared_ptr<ASTNode> getName() const;
            const Type &getTargetTy() const;
            std::uint32_t getFlags() const;

            ASTType getType() const;
        };

        class ASTVariableDefinition : public ASTNode {
        private:
            std::shared_ptr<ASTVariableDeclaration> decl;
            std::shared_ptr<ASTNode> expr;
        public:
            ASTVariableDefinition(const std::shared_ptr<ASTVariableDeclaration> &decl, const std::shared_ptr<ASTNode> &expr);

            const std::shared_ptr<ASTVariableDeclaration> &getDecl() const;
            const std::shared_ptr<ASTNode> &getExpr() const;

            ASTType getType() const;
        };

        class ASTReturn : public ASTNode {
        private:
            std::shared_ptr<ASTNode> value;
        public:
            ASTReturn(const std::shared_ptr<ASTNode> &value);

            const std::shared_ptr<ASTNode> &getValue() const;

            ASTType getType() const;
        };

        class ASTBlock : public ASTNode {
        private:
            std::shared_ptr<ASTBlock> parent;
            std::vector<std::shared_ptr<ASTNode>> nodes;
        public:
            ASTBlock(const std::vector<std::shared_ptr<ASTNode>>& nodes);

            void setParent(const std::shared_ptr<ASTBlock> &parent);

            const std::shared_ptr<ASTBlock>& getParent() const;
            const std::vector<std::shared_ptr<ASTNode>> getNodes() const;

            ASTType getType() const;
        };

        class ASTFunctionBody;
        class ASTFunctionHeader : public ASTNode {
        public:
            enum class Flags : std::uint32_t {
                Private = 0x1,
                Foreign = 0x2,
                Extern = 0x4,
                CCall = 0x8,
                FastCall = 0x10,
            };
        private:
            std::shared_ptr<ASTNode> name;
            std::shared_ptr<ASTFunctionBody> body;
            std::vector<std::shared_ptr<ASTVariableDeclaration>> paramDecls;
            Type rt;

            std::uint32_t flags = 0;
        public:
            ASTFunctionHeader() = default;
            ASTFunctionHeader(const std::shared_ptr<ASTNode>& name, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt);

            void setBody(const std::shared_ptr<ASTFunctionBody> body);
            void setFlags(std::uint32_t flags);

            const std::shared_ptr<ASTNode> &getName() const;
            const std::shared_ptr<ASTFunctionBody>& getBody() const;
            const std::vector<std::shared_ptr<ASTVariableDeclaration>>& getParamDecls() const;
            const Type& getRT() const;
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

            void setHeader(const std::shared_ptr<ASTFunctionHeader> &header);

            const std::shared_ptr<ASTFunctionHeader> &getHeader() const;
            const std::shared_ptr<ASTBlock>& getBlock() const;

            ASTType getType() const;
        };

        class ASTIf : public ASTNode {
        private:
            std::shared_ptr<ASTNode> condition;
            std::shared_ptr<ASTNode> statement;
            std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> elifs;
            std::shared_ptr<ASTNode> elseStatement;
        public:
            ASTIf(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement, const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> &elifs, const std::shared_ptr<ASTNode> &elseStatement);

            const std::shared_ptr<ASTNode> &getCondition() const;
            const std::shared_ptr<ASTNode> &getStatement() const;
            const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> getElifs() const;
            const std::shared_ptr<ASTNode> &getElseStatement() const;

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

        class ASTConversion : public ASTNode {
        private:
            std::shared_ptr<ASTNode> from;
            Type to;
        public:
            ASTConversion(const std::shared_ptr<ASTNode> &from, const Type &to);

            const std::shared_ptr<ASTNode> &getFrom() const;
            const Type &getTo() const;

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
