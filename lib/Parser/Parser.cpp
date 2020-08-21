#include "rtl/Parser/Parser.h"
#include "rtl/Core/Error.h"

#include <signal.h>

#include <fmt/format.h>

namespace rtl {
    namespace parser {
        Parser::Parser(std::vector<std::shared_ptr<ASTNode>>& nodes) : nodes(nodes) {
            lexer = std::make_unique<Lexer>();
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
            if (!matchSyntaxTree().second) throw error;

            while (matchTopLevel().second) {
                nodes.push_back(parseTopLevel());
            }
        }

        MatchType Parser::matchSyntaxTree(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            while ((c = matchTopLevel(b + p)).second) {
                p += c.first;
            }

            if (c.first) {
                return MatchType(p + c.first, false);
            }

            return MatchType(p, true);
        }

        std::shared_ptr<ASTNode> Parser::parseTopLevel() {
            if (!matchTopLevel().second) throw error;

            if (matchFunction().second) {
                return parseFunction();
            } else if (matchVariableDefinition().second) {
                return parseVariableDefinition();
            } else if (matchVariableDeclaration().second) {
                return parseVariableDeclaration();
            }

            return {};
        }

        MatchType Parser::matchTopLevel(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if ((c = matchFunction(b + p)).second) {
                return MatchType(p + c.first, true);
            } else if (c.first) {
                return MatchType(p + c.first, false);
            } else if ((c = matchVariableDefinition(b + p)).second) {
                return MatchType(p + c.first, true);
            } else if (c.first) {
                return MatchType(p + c.first, false);
            } else if ((c = matchVariableDeclaration(b + p)).second) {
                return MatchType(p + c.first, true);
            } else if (c.first) {
                return MatchType(p + c.first, false);
            } else if (lexer->peek(b + p).type != TokenType::Eoi) {
                return MatchType(p, false);
            }

            return MatchType(p, false);
        }

        Type Parser::parseType() {
            if (!matchType().second) throw error;

            std::uint32_t pointer = 0;
            while (lexer->peek().type == TokenType::BitXor) {
                lexer->eat();
                ++pointer;
            }

            std::shared_ptr<ASTNode> baseType;

            if (lexer->peek().type == TokenType::KwNone) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::None);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwBool) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::Bool);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwI8) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::I8);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwI16) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::I16);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwI32) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::I32);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwI64) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::I64);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwU8) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::U8);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwU16) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::U16);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwU32) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::U32);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwU64) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::U64);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwF32) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::F32);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (lexer->peek().type == TokenType::KwF64) {
                baseType = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::F64);
                baseType->begin = lexer->peek().begin;
                baseType->end = lexer->peek().end;
                lexer->eat();
            } else if (matchName().second) {
                baseType = parseName();
            } else if (lexer->peek().type == TokenType::LeftParen) {
                auto ebt = std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::FunctionPrototype);
                ebt->begin = lexer->peek().begin;
                lexer->eat(); // (

                while (lexer->peek().type != TokenType::RightParen) {
                    ebt->fpData.paramTypes.push_back(parseType());

                    if (lexer->peek().type == TokenType::Comma) lexer->eat();
                }

                lexer->eat(2); // ) ->
                ebt->fpData.rt = parseType();
                ebt->end = lexer->peek().end;

                if (pointer) {
                    throw core::Error(core::Error::Type::Syntactic, lexer->source, ebt->begin, ebt->end, "pointer to function prototype is invalid.");
                }

                baseType = ebt;
            }

            return Type(baseType, pointer);
        }

        MatchType Parser::matchType(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            while (lexer->peek(b + p).type == TokenType::BitXor) {
                ++p;
            }

            if (lexer->peek(b + p).type == TokenType::KwNone) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwBool) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwI8) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwI16) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwI32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwI64) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwU8) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwU16) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwU32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwU64) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwF32) {
                ++p;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwF64) {
                ++p;
                return MatchType(p, true);
            } else if ((c = matchName(b + p)).second) {
                p += c.first;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::LeftParen) {
                ++p;

                while (lexer->peek(b + p).type != TokenType::RightParen) {
                    if (!(c = matchType(b + p)).second) {
                        return MatchType(p + c.first, false);
                    }
                    p += c.first;

                    if (lexer->peek(b + p).type != TokenType::Comma && lexer->peek(b + p).type != TokenType::RightParen) {
                        error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ',' or ')'.");
                        return MatchType(p, false);
                    }

                    if (lexer->peek(b + p).type == TokenType::Comma) ++p;
                }

                ++p;

                if (lexer->peek(b + p).type != TokenType::Arrow) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '->'.");
                    return MatchType(p, false);
                }
                ++p;

                if (!(c = matchType(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;
            } else {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{}'", lexer->peek(b + p).text));
                return MatchType(p, false);
            }

            return MatchType(p, true);
        }

        std::shared_ptr<ASTVariableDeclaration> Parser::parseVariableDeclaration() {
            if (!matchVariableDeclaration().second) throw error;

            std::uint32_t flags = 0;

            core::SourceLocation begin = lexer->peek().begin;

            if (lexer->peek().type == TokenType::KwVal) {
                flags |= (std::uint32_t)ASTVariableDeclaration::Flags::Constant;
            }

            lexer->eat(); // val or var

            auto name = parseName();
            // auto name = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
            // name->begin = lexer->peek().begin;
            // name->end = lexer->peek().end;
            // lexer->eat();

            lexer->eat(); // :

            auto ty = parseType();

            auto decl = std::make_shared<ASTVariableDeclaration>(name, ty);
            decl->begin = begin;
            decl->end = ty.baseType->end;
            decl->flags = flags;

            return decl;
        }

        MatchType Parser::matchVariableDeclaration(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type == TokenType::KwVal) {
                ++p;
            } else if (lexer->peek(b + p).type == TokenType::KwVar) {
                ++p;
            } else {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));;
                return MatchType(p, false);
            }

            if (!(c = matchName(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (lexer->peek(b + p).type != TokenType::Colon) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ':'.");
                return MatchType(p + c.first, false);
            }
            ++p;

            if (!(c = matchType(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTVariableDefinition> Parser::parseVariableDefinition() {
            if (!matchVariableDefinition().second) throw error;

            std::shared_ptr<ASTVariableDeclaration> decl;

            std::uint32_t flags = 0;

            if (lexer->peek().type == TokenType::KwVal) {
                flags |= (std::uint32_t)ASTVariableDeclaration::Flags::Constant;
            }

            core::SourceLocation begin = lexer->peek().begin, end;
            lexer->eat(); // val or var

            auto name = parseName();
            // auto name = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
            // name->begin = lexer->peek().begin;
            // name->end = lexer->peek().end;
            // lexer->eat();

            Type ty;

            if (lexer->peek().type == TokenType::Colon) {
                lexer->eat();

                ty = parseType();
                end = ty.baseType->end;
            } else {
                ty = Type(std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::Auto), 0);
                end = name->end;
            }

            decl = std::make_shared<ASTVariableDeclaration>(name, ty);
            decl->begin = begin;
            decl->end = end;
            decl->flags = flags;

            lexer->eat(); // =

            auto expr = parseExpr();

            auto defn = std::make_shared<ASTVariableDefinition>(decl, expr);
            defn->begin = decl->begin;
            defn->end = expr->end;
            return defn;
        }

        MatchType Parser::matchVariableDefinition(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type == TokenType::KwVal) {
                ++p;
            } else if (lexer->peek(b + p).type == TokenType::KwVar) {
                ++p;
            } else {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }

            if (!(c = matchName(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (lexer->peek(b + p).type == TokenType::Colon) {
                ++p;

                if (!(c = matchType(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;
            }

            if (lexer->peek(b + p).type != TokenType::Equal) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '='.");
                return MatchType(p, false);
            }
            ++p;

            if (!(c = matchExpr(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTStructureDescription> Parser::parseStructureDescription() {
            return {};
        }

        MatchType Parser::matchStructureDescription(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type == TokenType::KwPub) {
                ++p;
            }

            if (lexer->peek(b + p).type != TokenType::KwStruct) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{}'.", lexer->peek(b + p).text));
                return MatchType(p, false);
            }
            ++p;

            if (lexer->peek(b + p).type != TokenType::LeftBrace) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '{'.");
                return MatchType(p, false);
            }
            ++p;



            if (lexer->peek(b + p).type != TokenType::RightBrace) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '}'.");
                return MatchType(p, false);
            }
            ++p;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTNode> Parser::parseStatement() {
            if (!matchStatement().second) throw error;

            if (matchIf().second) {
                return parseIf();
            } else if (matchBlock().second) {
                return parseBlock();
            } else if (matchVariableDefinition().second) {
                return parseVariableDefinition();
            } else if (matchVariableDeclaration().second) {
                return parseVariableDeclaration();
            } else if (matchReturn().second) {
                return parseReturn();
            } else if (matchFor().second) {
                return parseFor();
            } else if (matchWhile().second) {
                return parseWhile();
            } else if (matchContinue().second) {
                return parseContinue();
            } else if (matchBreak().second) {
                return parseBreak();
            } else if (matchExpr().second) {
                return parseExpr();
            }

            return {};
        }

        MatchType Parser::matchStatement(std::size_t b) {
            MatchType c;

            // We do this wonky thing so that we display the proper error.
            if ((c = matchIf(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchBlock(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchVariableDefinition(b)).second) {
                return c;
            } else if (c.first) {
                if (MatchType tmp; (tmp = matchVariableDeclaration(b)).second) {
                    return tmp;
                } else {
                    return c;
                }
            }

            if ((c = matchVariableDeclaration(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchReturn(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchFor(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchWhile(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchContinue(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchBreak(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            if ((c = matchExpr(b)).second) {
                return c;
            } else if (c.first) {
                return MatchType(c.first, false);
            }

            return MatchType(0, false);
        }

        std::shared_ptr<ASTBlock> Parser::parseBlock() {
            if (!matchBlock().second) throw error;

            auto begin = lexer->peek().begin;

            std::vector<std::shared_ptr<ASTNode>> nodes;
            lexer->eat(); // {

            while (matchStatement().second) {
                nodes.push_back(parseStatement());
            }

            auto end = lexer->peek().end;
            lexer->eat(); // }

            auto block = std::make_shared<ASTBlock>(nodes);
            block->begin = begin;
            block->end = end;

            // Set the current block as the parent to all of the sub blocks :)
            for (auto &node : nodes) {
                if (node->getType() == ASTType::Block) {
                    (std::reinterpret_pointer_cast<ASTBlock>(node))->parent = block;
                }
            }

            return block;
        }

        MatchType Parser::matchBlock(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::LeftBrace) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            while ((c = matchStatement(b + p)).second) {
                p += c.first;
            }

            if (c.first) {
                return MatchType(p + c.first, false); // Error whilst parsing statement.
            }

            if (lexer->peek(b + p).type != TokenType::RightBrace) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTReturn> Parser::parseReturn() {
            core::SourceLocation begin = lexer->peek().begin;
            lexer->eat(); // return
            core::SourceLocation end = lexer->peek().end;

            std::shared_ptr<ASTNode> expr;

            if (matchExpr().second) {
                expr = parseExpr();
                end = expr->end;
            }

            auto returnStatement = std::make_shared<ASTReturn>(expr);
            returnStatement->begin = begin;
            returnStatement->end = end;
            return returnStatement;
        }

        MatchType Parser::matchReturn(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwReturn) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            if ((c = matchExpr(b + p)).second) {
                p += c.first;
            } else if (c.first) {
                return MatchType(p + c.first, false);
            }

            return MatchType(p, true);
        }

        std::shared_ptr<ASTBreak> Parser::parseBreak() {
            if (!matchBreak().second) throw error;

            core::SourceLocation begin = lexer->peek().begin, end = lexer->peek().end;
            lexer->eat();

            auto breakStatement = std::make_shared<ASTBreak>();
            breakStatement->begin = begin;
            breakStatement->end = end;
            return breakStatement;
        }

        MatchType Parser::matchBreak(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwBreak) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTContinue> Parser::parseContinue() {
            if (!matchContinue().second) throw error;

            core::SourceLocation begin = lexer->peek().begin, end = lexer->peek().end;
            lexer->eat();

            auto continueStatement = std::make_shared<ASTContinue>();
            continueStatement->begin = begin;
            continueStatement->end = end;
            return continueStatement;
        }

        MatchType Parser::matchContinue(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwContinue) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTFor> Parser::parseFor() {
            if (!matchFor().second) throw error;

            lexer->eat(); // for

            auto expr = parseRange();
            auto statement = parseStatement();

            auto forStatement = std::make_shared<ASTFor>(expr, statement);
            forStatement->begin = expr->begin;
            forStatement->end = statement->end;
            return forStatement;
        }

        MatchType Parser::matchFor(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwFor) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            if (!(c = matchRange(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (!(c = matchStatement(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTRange> Parser::parseRange() {
            if (!matchRange().second) throw error;

            auto lower = parseExpr();
            lexer->eat(); // ..
            auto upper = parseExpr();

            auto range = std::make_shared<ASTRange>(lower, upper);
            range->begin = lower->begin;
            range->end = upper->end;

            return range;
        }

        MatchType Parser::matchRange(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (!(c = matchExpr(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (lexer->peek(b + p).type != TokenType::DotDot) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '..'.");
                return MatchType(p, false);
            }
            ++p;

            if (!(c = matchExpr(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTWhile> Parser::parseWhile() {
            if (!matchWhile().second) throw error;

            core::SourceLocation begin = lexer->peek().begin;
            lexer->eat(); // while;

            std::shared_ptr<ASTNode> condition;

            if (matchExpr().second) {
                condition = parseExpr();
            }

            auto statement = parseStatement();

            auto whileStatement = std::make_shared<ASTWhile>(condition, statement);
            whileStatement->begin = begin;
            whileStatement->end = statement->end;
            return whileStatement;
        }

        MatchType Parser::matchWhile(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwWhile) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected: '{:.{}}'", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p + c.first, false);
            }
            ++p;

            if ((c = matchExpr(b + p)).second) {
                p += c.first;
            } else if (c.first) {
                return MatchType(p + c.first, false);
            }

            if (!(c = matchStatement(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            return MatchType(p, true);
        }

        std::shared_ptr<ASTIf> Parser::parseIf() {
            if (!matchIf().second) throw error;

            core::SourceLocation begin = lexer->peek().begin, end;
            lexer->eat(); // if

            auto condition = parseExpr();
            auto statement = parseStatement();

            std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<ASTNode>>> elifs; // vector<pair<condition, statement>>

            while (lexer->peek().type == TokenType::KwElif) {
                lexer->eat();

                auto condition = parseExpr();
                auto statement = parseStatement();

                elifs.emplace_back(condition, statement);
                end = statement->end;
            }

            std::shared_ptr<ASTNode> elseStatement;

            if (lexer->peek().type == TokenType::KwElse) {
                core::SourceLocation begin = lexer->peek().begin;
                lexer->eat();

                elseStatement = parseStatement();
                elseStatement->begin = begin;
                end = elseStatement->end;
            }

            auto ifStatement = std::make_shared<ASTIf>(condition, statement, elifs, elseStatement);
            ifStatement->begin = begin;
            ifStatement->end = end;
            return ifStatement;
        }

        MatchType Parser::matchIf(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::KwIf) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected 'if'.");
                return MatchType(p, false);
            }
            ++p;

            if (!(c = matchExpr(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (!(c = matchStatement(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::KwElif) {
                ++p;

                if (!(c = matchExpr(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;

                if (!(c = matchStatement(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;
            }

            if (lexer->peek(b + p).type == TokenType::KwElse) {
                ++p;

                if (!(c = matchStatement(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;
            }

            return MatchType(p, true);
        }

        std::shared_ptr<ASTFunctionHeader> Parser::parseFunction() {
            if (!matchFunction().second) throw error;

            std::uint32_t flags = 0;

            core::SourceLocation begin = lexer->peek().begin, end;

            if (lexer->peek().type == TokenType::KwPub) {
                flags |= (std::uint32_t)ASTFunctionHeader::Flags::Public;
                lexer->eat();
            }

            lexer->eat(); // fun

            auto name = parseName();

            std::vector<std::shared_ptr<ASTVariableDeclaration>> paramDecls;

            lexer->eat(); // (

            while (lexer->peek().type != TokenType::RightParen) {
                std::uint32_t flags = 0;

                if (lexer->peek().type == TokenType::KwVar) {
                    lexer->eat();
                } else if (lexer->peek().type == TokenType::KwVal) {
                    flags |= (std::uint32_t)ASTVariableDeclaration::Flags::Constant;
                    lexer->eat();
                }

                auto name = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
                name->begin = lexer->peek().begin;
                name->end = lexer->peek().end;
                lexer->eat();

                lexer->eat(); // :

                auto type = parseType();

                auto decl = std::make_shared<ASTVariableDeclaration>(name, type);
                decl->begin = name->begin;
                decl->end = type.baseType->end;
                decl->flags = flags;

                paramDecls.push_back(decl);

                if (lexer->peek().type == TokenType::Comma) lexer->eat();
            }

            end = lexer->peek().end;
            lexer->eat(); // )

            if (lexer->peek().type == TokenType::LeftBracket) {
                lexer->eat(); // [

                while (lexer->peek().type == TokenType::Dollar) {
                    lexer->eat();

                    auto name = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
                    name->begin = lexer->peek().begin;
                    name->end = lexer->peek().end;
                    lexer->eat();

                    if (name->getString() == "foreign") {
                        flags |= (std::uint32_t)ASTFunctionHeader::Flags::Foreign;
                    } else if (name->getString() == "extern") {
                        flags |= (std::uint32_t)ASTFunctionHeader::Flags::Extern;
                    } else if (name->getString() == "ccall") {
                        flags |= (std::uint32_t)ASTFunctionHeader::Flags::CCall;
                    } else if (name->getString() == "fastcall") {
                        flags |= (std::uint32_t)ASTFunctionHeader::Flags::FastCall;
                    } else {
                        throw core::Error(core::Error::Type::Syntactic, lexer->source, name->begin, name->end, fmt::format("invalid hint '{}'.", name->getString()));
                    }
                }

                end = lexer->peek().end; // In case we have implicit return type we need the proper source location for 'end'
                lexer->eat(); // ]
            }

            Type rt;

            if (lexer->peek().type == TokenType::Arrow) {
                lexer->eat(); // ->
                rt = parseType();
                end = rt.baseType->end;
            } else {
                rt = Type(std::make_shared<ASTBuiltinType>(ASTBuiltinType::Type::None), 0);
            }

            auto functionHeader = std::make_shared<ASTFunctionHeader>(name, paramDecls, rt);
            functionHeader->begin = begin;
            functionHeader->end = end;
            functionHeader->flags = flags;

            if (matchBlock().second) {
                auto functionBody = std::make_shared<ASTFunctionBody>(parseBlock());
                functionBody->begin = functionBody->block->begin;
                functionBody->end = functionBody->block->end;

                functionHeader->body = functionBody;
                functionBody->header = functionHeader;
            }

            return functionHeader;
        }

        MatchType Parser::matchFunction(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type == TokenType::KwPub) {
                ++p;
            }

            if (lexer->peek(b + p).type != TokenType::KwFun) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
            ++p;

            if (!(c = matchName(b + p)).second) {
                return MatchType(p + c.first, false);
            }
            p += c.first;

            if (lexer->peek(b + p).type != TokenType::LeftParen) {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected '('.");
                return MatchType(p, false);
            }
            ++p;

            while (lexer->peek(b + p).type != TokenType::RightParen) {
                if (lexer->peek(b + p).type == TokenType::KwVar || lexer->peek(b + p).type == TokenType::KwVal) {
                    ++p;
                }

                if (lexer->peek(b + p).type != TokenType::Name) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected identifier.");
                    return MatchType(p, false);
                }
                ++p;

                if (lexer->peek(b + p).type != TokenType::Colon) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ':'.");
                    return MatchType(p, false);
                }
                ++p;

                if (!(c = matchType(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;

                if (lexer->peek(b + p).type != TokenType::Comma && lexer->peek(b + p).type != TokenType::RightParen) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ',' or ')'.");
                    return MatchType(p, false);
                }

                if (lexer->peek(b + p).type == TokenType::Comma) ++p;
            }

            ++p;

            if (lexer->peek(b + p).type == TokenType::LeftBracket) {
                ++p;

                while (lexer->peek(b + p).type == TokenType::Dollar) {
                    ++p;

                    if (lexer->peek(b + p).type != TokenType::Name) {
                        error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected identifier.");
                        return MatchType(p, false);
                    }
                    ++p;
                }

                if (lexer->peek(b + p).type != TokenType::RightBracket) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ']'.");
                    return MatchType(p, false);
                }
                ++p;
            }

            if (lexer->peek(b + p).type == TokenType::Arrow) {
                ++p;

                if (!(c = matchType(b + p)).second) {
                    return MatchType(p + c.first, false);
                }
                p += c.first;
            }

            if ((c = matchBlock(b + p)).second) {
                p += c.first;
            } else if (c.first) {
                return MatchType(p + c.first, false);
            }

            return MatchType(p, true);
        }

        std::shared_ptr<ASTNode> Parser::parseParen() {
            if (!matchParen().second) throw error;
            lexer->eat(); // (

            auto result = parseExpr();

            lexer->eat(); // )

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseExpr() {
            return parseAssignment();
        }

        std::shared_ptr<ASTNode> Parser::parseAssignment() {
            if (!matchAssignment().second) throw error;
            std::shared_ptr<ASTNode> result = parseLogicalOr();

            if (lexer->peek().type == TokenType::Equal || lexer->peek().type == TokenType::AddEqual || lexer->peek().type == TokenType::SubtractEqual || lexer->peek().type == TokenType::ModuloEqual || lexer->peek().type == TokenType::MultiplyEqual || lexer->peek().type == TokenType::DivideEqual || lexer->peek().type == TokenType::BitAndEqual || lexer->peek().type == TokenType::BitXorEqual || lexer->peek().type == TokenType::BitOrEqual || lexer->peek().type == TokenType::BitShiftLeftEqual || lexer->peek().type == TokenType::BitShiftRightEqual) {
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
                    result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                    result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
                    return result;
                }

                lexer->eat();

                auto interm = std::make_shared<ASTBinaryOperator>(intermTy, result, parseAssignment());
                interm->begin = interm->left->begin;
                interm->end = interm->right->end;

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::Assign, result, interm);
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
            if (!matchLogicalOr().second) throw error;
            std::shared_ptr<ASTNode> result = parseLogicalAnd();

            while (lexer->peek().type == TokenType::LogicalOr) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::LogicalOr, result, parseLogicalAnd());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
            if (!matchLogicalAnd().second) throw error;
            std::shared_ptr<ASTNode> result = parseDirectComparison();

            while (lexer->peek().type == TokenType::LogicalAnd) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::LogicalAnd, result, parseDirectComparison());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseDirectComparison() {
            if (!matchDirectComparison().second) throw error;
            std::shared_ptr<ASTNode> result = parseComparison();

            while (lexer->peek().type == TokenType::LogicalEqual || lexer->peek().type == TokenType::LogicalNotEqual) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalEqual) ty = ASTBinaryOperator::Type::LogicalEqual;
                else if (lexer->peek().type == TokenType::LogicalNotEqual) ty = ASTBinaryOperator::Type::LogicalNotEqual;
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseComparison());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseComparison() {
            if (!matchComparison().second) throw error;
            std::shared_ptr<ASTNode> result = parseBitOr();

            while (lexer->peek().type == TokenType::LogicalLessThan || lexer->peek().type == TokenType::LogicalLessThanEqual || lexer->peek().type == TokenType::LogicalGreaterThan || lexer->peek().type == TokenType::LogicalGreaterThanEqual) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalLessThan) ty = ASTBinaryOperator::Type::LogicalLessThan;
                else if (lexer->peek().type == TokenType::LogicalLessThanEqual) ty = ASTBinaryOperator::Type::LogicalLessThanEqual;
                else if (lexer->peek().type == TokenType::LogicalGreaterThan) ty = ASTBinaryOperator::Type::LogicalGreaterThan;
                else if (lexer->peek().type == TokenType::LogicalGreaterThanEqual) ty = ASTBinaryOperator::Type::LogicalGreaterThanEqual;
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseBitOr());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitOr() {
            if (!matchBitOr().second) throw error;
            std::shared_ptr<ASTNode> result = parseBitXor();

            while (lexer->peek().type == TokenType::BitOr) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitOr, result, parseBitXor());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitXor() {
            if (!matchBitXor().second) throw error;
            std::shared_ptr<ASTNode> result = parseBitAnd();

            while (lexer->peek().type == TokenType::BitXor) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitXor, result, parseBitAnd());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitAnd() {
            if (!matchBitAnd().second) throw error;
            std::shared_ptr<ASTNode> result = parseBitShift();

            while (lexer->peek().type == TokenType::BitAnd) {
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::BitAnd, result, parseBitShift());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseBitShift() {
            if (!matchBitShift().second) throw error;
            std::shared_ptr<ASTNode> result = parseTerm();

            while ((lexer->peek().type == TokenType::BitShiftLeft || lexer->peek().type == TokenType::BitShiftRight)) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::BitShiftLeft) ty = ASTBinaryOperator::Type::BitShiftLeft;
                else if (lexer->peek().type == TokenType::BitShiftRight) ty = ASTBinaryOperator::Type::BitShiftRight;
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseTerm());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseTerm() {
            if (!matchTerm().second) throw error;
            std::shared_ptr<ASTNode> result = parseFactor();

            while (lexer->peek().type == TokenType::Add || lexer->peek().type == TokenType::Subtract) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::Add) ty = ASTBinaryOperator::Type::Add;
                else if (lexer->peek().type == TokenType::Subtract) ty = ASTBinaryOperator::Type::Subtract;
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseFactor());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseFactor() {
            if (!matchFactor().second) throw error;
            std::shared_ptr<ASTNode> result = parseConversion();

            while (lexer->peek().type == TokenType::Modulo || lexer->peek().type == TokenType::Multiply || lexer->peek().type == TokenType::Divide) {
                ASTBinaryOperator::Type ty;
                if (lexer->peek().type == TokenType::Modulo) ty = ASTBinaryOperator::Type::Modulo;
                else if (lexer->peek().type == TokenType::Multiply) ty = ASTBinaryOperator::Type::Multiply;
                else if (lexer->peek().type == TokenType::Divide) ty = ASTBinaryOperator::Type::Divide;
                lexer->eat();

                result = std::make_shared<ASTBinaryOperator>(ty, result, parseConversion());
                result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseConversion() {
            if (!matchConversion().second) throw error;
            std::shared_ptr<ASTNode> result = parseUnary();

            while (lexer->peek().type == TokenType::KwAs) {
                lexer->eat();

                result = std::make_shared<ASTConversion>(result, parseType());
                result->begin = (std::reinterpret_pointer_cast<ASTConversion>(result))->from->begin;
                result->end = lexer->peek().end;
            }

            return result;
        }


        std::shared_ptr<ASTNode> Parser::parseUnary() {
            if (!matchUnary().second) throw error;

            if (lexer->peek().type == TokenType::LogicalNot || lexer->peek().type == TokenType::BitNot || lexer->peek().type == TokenType::Add || lexer->peek().type == TokenType::Subtract || lexer->peek().type == TokenType::Multiply || lexer->peek().type == TokenType::BitXor) {
                ASTUnaryOperator::Type ty;
                if (lexer->peek().type == TokenType::LogicalNot) ty = ASTUnaryOperator::Type::LogicalNot;
                else if (lexer->peek().type == TokenType::BitNot) ty = ASTUnaryOperator::Type::BitNot;
                else if (lexer->peek().type == TokenType::Add) { lexer->eat(); return parseUnary(); }
                else if (lexer->peek().type == TokenType::Subtract) ty = ASTUnaryOperator::Type::Minus;
                else if (lexer->peek().type == TokenType::Multiply) ty = ASTUnaryOperator::Type::Dereference;
                else if (lexer->peek().type == TokenType::BitXor) ty = ASTUnaryOperator::Type::AddressOf;
                core::SourceLocation begin = lexer->peek().begin;
                lexer->eat();

                std::shared_ptr<ASTNode> second;

                MatchType c;
                if ((c = matchParen()).second) {
                    second = parseParen();
                } else if ((c = matchUnary()).second) {
                    second = parseUnary();
                }

                auto result = std::make_shared<ASTUnaryOperator>(ty, second);
                result->begin = begin;
                result->end = (std::reinterpret_pointer_cast<ASTUnaryOperator>(result))->node->end;

                begin = lexer->peek().begin;
                return result;
            } else if (matchCallSubscriptOrMember().second) {
                return parseCallSubscriptOrMember();
            }

            return {};
        }

        std::shared_ptr<ASTNode> Parser::parseCallSubscriptOrMember() {
            if (!matchCallSubscriptOrMember().second) throw error;
            std::shared_ptr<ASTNode> result = parseOne();

            while (lexer->peek().type == TokenType::LeftParen || lexer->peek().type == TokenType::LeftBracket || lexer->peek().type == TokenType::Dot) {
                if (lexer->peek().type == TokenType::LeftParen) {
                    core::SourceLocation begin = lexer->peek().begin;
                    lexer->eat(); // ()

                    std::vector<std::shared_ptr<ASTNode>> callArgs;

                    while (matchExpr().second) {
                        callArgs.push_back(parseExpr());

                        if (lexer->peek().type == TokenType::Comma) lexer->eat();
                    }

                    result = std::make_shared<ASTCall>(result, callArgs);
                    result->begin = (std::reinterpret_pointer_cast<ASTCall>(result))->called->begin;
                    result->end = lexer->peek().end;

                    lexer->eat(); // )
                } else if (lexer->peek().type == TokenType::LeftBracket) {
                    core::SourceLocation begin = lexer->peek().begin;

                    lexer->eat(); // [

                    result = std::make_shared<ASTSubscript>(result, parseExpr());
                    result->begin = (std::reinterpret_pointer_cast<ASTSubscript>(result))->indexed->begin;

                    lexer->eat(); // ]
                    result->end = lexer->peek().end;
                } else if (lexer->peek().type == TokenType::Dot) {
                    lexer->eat(); // .

                    auto n = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
                    n->begin = lexer->peek().begin;
                    n->end = lexer->peek().end;
                    lexer->eat();

                    result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::MemberResolution, result, n);
                    result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                    result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->end;
                }
            }

            return result;
        }

        std::shared_ptr<ASTNode> Parser::parseOne() {
            if (!matchOne().second) throw error;
            core::SourceLocation begin = lexer->peek().begin;

            if (matchParen().second) {
                return parseParen();
            } else if (lexer->peek().type == TokenType::Integer) {
                auto result = std::make_shared<ASTLiteral>(*(std::uint64_t*)&lexer->peek().litrl);
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Decimal) {
                auto result = std::make_shared<ASTLiteral>(*(double*)&lexer->peek().litrl);
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::String) {
                auto result = std::make_shared<ASTLiteral>(std::get<std::string>(lexer->peek().litrl));
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Character) {
                auto result = std::make_shared<ASTLiteral>(std::get<std::string>(lexer->peek().litrl), ASTLiteral::Type::Character);
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::KwTrue || lexer->peek().type == TokenType::KwFalse) {
                auto result = std::make_shared<ASTLiteral>(lexer->peek().type == TokenType::KwTrue);
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();
                return result;
            } else if (lexer->peek().type == TokenType::Name) {
                return parseName();
            }

            return {};
        }

        std::shared_ptr<ASTNode> Parser::parseName() {
            if (!matchName().second) throw error;

            if (lexer->peek().type == TokenType::Name) {
                std::shared_ptr<ASTNode> result = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
                result->begin = lexer->peek().begin;
                result->end = lexer->peek().end;

                lexer->eat();

                while (lexer->peek().type == TokenType::ColonColon) {
                    lexer->eat(); // :P

                    std::shared_ptr<ASTNode> second = std::make_shared<ASTLiteral>(lexer->peek().text, ASTLiteral::Type::Name);
                    second->begin = lexer->peek().begin;
                    second->end = lexer->peek().end;
                    lexer->eat();

                    result = std::make_shared<ASTBinaryOperator>(ASTBinaryOperator::Type::NamespaceResolution, result, second);
                    result->begin = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->left->begin;
                    result->end = (std::reinterpret_pointer_cast<ASTBinaryOperator>(result))->right->end;
                }

                return result;
            } else {
                return {};
            }
        }

        MatchType Parser::matchParen(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type != TokenType::LeftParen) return MatchType(p, false);
            ++p;

            c = matchExpr(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            if (lexer->peek(b + p).type != TokenType::RightParen) return MatchType(p, false);
            ++p;

            return MatchType(p, true);
        }

        MatchType Parser::matchExpr(std::size_t b) {
            return matchAssignment(b);
        }

        MatchType Parser::matchAssignment(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchLogicalOr(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            if (lexer->peek(b + p).type == TokenType::Equal || lexer->peek(b + p).type == TokenType::AddEqual || lexer->peek(b + p).type == TokenType::SubtractEqual || lexer->peek(b + p).type == TokenType::ModuloEqual || lexer->peek(b + p).type == TokenType::MultiplyEqual || lexer->peek(b + p).type == TokenType::DivideEqual || lexer->peek(b + p).type == TokenType::BitAndEqual || lexer->peek(b + p).type == TokenType::BitXorEqual || lexer->peek(b + p).type == TokenType::BitOrEqual || lexer->peek(b + p).type == TokenType::BitShiftLeftEqual || lexer->peek(b + p).type == TokenType::BitShiftRightEqual) {
                ++p;

                c = matchAssignment(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchLogicalOr(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchLogicalAnd(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::LogicalOr) {
                ++p;

                c = matchLogicalAnd(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchLogicalAnd(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchDirectComparison(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::LogicalAnd) {
                ++p;

                c = matchDirectComparison(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchDirectComparison(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchComparison(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::LogicalEqual || lexer->peek(b + p).type == TokenType::LogicalNotEqual) {
                ++p;

                c = matchComparison(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchComparison(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitOr(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::LogicalLessThan || lexer->peek(b + p).type == TokenType::LogicalLessThanEqual || lexer->peek(b + p).type == TokenType::LogicalGreaterThan || lexer->peek(b + p).type == TokenType::LogicalGreaterThanEqual) {
                ++p;

                c = matchBitOr(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitOr(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitXor(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::BitOr) {
                ++p;

                c = matchBitXor(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitXor(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitAnd(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::BitXor) {
                ++p;

                c = matchBitAnd(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitAnd(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchBitShift(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::BitAnd) {
                ++p;

                c = matchBitShift(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchBitShift(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchTerm(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::BitShiftLeft || lexer->peek(b + p).type == TokenType::BitShiftRight) {
                ++p;

                c = matchTerm(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchTerm(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchFactor(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::Add || lexer->peek(b + p).type == TokenType::Subtract) {
                ++p;

                c = matchFactor(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchFactor(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchConversion(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::Modulo || lexer->peek(b + p).type == TokenType::Multiply || lexer->peek(b + p).type == TokenType::Divide) {
                ++p;

                c = matchConversion(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchConversion(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchUnary(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::KwAs) {
                ++p;

                c = matchType(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchUnary(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if (lexer->peek(b + p).type == TokenType::LogicalNot || lexer->peek(b + p).type == TokenType::BitNot || lexer->peek(b + p).type == TokenType::Add || lexer->peek(b + p).type == TokenType::Subtract || lexer->peek(b + p).type == TokenType::Multiply || lexer->peek(b + p).type == TokenType::BitXor) {
                ++p;

                c = matchUnary(b + p);
                if (!c.second && !(c = matchParen()).second) return MatchType(p, false);
                p += c.first;
            } else {
                c = matchCallSubscriptOrMember(b + p);
                if (!c.second) return MatchType(p + c.first, false);
                p += c.first;
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchCallSubscriptOrMember(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            c = matchOne(b + p);
            if (!c.second) return MatchType(p + c.first, false);
            p += c.first;

            while (lexer->peek(b + p).type == TokenType::LeftParen || lexer->peek(b + p).type == TokenType::LeftBracket || lexer->peek(b + p).type == TokenType::Dot) {
                if (lexer->peek(b + p).type == TokenType::LeftParen) {
                    ++p;

                    while (lexer->peek(b + p).type != TokenType::RightParen) {
                        c = matchExpr(b + p);
                        if (!c.second) return MatchType(p + c.first, false);
                        p += c.first;

                        if (lexer->peek(b + p).type != TokenType::Comma && lexer->peek(b + p).type != TokenType::RightParen) {
                            error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ',' or ')'.");
                            return MatchType(p, false);
                        }

                        if (lexer->peek(b + p).type == TokenType::Comma) {
                            ++p;
                        }
                    }

                    ++p;
                } else if (lexer->peek(b + p).type == TokenType::LeftBracket) {
                    ++p;

                    c = matchExpr(b + p);
                    if (!c.second) return MatchType(p + c.first, false);
                    p += c.first;

                    if (lexer->peek(b + p).type != TokenType::RightBracket) {
                        error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected ']'.");
                        return MatchType(p, false);
                    }

                    ++p;
                } else if (lexer->peek(b + p).type == TokenType::Dot) {
                    ++p;

                    if (lexer->peek(b + p).type != TokenType::Name) {
                        error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected identifier.");
                        return MatchType(p, false);
                    }

                    ++p;
                }
            }

            return MatchType(p, true);
        }

        MatchType Parser::matchOne(std::size_t b) {
            std::size_t p = 0;
            MatchType c;

            if ((c = matchParen(b + p)).second) {
                p += c.first;
                return MatchType(p, true);
            } else if (lexer->peek(b + p).type == TokenType::Integer) {
                return MatchType(++p, true);
            } else if (lexer->peek(b + p).type == TokenType::Decimal) {
                return MatchType(++p, true);
            } else if (lexer->peek(b + p).type == TokenType::String) {
                return MatchType(++p, true);
            } else if (lexer->peek(b + p).type == TokenType::Character) {
                if (std::get<std::string>(lexer->peek(b + p).litrl).size() < 1) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "invalid character literal.");
                    return MatchType(p, false);
                }

                return MatchType(++p, true);
            } else if (lexer->peek(b + p).type == TokenType::KwTrue || lexer->peek(b + p).type == TokenType::KwFalse) {
                return MatchType(++p, true);
            } else if (lexer->peek(b + p).type == TokenType::Name) {
                return matchName(b + p);
            } else {
                error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, fmt::format("unexpected '{:.{}}'.", lexer->peek(b + p).text.data(), lexer->peek(b + p).text.size()));
                return MatchType(p, false);
            }
        }

        MatchType Parser::matchName(std::size_t b) {
            std::size_t p = 0;
            if (lexer->peek(b + p).type != TokenType::Name) return MatchType(p, false);
            ++p;

            while (lexer->peek(b + p).type == TokenType::ColonColon) {
                ++p;

                if (lexer->peek(b + p).type != TokenType::Name) {
                    error = core::Error(core::Error::Type::Syntactic, lexer->source, lexer->peek(b + p).begin, lexer->peek(b + p).end, "expected identifier.");
                    return MatchType(p, false);
                }

                ++p;
            }

            return MatchType(p, true);
        }
    }
}
