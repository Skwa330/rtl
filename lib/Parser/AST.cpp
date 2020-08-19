#include "rtl/parser/AST.h"

namespace rtl {
    namespace parser {
        Type::Type(const std::shared_ptr<ASTNode>& baseType, std::uint32_t pointer) {
            this->baseType = baseType;
            this->pointer = pointer;
        }

        ASTBuiltinType::ASTBuiltinType(Type builtinType) {
            this->builtinType = builtinType;
        }

        ASTType ASTBuiltinType::getType() const {
            return ASTType::BuiltinType;
        }

        ASTVariableDeclaration::ASTVariableDeclaration(const std::shared_ptr<ASTNode>& name, const Type &targetTy) {
            this->name = name;
            this->targetTy = targetTy;
        }

        ASTType ASTVariableDeclaration::getType() const {
            return ASTType::VariableDeclaration;
        }

        ASTVariableDefinition::ASTVariableDefinition(const std::shared_ptr<ASTVariableDeclaration> &decl, const std::shared_ptr<ASTNode> &expr) {
            this->decl = decl;
            this->expr = expr;
        }

        ASTType ASTVariableDefinition::getType() const {
            return ASTType::VariableDefinition;
        }

        ASTReturn::ASTReturn(const std::shared_ptr<ASTNode> &expr) {
            this->expr = expr;
        }

        ASTType ASTReturn::getType() const {
            return ASTType::Return;
        }

        ASTBlock::ASTBlock(const std::vector<std::shared_ptr<ASTNode>>& nodes) {
            this->nodes = nodes;
        }

        ASTType ASTBlock::getType() const {
            return ASTType::Block;
        }

        ASTFunctionHeader::ASTFunctionHeader(const std::shared_ptr<ASTNode>& name, const std::vector<std::shared_ptr<ASTVariableDeclaration>>& paramDecls, const Type &rt) {
            this->name = name;
            this->paramDecls = paramDecls;
            this->rt = rt;
        }

        ASTType ASTFunctionHeader::getType() const {
            return ASTType::FunctionHeader;
        }

        ASTFunctionBody::ASTFunctionBody(const std::shared_ptr<ASTBlock>& block) {
            this->block = block;
        }

        ASTType ASTFunctionBody::getType() const {
            return ASTType::FunctionBody;
        }

        ASTWhile::ASTWhile(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement) {
            this->condition = condition;
            this->statement = statement;
        }

        ASTType ASTWhile::getType() const {
            return ASTType::While;
        }

        ASTFor::ASTFor(const std::shared_ptr<ASTNode> &expr, const std::shared_ptr<ASTNode> &statement) {
            this->expr = expr;
            this->statement = statement;
        }

        ASTType ASTFor::getType() const {
            return ASTType::For;
        }

        ASTRange::ASTRange(const std::shared_ptr<ASTNode> &lower, const std::shared_ptr<ASTNode> &upper) {
            this->lower = lower;
            this->upper = upper;
        }

        ASTType ASTRange::getType() const {
            return ASTType::Range;
        }

        ASTType ASTContinue::getType() const {
            return ASTType::Continue;
        }

        ASTType ASTBreak::getType() const {
            return ASTType::Break;
        }

        ASTIf::ASTIf(const std::shared_ptr<ASTNode> &condition, const std::shared_ptr<ASTNode> &statement, const std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> &elifs, const std::shared_ptr<ASTNode> &elseStatement) {
            this->condition = condition;
            this->statement = statement;
            this->elifs = elifs;
            this->elseStatement = elseStatement;
        }

        ASTType ASTIf::getType() const {
            return ASTType::If;
        }

        ASTType ASTExpression::getType() const {
           return ASTType::Expression;
        }

        ASTCall::ASTCall(const std::shared_ptr<ASTNode>& called, const std::vector<std::shared_ptr<ASTNode>>& callArgs) {
            this->called = called;
            this->callArgs = callArgs;
        }

        ASTExpression::Type ASTCall::getExprType() const {
            return ASTExpression::Type::Call;
        }

        ASTSubscript::ASTSubscript(const std::shared_ptr<ASTNode>& indexed, const std::shared_ptr<ASTNode> index) {
            this->indexed = indexed;
            this->index = index;
        }

        ASTExpression::Type ASTSubscript::getExprType() const {
            return ASTExpression::Type::Subscript;
        }

        ASTLiteral::ASTLiteral(std::uint64_t value) {
            literalType = Type::Integer;
            this->value = value;
            hash = std::hash<decltype(value)>()(value);
        }

        ASTLiteral::ASTLiteral(double value) {
            literalType = Type::Decimal;
            this->value = value;
            hash = std::hash<decltype(value)>()(value);
        }

        ASTLiteral::ASTLiteral(const std::string_view &value, Type ty) {
            literalType = ty;
            this->value = std::string(value.data(), value.size());
            hash = std::hash<std::string_view>()(value);
        }

        ASTLiteral::ASTLiteral(char value) {
            literalType = Type::Character;
            this->value = (char)value;
            hash = std::hash<decltype(value)>()(value);
        }

        ASTLiteral::ASTLiteral(bool value) {
            literalType = Type::Bool;
            this->value = value;
            hash = std::hash<decltype(value)>()(value);
        }

        ASTLiteral::~ASTLiteral() {
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

        ASTExpression::Type ASTLiteral::getExprType() const {
            return ASTExpression::Type::Literal;
        }

        ASTConversion::ASTConversion(const std::shared_ptr<ASTNode> &from, const rtl::parser::Type &to) {
            this->from = from;
            this->to = to;
        }

        ASTExpression::Type ASTConversion::getExprType() const {
            return ASTExpression::Type::Conversion;
        }

        ASTUnaryOperator::ASTUnaryOperator(ASTUnaryOperator::Type unopType, const std::shared_ptr<ASTNode>& node) {
            this->unopType = unopType;
            this->node = node;
        }

        ASTExpression::Type ASTUnaryOperator::getExprType() const {
            return ASTExpression::Type::UnaryOperator;
        }

        ASTBinaryOperator::ASTBinaryOperator(Type binopType, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right) {
            this->binopType = binopType;
            this->left = left;
            this->right = right;
        }

        ASTExpression::Type ASTBinaryOperator::getExprType() const {
            return ASTExpression::Type::BinaryOperator;
        }
    }
}
