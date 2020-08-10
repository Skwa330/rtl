#include "rlt/parser/AST.h"

namespace rlt {
    namespace parser {
        TypeDeclaration::TypeDeclaration(Tag tag, const std::string &name) {
            this->tag = tag;
            this->name = name;
        }

        TypeDeclaration::Tag TypeDeclaration::getTag() const {
            return tag;
        }

        const std::string &TypeDeclaration::getName() const {
            return name;
        }

        Type::Type(const std::shared_ptr<ASTNode>& baseType, std::size_t pointer) {
            this->baseType = baseType;
            this->pointer = pointer;
        }

        const std::shared_ptr<ASTNode>& Type::getBaseType() const {
            return baseType;
        }

        std::size_t Type::getPointer() const {
            return pointer;
        }

        // Move ^^ these to Sema eventually.

        ASTBuiltinType::ASTBuiltinType(Type builtinType) {
            this->builtinType = builtinType;
        }

        ASTBuiltinType::Type ASTBuiltinType::getBuiltinType() const {
            return builtinType;
        }

        ASTType ASTBuiltinType::getType() const {
            return ASTType::BuiltinType;
        }

        ASTVariableDeclaration::ASTVariableDeclaration(const std::shared_ptr<ASTNode>& name, const Type &targetTy) {
            this->name = name;
            this->targetTy = targetTy;
        }

        void ASTVariableDeclaration::setFlags(std::uint32_t flags) {
            this->flags = flags;
        }

        const std::shared_ptr<ASTNode> ASTVariableDeclaration::getName() const {
            return name;
        }

        const Type &ASTVariableDeclaration::getTargetTy() const {
            return targetTy;
        }

        std::uint32_t ASTVariableDeclaration::getFlags() const {
            return flags;
        }

        ASTType ASTVariableDeclaration::getType() const {
            return ASTType::VariableDeclaration;
        }

        ASTVariableDefinition::ASTVariableDefinition(const std::shared_ptr<ASTVariableDeclaration> &decl, const std::shared_ptr<ASTNode> &expr) {
            this->decl = decl;
            this->expr = expr;
        }

        const std::shared_ptr<ASTVariableDeclaration> &ASTVariableDefinition::getDecl() const {
            return decl;
        }

        const std::shared_ptr<ASTNode> &ASTVariableDefinition::getExpr() const {
            return expr;
        }

        ASTType ASTVariableDefinition::getType() const {
            return ASTType::VariableDefinition;
        }

        ASTReturn::ASTReturn(const std::shared_ptr<ASTNode> &value) {
            this->value = value;
        }

        const std::shared_ptr<ASTNode> &ASTReturn::getValue() const {
            return value;
        }

        ASTType ASTReturn::getType() const {
            return ASTType::Return;
        }

        ASTBlock::ASTBlock(const std::vector<std::shared_ptr<ASTNode>>& nodes) {
            this->nodes = nodes;
        }

        void ASTBlock::setParent(const std::shared_ptr<ASTBlock> &parent) {
            this->parent = parent;
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

        ASTFunctionHeader::ASTFunctionHeader(const std::shared_ptr<ASTNode>& name, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt) {
            this->name = name;
            this->paramDecls = paramDecls;
            this->rt = rt;
        }

        void ASTFunctionHeader::setBody(const std::shared_ptr<ASTFunctionBody> body) {
            this->body = body;
        }

        void ASTFunctionHeader::setFlags(std::uint32_t flags) {
            this->flags = flags;
        }

        const std::shared_ptr<ASTNode> &ASTFunctionHeader::getName() const {
            return name;
        }

        const std::shared_ptr<ASTFunctionBody>& ASTFunctionHeader::getBody() const {
            return body;
        }

        const std::vector<std::shared_ptr<ASTVariableDeclaration>>& ASTFunctionHeader::getParamDecls() const {
            return paramDecls;
        }

        const Type &ASTFunctionHeader::getRT() const {
            return rt;
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

        void ASTFunctionBody::setHeader(const std::shared_ptr<ASTFunctionHeader> &header) {
            this->header = header;
        }

        const std::shared_ptr<ASTFunctionHeader> &ASTFunctionBody::getHeader() const {
            return header;
        }

        const std::shared_ptr<ASTBlock>& ASTFunctionBody::getBlock() const {
            return block;
        }

        ASTType ASTFunctionBody::getType() const {
            return ASTType::FunctionBody;
        }

        ASTIf::ASTIf(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement, const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> &elifs, const std::shared_ptr<ASTNode> &elseStatement) {
            this->condition = condition;
            this->statement = statement;
            this->elifs = elifs;
            this->elseStatement = elseStatement;
        }

        const std::shared_ptr<ASTNode> &ASTIf::getCondition() const {
            return condition;
        }

        const std::shared_ptr<ASTNode> &ASTIf::getStatement() const {
            return statement;
        }

        const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> ASTIf::getElifs() const {
            return elifs;
        }

        const std::shared_ptr<ASTNode> &ASTIf::getElseStatement() const {
            return elseStatement;
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
            this->value = value;
        }

        ASTLiteral::ASTLiteral(double value) {
            literalType = Type::Decimal;
            this->value = value;
        }

        ASTLiteral::ASTLiteral(const std::string_view &value, Type ty) {
            literalType = ty;
            this->value = std::string(value.data(), value.size());
        }

        ASTLiteral::ASTLiteral(char value) {
            literalType = Type::Character;
            this->value = (char)value;
        }

        ASTLiteral::ASTLiteral(bool value) {
            literalType = Type::Bool;
            this->value = value;
        }

        ASTLiteral::~ASTLiteral() {
        }

        ASTLiteral::Type ASTLiteral::getLiteralType() const {
            return literalType;
        }

        std::uint64_t ASTLiteral::getInteger() const {
            return std::get<std::uint64_t>(value);
        }

        double ASTLiteral::getDecimal() const {
            return std::get<double>(value);
        }

        const std::string &ASTLiteral::getString() const {
            return std::get<std::string>(value);
        }

        char ASTLiteral::getCharacter() const {
            return std::get<char>(value);
        }

        bool ASTLiteral::getBool() const {
            return std::get<bool>(value);
        }

        ASTType ASTLiteral::getType() const {
            return ASTType::Literal;
        }

        ASTConversion::ASTConversion(const std::shared_ptr<ASTNode> &from, const Type &to) {
            this->from = from;
            this->to = to;
        }

        const std::shared_ptr<ASTNode> &ASTConversion::getFrom() const {
            return from;
        }

        const Type &ASTConversion::getTo() const {
            return to;
        }

        ASTType ASTConversion::getType() const {
            return ASTType::Conversion;
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
