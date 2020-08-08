#include "lang/parser/AST.h"

namespace lang {
    namespace parser {
        TypeDeclaration::TypeDeclaration(Tag tag) {
            this->tag = tag;
        }

        TypeDeclaration::Tag TypeDeclaration::getTag() const {
            return tag;
        }

        Type::Type(const std::shared_ptr<TypeDeclaration>& declaration, std::size_t pointer) {
            this->declaration = declaration;
            this->pointer = pointer;
        }

        const std::shared_ptr<TypeDeclaration>& Type::getDeclaration() const {
            return declaration;
        }

        std::size_t Type::getPointer() const {
            return pointer;
        }

        ASTVariableDeclaration::ASTVariableDeclaration(const std::shared_ptr<ASTNode>& target) {
            this->target = target;
        }

        const std::shared_ptr<ASTNode> ASTVariableDeclaration::getTarget() const {
            return target;
        }

        ASTType ASTVariableDeclaration::getType() const {
            return ASTType::VariableDeclaration;
        }

        ASTBlock::ASTBlock(const std::shared_ptr<ASTBlock>& parent, const std::vector<std::shared_ptr<ASTNode>>& nodes) {
            this->parent = parent;
            this->nodes = nodes;
        }

        const std::shared_ptr<ASTBlock>& ASTBlock::getParent() const {
            return parent;
        }

        const std::vector<std::shared_ptr<ASTNode>> ASTBlock::getNodes() const {
            return nodes;
        }

        ASTType ASTBlock::getType() const {
            return ASTType::Block;
        }

        ASTFunctionHeader::ASTFunctionHeader(const std::shared_ptr<ASTNode>& target, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt) {
            this->target = target;
            this->paramDecls = paramDecls;
            this->rt = rt;
        }

        void ASTFunctionHeader::setBody(const std::shared_ptr<ASTFunctionBody> body) {
            this->body = body;
        }

        void ASTFunctionHeader::setFlags(std::uint32_t flags) {
            this->flags = flags;
        }

        const std::shared_ptr<ASTFunctionBody>& ASTFunctionHeader::getBody() const {
            return body;
        }

        const std::vector<std::shared_ptr<ASTVariableDeclaration>>& ASTFunctionHeader::getParamDecls() const {
            return paramDecls;
        }

        std::uint32_t ASTFunctionHeader::getFlags() const {
            return flags;
        }

        ASTType ASTFunctionHeader::getType() const {
            return ASTType::FunctionHeader;
        }

        ASTFunctionBody::ASTFunctionBody(const std::shared_ptr<ASTBlock>& block) {
            this->block = block;
        }

        const std::shared_ptr<ASTBlock>& ASTFunctionBody::getBlock() const {
            return block;
        }

        ASTType ASTFunctionBody::getType() const {
            return ASTType::FunctionBody;
        }

        ASTIf::ASTIf(const std::shared_ptr<ASTNode>& condition, const std::shared_ptr<ASTBlock>& block, const std::shared_ptr<ASTIf>& elif, const std::shared_ptr<ASTBlock>& elseBlock) {
            this->condition = condition;
            this->block = block;
            this->elif = elif;
            this->elseBlock = elseBlock;
        }

        const std::shared_ptr<ASTNode>& ASTIf::getCondition() const {
            return condition;
        }

        const std::shared_ptr<ASTBlock>& ASTIf::getBlock() const {
            return block;
        }

        const std::shared_ptr<ASTIf>& ASTIf::getElif() const {
            return elif;
        }

        const std::shared_ptr<ASTBlock> ASTIf::getElseBlock() const {
            return elseBlock;
        }

        ASTType ASTIf::getType() const {
            return ASTType::If;
        }

        ASTCall::ASTCall(const std::shared_ptr<ASTNode>& called, const std::vector<std::shared_ptr<ASTNode>>& callArgs) {
            this->called = called;
            this->callArgs = callArgs;
        }

        const std::shared_ptr<ASTNode>& ASTCall::getCalled() const {
            return called;
        }

        const std::vector<std::shared_ptr<ASTNode>> ASTCall::getCallArgs() const {
            return callArgs;
        }

        ASTType ASTCall::getType() const {
            return ASTType::Call;
        }

        ASTSubscript::ASTSubscript(const std::shared_ptr<ASTNode>& indexed, const std::shared_ptr<ASTNode> index) {
            this->indexed = indexed;
            this->index = index;
        }

        const std::shared_ptr<ASTNode>& ASTSubscript::getIndexed() const {
            return indexed;
        }

        const std::shared_ptr<ASTNode>& ASTSubscript::getIndex() const {
            return index;
        }

        ASTType ASTSubscript::getType() const {
            return ASTType::Subscript;
        }

        ASTLiteral::ASTLiteral(std::uint64_t value) {
            literalType = Type::Integer;
            *(std::uint64_t*)&this->value = value;
        }

        ASTLiteral::ASTLiteral(double value) {
            literalType = Type::Decimal;
            *(double*)&this->value = value;
        }

        ASTLiteral::ASTLiteral(const std::string_view &value, Type ty) {
            literalType = ty;
            new (&this->value) std::string(value.data(), value.size());
        }

        ASTLiteral::ASTLiteral(char value) {
            literalType = Type::Character;
            *(char*)&this->value = value;
        }

        ASTLiteral::ASTLiteral(bool value) {
            literalType = Type::Bool;
            *(bool*)&this->value = value;
        }

        ASTLiteral::~ASTLiteral() {
            if (literalType == Type::String || literalType == Type::Name) {
                (*(std::string*)&value).~basic_string();
            }
        }

        ASTLiteral::Type ASTLiteral::getLiteralType() const {
            return literalType;
        }

        std::uint64_t ASTLiteral::getInteger() const {
            return *(std::uint64_t*)&value;
        }

        double ASTLiteral::getDecimal() const {
            return *(double*)&value;
        }

        const std::string &ASTLiteral::getString() const {
            return *(std::string*)&value;
        }

        char ASTLiteral::getCharacter() const {
            return *(char*)&value;
        }

        bool ASTLiteral::getBool() const {
            return *(bool*)&value;
        }

        ASTType ASTLiteral::getType() const {
            return ASTType::Literal;
        }

        ASTUnaryOperator::ASTUnaryOperator(ASTUnaryOperator::Type unopType, const std::shared_ptr<ASTNode>& node) {
            this->unopType = unopType;
            this->node = node;
        }

        ASTUnaryOperator::Type ASTUnaryOperator::getUnopType() const {
            return unopType;
        }

        const std::shared_ptr<ASTNode>& ASTUnaryOperator::getNode() const {
            return node;
        }

        ASTType ASTUnaryOperator::getType() const {
            return ASTType::UnaryOperator;
        }

        ASTBinaryOperator::ASTBinaryOperator(Type binopType, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right) {
            this->binopType = binopType;
            this->left = left;
            this->right = right;
        }

        ASTBinaryOperator::Type ASTBinaryOperator::getBinopType() const {
            return binopType;
        }

        const std::shared_ptr<ASTNode> ASTBinaryOperator::getLeft() {
            return left;
        }

        const std::shared_ptr<ASTNode> ASTBinaryOperator::getRight() {
            return right;
        }

        ASTType ASTBinaryOperator::getType() const {
            return ASTType::BinaryOperator;
        }
    }
}
