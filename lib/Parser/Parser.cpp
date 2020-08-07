#include "fosl/parser/Parser.h"
#include "fosl/Error.h"
#include <fmt/format.h>

namespace fosl {
    namespace parser {
        Parser::Parser(std::vector<std::shared_ptr<ASTNode>>& nodes) : nodes(nodes) {
            lexer = std::make_unique<Lexer>();

            types = std::make_shared<Types>();

            types->booleanType = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtBoolean);

            types->int8Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtInt8);
            types->int16Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtInt16);
            types->int32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtInt32);
            types->int64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtInt64);

            types->uint8Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtUInt8);
            types->uint16Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtUInt16);
            types->uint32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtUInt32);
            types->uint64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtUInt64);

            types->float32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtFloat32);
            types->float64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::DtFloat64);
        }

        void Parser::initFromSource(const std::string &moduleName, const std::string &source) {
            lexer->initFromSource(moduleName, source);
        }

        void Parser::initFromFile(const std::string &filepath) {
            lexer->initFromFile(filepath);
        }

        const std::unique_ptr<Lexer>& Parser::getLexer() const {
            return lexer;
        }

        void Parser::parseSyntaxTree() {

        }

        std::shared_ptr<ASTNode> Parser::parse() {
            return nullptr;
        }

        MatchType Parser::match(std::size_t beg) {
            return MatchType(0, false);
        }

        Type Parser::parseType() {
            std::size_t pointer = 0;
            while (lexer->peek().type == TokenType::BitXor) {
                lexer->eat();
                ++pointer;
            }

            std::shared_ptr<TypeDeclaration> declaration;

            if (lexer->peek().type == TokenType::KwBoolean) {
                lexer->eat();
                declaration = types->booleanType;
            } else if (lexer->peek().type == TokenType::KwInt8) {
                lexer->eat();
                declaration = types->int8Type;
            } else if (lexer->peek().type == TokenType::KwInt16) {
                lexer->eat();
                declaration = types->int16Type;
            } else if (lexer->peek().type == TokenType::KwInt32) {
                lexer->eat();
                declaration = types->int32Type;
            } else if (lexer->peek().type == TokenType::KwInt64) {
                lexer->eat();
                declaration = types->int64Type;
            } else if (lexer->peek().type == TokenType::KwUInt8) {
                lexer->eat();
                declaration = types->uint8Type;
            } else if (lexer->peek().type == TokenType::KwUInt16) {
                lexer->eat();
                declaration = types->uint16Type;
            } else if (lexer->peek().type == TokenType::KwUInt32) {
                lexer->eat();
                declaration = types->uint32Type;
            } else if (lexer->peek().type == TokenType::KwUInt64) {
                lexer->eat();
                declaration = types->uint64Type;
            } else if (lexer->peek().type == TokenType::KwFloat32) {
                lexer->eat();
                declaration = types->float32Type;
            } else if (lexer->peek().type == TokenType::KwFloat64) {
                lexer->eat();
                declaration = types->float64Type;
            }

            return Type(declaration, pointer);
        }

        MatchType Parser::matchType(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(beg + p).type == TokenType::KwBoolean) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwInt8) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwInt16) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwInt32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwInt64) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwUInt8) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwUInt16) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwUInt32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwUInt64) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwFloat32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwFloat64) {
                ++p;
                return MatchType(p, true);
            } else {
                return MatchType(p, false);
            }
        }

        // std::shared_ptr<ASTVariableDeclaration> Parser::parseVariableDeclaration() {

        // }

        // MatchType Parser::matchVariableDeclaration(std::size_t beg) {

        // }

        // std::shared_ptr<ASTNode> Parser::parseStatement() {

        // }

        // MatchType Parser::matchStatement(std::size_t beg) {

        // }

        // std::shared_ptr<ASTBlock> Parser::parseBlock() {

        // }

        // MatchType Parser::matchBlock(std::size_t beg) {

        // }

        // std::shared_ptr<ASTIf> parseIf() {

        // }

        // MatchType matchIf(std::size_t beg = 0) {

        // }

        // std::shared_ptr<ASTFunctionHeader> Parser::parseFunction() {

        // }

        // MatchType Parser::matchFunction(std::size_t beg) {

        // }

        std::shared_ptr<ASTNode> Parser::parseParen() {
            if (lexer->peek().type != TokenType::LeftParen) return nullptr;
            lexer->eat();

            auto result = parseExpr();

            if (lexer->peek().type != TokenType::RightParen) return nullptr;
            lexer->eat();

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseExpr() {
            return parseAssignment();
        }

        std::shared_ptr<ASTNode> Parser::parseAssignment() {
            std::shared_ptr<ASTNode> result = parseLogicalOr();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            if ((lexer->peek().type == TokenType::Equal || lexer->peek().type == TokenType::AddEqual || lexer->peek().type == TokenType::SubtractEqual || lexer->peek().type == TokenType::ModuloEqual || lexer->peek().type == TokenType::MultiplyEqual || lexer->peek().type == TokenType::DivideEqual || lexer->peek().type == TokenType::BitAndEqual || lexer->peek().type == TokenType::BitXorEqual || lexer->peek().type == TokenType::BitOrEqual || lexer->peek().type == TokenType::BitShiftLeftEqual || lexer->peek().type == TokenType::BitShiftRightEqual) && std::get<1>(matchAssignment(1))) {
                ASTBinaryOperator::Type intermTy;
                if (lexer->peek().type == TokenType::AddEqual) intermTy = ASTBinaryOperator::Type::Add;
                else if (lexer->peek().type == TokenType::SubtractEqual) intermTy = ASTBinaryOperator::Type::Subtract;
                else if (lexer->peek().type == TokenType::ModuloEqual) intermTy = ASTBinaryOperator::Type::Modulo;
                else if (lexer->peek().type == TokenType::MultiplyEqual) intermTy = ASTBinaryOperator::Type::Multiply;
                else if (lexer->peek().type == TokenType::DivideEqual) intermTy = ASTBinaryOperator::Type::Divide;
                else if (lexer->peek().type == TokenType::BitAndEqual) intermTy = ASTBinaryOperator::Type::BitAnd;
                else if (lexer->peek().type == TokenType::BitXorEqual) intermTy = ASTBinaryOperator::Type::BitXor;
                else if (lexer->peek().type == TokenType::BitOrEqual) intermTy = ASTBinaryOperator::Type::BitOr;
                else if (lexer->peek().type == TokenType::BitShiftLeftEqual) intermTy = ASTBinaryOperator::Type::BitShiftLeft;
                else if (lexer->peek().type == TokenType::BitShiftRightEqual) intermTy = ASTBinaryOperator::Type::BitShiftRight;
                else {
                    lexer->eat();
                    result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::Assign, result, parseAssignment());
                    result->moduleName = lexer->getModuleName();
                    result->line = line;
                    result->lexpos = lexpos;
                    return result;
                }

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
                lexer->eat();

                auto interm = std::make_shared<ASTBinaryOperator>(intermTy, result, parseAssignment());
                interm->moduleName = lexer->getModuleName();
                interm->line = line;
                interm->lexpos = lexpos;

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::Assign, result, interm);

                return result;
            } else {
                return result;
            }
        }

        std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
            std::shared_ptr<ASTNode> result = parseLogicalAnd();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::LogicalOr && std::get<1>(matchLogicalAnd(1))) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::LogicalOr, result, parseLogicalAnd());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
            std::shared_ptr<ASTNode> result = parseDirectComparison();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::LogicalAnd && std::get<1>(matchDirectComparison(1))) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::LogicalAnd, result, parseDirectComparison());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseDirectComparison() {
            std::shared_ptr<ASTNode> result = parseComparison();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::LogicalEqual || lexer->peek().type == TokenType::LogicalNotEqual) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalEqual) ty = ASTBinaryOperator::Type::LogicalEqual;
                else if (lexer->peek().type == TokenType::LogicalNotEqual) ty = ASTBinaryOperator::Type::LogicalNotEqual;
                lexer->eat();

                if (!std::get<1>(matchComparison())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseComparison());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseComparison() {
            std::shared_ptr<ASTNode> result = parseBitOr();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::LogicalLessThan || lexer->peek().type == TokenType::LogicalLessThanEqual || lexer->peek().type == TokenType::LogicalGreaterThan || lexer->peek().type == TokenType::LogicalGreaterThanEqual) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalLessThan) ty = ASTBinaryOperator::Type::LogicalLessThan;
                else if (lexer->peek().type == TokenType::LogicalLessThanEqual) ty = ASTBinaryOperator::Type::LogicalLessThanEqual;
                else if (lexer->peek().type == TokenType::LogicalGreaterThan) ty = ASTBinaryOperator::Type::LogicalGreaterThan;
                else if (lexer->peek().type == TokenType::LogicalGreaterThanEqual) ty = ASTBinaryOperator::Type::LogicalGreaterThanEqual;
                lexer->eat();

                if (!std::get<1>(matchBitOr())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseBitOr());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitOr() {
            std::shared_ptr<ASTNode> result = parseBitXor();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::BitOr) {
                lexer->eat();

                if (!std::get<1>(matchBitXor())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitOr, result, parseBitXor());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitXor() {
            std::shared_ptr<ASTNode> result = parseBitAnd();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::BitXor) {
                lexer->eat();

                if (!std::get<1>(matchBitAnd())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitXor, result, parseBitAnd());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitAnd() {
            std::shared_ptr<ASTNode> result = parseBitShift();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::BitAnd) {
                lexer->eat();

                if (!std::get<1>(matchBitShift())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitAnd, result, parseBitShift());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitShift() {
            std::shared_ptr<ASTNode> result = parseTerm();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while ((lexer->peek().type == TokenType::BitShiftLeft || lexer->peek().type == TokenType::BitShiftRight)) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::BitShiftLeft) ty = ASTBinaryOperator::Type::BitShiftLeft;
                else if (lexer->peek().type == TokenType::BitShiftRight) ty = ASTBinaryOperator::Type::BitShiftRight;
                lexer->eat();

                if (!std::get<1>(matchTerm())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseTerm());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseTerm() {
            std::shared_ptr<ASTNode> result = parseFactor();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::Add || lexer->peek().type == TokenType::Subtract) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::Add) ty = ASTBinaryOperator::Type::Add;
                else if (lexer->peek().type == TokenType::Add) ty = ASTBinaryOperator::Type::Subtract;
                lexer->eat();

                if (!std::get<1>(matchFactor())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseFactor());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseFactor() {
            std::shared_ptr<ASTNode> result = parseUnary();

            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            while (lexer->peek().type == TokenType::Modulo || lexer->peek().type == TokenType::Multiply || lexer->peek().type == TokenType::Divide) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::Modulo) ty = ASTBinaryOperator::Type::Modulo;
                else if (lexer->peek().type == TokenType::Multiply) ty = ASTBinaryOperator::Type::Multiply;
                else if (lexer->peek().type == TokenType::Divide) ty = ASTBinaryOperator::Type::Divide;
                lexer->eat();

                if (!std::get<1>(matchUnary())) {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid second operand.");
                }

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseUnary());
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseUnary() {
            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            if (lexer->peek().type == TokenType::LogicalNot || lexer->peek().type == TokenType::BitNot || lexer->peek().type == TokenType::Add || lexer->peek().type == TokenType::Subtract || lexer->peek().type == TokenType::Multiply || lexer->peek().type == TokenType::BitXor) {
                ASTUnaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalNot) ty = ASTUnaryOperator::Type::LogicalNot;
                else if (lexer->peek().type == TokenType::BitNot) ty = ASTUnaryOperator::Type::BitNot;
                else if (lexer->peek().type == TokenType::Add) { lexer->eat(); return parseUnary(); }
                else if (lexer->peek().type == TokenType::Subtract) ty = ASTUnaryOperator::Type::Minus;
                else if (lexer->peek().type == TokenType::Multiply) ty = ASTUnaryOperator::Type::Dereference;
                else if (lexer->peek().type == TokenType::BitXor) ty = ASTUnaryOperator::Type::AddressOf;
                lexer->eat();

                std::shared_ptr<ASTNode> second;

                if (std::get<1>(matchParen())) {
                    second = parseParen();
                } else if (std::get<1>(matchUnary())) {
                    second = parseUnary();
                } else {
                    throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid unary operand.");
                }

                auto result = std::make_shared<ASTUnaryOperator>(ty, second);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;
                return result;
            } else if (std::get<1>(matchCallSubscriptOrMember())) {
                return parseCallSubscriptOrMember();
            } else {
                return nullptr;
            }
        }

        std::shared_ptr<ASTNode> Parser::parseCallSubscriptOrMember() {
            std::shared_ptr<ASTNode> result = parseOne();

            while (lexer->peek().type == TokenType::LeftParen || lexer->peek().type == TokenType::LeftBracket || lexer->peek().type == TokenType::Dot) {
                if (lexer->peek().type == TokenType::LeftParen) {
                    std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;
                    lexer->eat();

                    std::vector<std::shared_ptr<ASTNode>> callArgs;

                    while (lexer->peek().type != TokenType::RightParen) {
                        callArgs.push_back(parseExpr());

                        if (lexer->peek().type != TokenType::Comma && lexer->peek().type != TokenType::RightParen) {
                            throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->getModuleName(), line, lexpos, "expected ')' to terminate call.");
                        }

                        if (lexer->peek().type == TokenType::Comma) lexer->eat();
                    }

                    lexer->eat();

                    result = std::make_shared<ASTCall>(result, callArgs);
                    result->moduleName = lexer->getModuleName();
                    result->line = line;
                    result->lexpos = lexpos;
                } else if (lexer->peek().type == TokenType::LeftBracket) {
                    std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

                    lexer->eat();

                    result = std::make_shared<ASTSubscript>(result, parseExpr());
                    result->moduleName = lexer->getModuleName();
                    result->line = line;
                    result->lexpos = lexpos;

                    if (lexer->peek().type != TokenType::RightBracket) {
                        throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->getModuleName(), line, lexpos, "expected ']' to terminate subscript.");
                    }

                    lexer->eat();
                } else if (lexer->peek().type == TokenType::Dot) {
                    std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;
                    lexer->eat();

                    if (lexer->peek().type != TokenType::Name) {
                        throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->getModuleName(), line, lexpos, "expected name after '.'.");
                    }

                    auto n = std::make_shared<ASTLiteral>(lexer->peek().text.first, lexer->peek().text.second, ASTLiteral::Type::Name);
                    n->moduleName = lexer->getModuleName();
                    n->line = lexer->peek().line;
                    n->lexpos = lexer->peek().lexpos;
                    lexer->eat();

                    result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::MemberResolution, result, n);
                    result->moduleName = lexer->getModuleName();
                    result->line = line;
                    result->lexpos = lexpos;
                }
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseOne() {
            std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

            if (std::get<1>(matchParen())) {
                return parseParen();
            } else if (lexer->peek().type == TokenType::Integer) {
                auto result = std::make_shared<ASTLiteral>(*(std::uint64_t*)&lexer->peek().litrl);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Decimal) {
                auto result = std::make_shared<ASTLiteral>(*(double*)&lexer->peek().litrl);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::String) {
                auto result = std::make_shared<ASTLiteral>(lexer->peek().text.first, lexer->peek().text.second);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Character) {
                if (lexer->peek().text.second > 1) throw Error(Error::Type::Syntactical, Error::Priority::Error, lexer->peek().moduleName, lexer->peek().line, lexer->peek().lexpos, "invalid character literal.");
                auto result = std::make_shared<ASTLiteral>(lexer->peek().text.first[0]);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::KwTrue || lexer->peek().type == TokenType::KwFalse) {
                auto result = std::make_shared<ASTLiteral>(lexer->peek().type == TokenType::KwTrue);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Name) {
                return parseName();
            }

            return nullptr;
        }

        std::shared_ptr<ASTNode> Parser::parseName() {
            if (lexer->peek().type == TokenType::Name) {
                std::uint32_t line = lexer->peek().line, lexpos = lexer->peek().lexpos;

                std::shared_ptr<ASTNode> result = std::make_shared<ASTLiteral>(lexer->peek().text.first, lexer->peek().text.second, ASTLiteral::Type::Name);
                result->moduleName = lexer->getModuleName();
                result->line = line;
                result->lexpos = lexpos;

                line = lexer->peek().line;
                lexpos = lexer->peek().lexpos;

                lexer->eat();

                while (lexer->peek().type == TokenType::ColonColon && lexer->peek(1).type == TokenType::Name) {
                    std::shared_ptr<ASTNode> second = std::make_shared<ASTLiteral>(lexer->peek(1).text.first, lexer->peek(1).text.second, ASTLiteral::Type::Name);
                    lexer->eat(2);

                    result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::NamespaceResolution, result, second);
                    result->moduleName = lexer->getModuleName();
                    result->line = line;
                    result->lexpos = lexpos;

                    line = lexer->peek().line;
                    lexpos = lexer->peek().lexpos;
                }

                return result;
            } else {
                return nullptr;
            }
        }

        MatchType Parser::matchParen(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(beg + p).type != TokenType::LeftParen) return MatchType(p, false);
            ++p;

            c = matchExpr(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            if (lexer->peek(beg + p).type != TokenType::RightParen) return MatchType(p, false);
            ++p;

            return MatchType(p, true);
        }

        MatchType Parser::matchExpr(std::size_t beg) {
            return matchAssignment(beg);
        }

        MatchType Parser::matchAssignment(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchLogicalOr(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            if (lexer->peek(beg + p).type == TokenType::Equal || lexer->peek(beg + p).type == TokenType::AddEqual || lexer->peek(beg + p).type == TokenType::SubtractEqual || lexer->peek(beg + p).type == TokenType::ModuloEqual || lexer->peek(beg + p).type == TokenType::MultiplyEqual || lexer->peek(beg + p).type == TokenType::DivideEqual || lexer->peek(beg + p).type == TokenType::BitAndEqual || lexer->peek(beg + p).type == TokenType::BitXorEqual || lexer->peek(beg + p).type == TokenType::BitOrEqual || lexer->peek(beg + p).type == TokenType::BitShiftLeftEqual || lexer->peek(beg + p).type == TokenType::BitShiftRightEqual) {
                ++p;

                c = matchAssignment(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchLogicalOr(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchLogicalAnd(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::LogicalOr) {
                ++p;

                c = matchLogicalAnd(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchLogicalAnd(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchDirectComparison(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::LogicalAnd) {
                ++p;

                c = matchDirectComparison(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchDirectComparison(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchComparison(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::LogicalEqual || lexer->peek(beg + p).type == TokenType::LogicalNotEqual) {
                ++p;

                c = matchComparison(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchComparison(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitOr(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::LogicalLessThan || lexer->peek(beg + p).type == TokenType::LogicalLessThanEqual || lexer->peek(beg + p).type == TokenType::LogicalGreaterThan || lexer->peek(beg + p).type == TokenType::LogicalGreaterThanEqual) {
                ++p;

                c = matchBitOr(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitOr(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitXor(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::BitOr) {
                ++p;

                c = matchBitXor(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitXor(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitAnd(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::BitXor) {
                ++p;

                c = matchBitAnd(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitAnd(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitShift(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::BitAnd) {
                ++p;

                c = matchBitShift(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitShift(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchTerm(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::BitShiftLeft || lexer->peek(beg + p).type == TokenType::BitShiftRight) {
                ++p;

                c = matchTerm(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchTerm(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchFactor(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::Add || lexer->peek(beg + p).type == TokenType::Subtract) {
                ++p;

                c = matchFactor(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchFactor(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchUnary(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::Modulo || lexer->peek(beg + p).type == TokenType::Multiply || lexer->peek(beg + p).type == TokenType::Divide) {
                ++p;

                c = matchUnary(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchUnary(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(beg + p).type == TokenType::LogicalNot || lexer->peek(beg + p).type == TokenType::BitNot || lexer->peek(beg + p).type == TokenType::Add || lexer->peek(beg + p).type == TokenType::Subtract || lexer->peek(beg + p).type == TokenType::Multiply || lexer->peek(beg + p).type == TokenType::BitXor) {
                ++p;

                c = matchUnary(beg + p);
                if (!std::get<1>(c) && !std::get<1>(c = matchParen())) return MatchType(p, false);
                p += std::get<0>(c);
            } else {
                c = matchCallSubscriptOrMember(beg + p);
                if (!std::get<1>(c)) return MatchType(p, false);
                p += std::get<0>(c);
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchCallSubscriptOrMember(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            c = matchOne(beg + p);
            if (!std::get<1>(c)) return MatchType(p, false);
            p += std::get<0>(c);

            while (lexer->peek(beg + p).type == TokenType::LeftParen || lexer->peek(beg + p).type == TokenType::LeftBracket || (lexer->peek(beg + p).type == TokenType::Dot && lexer->peek(beg + p + 1).type == TokenType::Name)) {
                ++p;

                if (lexer->peek(beg + p).type == TokenType::LeftParen) {
                    ++p;

                    while (lexer->peek(beg + p).type != TokenType::RightParen) {
                        c = matchExpr(beg + p);
                        if (!std::get<1>(c)) return MatchType(p, false);
                        p += std::get<0>(c);

                        if (lexer->peek(beg + p).type != TokenType::Comma && lexer->peek(beg + p).type != TokenType::RightParen) return MatchType(0, false);

                        if (lexer->peek(beg + p).type == TokenType::Comma) {
                            ++p;
                        }
                    }

                    ++p;
                } else if (lexer->peek(beg + p).type == TokenType::LeftBracket) {
                    ++p;

                    c = matchExpr(beg + p);
                    if (!std::get<1>(c)) return MatchType(p, false);
                    p += std::get<0>(c);

                    if (lexer->peek(beg + 0).type != TokenType::RightBracket) {
                        if (!std::get<1>(c)) return MatchType(p, false);
                    }

                    ++p;
                } else if (lexer->peek(beg + p).type == TokenType::Dot && lexer->peek(beg + p + 1).type == TokenType::Name) {
                    p += 2;
                }
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchOne(std::size_t beg) {
            std::size_t p = 0;
            MatchType c;

            if (std::get<1>((c = matchParen(beg + p)))) {
                p += std::get<0>(c);
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::Integer) {
                return MatchType(++p, true);
            } else if (lexer->peek(beg + p).type == TokenType::Decimal) {
                return MatchType(++p, true);
            } else if (lexer->peek(beg + p).type == TokenType::String) {
                return MatchType(++p, true);
            } else if (lexer->peek(beg + p).type == TokenType::Character) {
                return MatchType(++p, true);
            } else if (lexer->peek(beg + p).type == TokenType::KwTrue || lexer->peek().type == TokenType::KwFalse) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(beg + p).type == TokenType::Name) {
                return matchName(beg + p++);
            } else {
                return MatchType(p, false);
            }
        }

        MatchType Parser::matchName(std::size_t beg) {
            std::size_t p = 0;
            if (lexer->peek(beg + p).type != TokenType::Name) return MatchType(p, false);
            ++p;

            while (lexer->peek(beg + p).type == TokenType::ColonColon && lexer->peek(beg + p + 1).type == TokenType::Name) {
                p += 2;
            }

            return MatchType(p, true);
        }
    }
}
