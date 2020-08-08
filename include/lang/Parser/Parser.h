#ifndef LANG_PARSER_PARSER_H
#define LANG_PARSER_PARSER_H

#include "lang/parser/Lexer.h"
#include "lang/parser/AST.h"

#include <tuple>

namespace lang {
    namespace parser {
        using MatchType = std::pair<std::size_t, bool>;

        struct Types {
            std::shared_ptr<TypeDeclaration> booleanType;

            std::shared_ptr<TypeDeclaration> int8Type;
            std::shared_ptr<TypeDeclaration> int16Type;
            std::shared_ptr<TypeDeclaration> int32Type;
            std::shared_ptr<TypeDeclaration> int64Type;

            std::shared_ptr<TypeDeclaration> uint8Type;
            std::shared_ptr<TypeDeclaration> uint16Type;
            std::shared_ptr<TypeDeclaration> uint32Type;
            std::shared_ptr<TypeDeclaration> uint64Type;

            std::shared_ptr<TypeDeclaration> float32Type;
            std::shared_ptr<TypeDeclaration> float64Type;
        };

        class Parser {
        private:
            std::vector<std::shared_ptr<ASTNode>>& nodes;
            std::unique_ptr<Lexer> lexer;
            std::shared_ptr<Types> types;
        public:
            Parser(std::vector<std::shared_ptr<ASTNode>>& nodes);

            void initFromSource(const std::string &moduleName, const std::string &source);
            void initFromFile(const std::string &filepath);

            const std::unique_ptr<Lexer>& getLexer() const;
            const std::shared_ptr<Types>& getTypes() const;

            void parseSyntaxTree();

            std::shared_ptr<ASTNode> parse();
            MatchType match(std::size_t beg = 0);

            Type parseType();
            MatchType matchType(std::size_t beg = 0);

            std::shared_ptr<ASTVariableDeclaration> parseVariableDeclaration();
            MatchType matchVariableDeclaration(std::size_t beg = 0);

            std::shared_ptr<ASTNode> parseStatement();
            MatchType matchStatement(std::size_t beg = 0);

            std::shared_ptr<ASTBlock> parseBlock();
            MatchType matchBlock(std::size_t beg = 0);

            std::shared_ptr<ASTIf> parseIf();
            MatchType matchIf(std::size_t beg = 0);

            std::shared_ptr<ASTFunctionHeader> parseFunction();
            MatchType matchFunction(std::size_t beg = 0);

            std::shared_ptr<ASTNode> parseParen();

            std::shared_ptr<ASTNode> parseExpr();

            std::shared_ptr<ASTNode> parseAssignment();
            std::shared_ptr<ASTNode> parseLogicalOr();
            std::shared_ptr<ASTNode> parseLogicalAnd();
            std::shared_ptr<ASTNode> parseDirectComparison();
            std::shared_ptr<ASTNode> parseComparison();
            std::shared_ptr<ASTNode> parseBitOr();
            std::shared_ptr<ASTNode> parseBitXor();
            std::shared_ptr<ASTNode> parseBitAnd();
            std::shared_ptr<ASTNode> parseBitShift();
            std::shared_ptr<ASTNode> parseTerm();
            std::shared_ptr<ASTNode> parseFactor();
            std::shared_ptr<ASTNode> parseUnary();
            std::shared_ptr<ASTNode> parseCallSubscriptOrMember();
            std::shared_ptr<ASTNode> parseOne();

            std::shared_ptr<ASTNode> parseName();

            MatchType matchParen(std::size_t beg = 0);

            MatchType matchExpr(std::size_t beg = 0);

            MatchType matchAssignment(std::size_t beg = 0);
            MatchType matchLogicalOr(std::size_t beg = 0);
            MatchType matchLogicalAnd(std::size_t beg = 0);
            MatchType matchDirectComparison(std::size_t beg = 0);
            MatchType matchComparison(std::size_t beg = 0);
            MatchType matchBitOr(std::size_t beg = 0);
            MatchType matchBitXor(std::size_t beg = 0);
            MatchType matchBitAnd(std::size_t beg = 0);
            MatchType matchBitShift(std::size_t beg = 0);
            MatchType matchTerm(std::size_t beg = 0);
            MatchType matchFactor(std::size_t beg = 0);
            MatchType matchUnary(std::size_t beg = 0);
            MatchType matchCallSubscriptOrMember(std::size_t beg = 0);
            MatchType matchOne(std::size_t beg = 0);

            MatchType matchName(std::size_t beg = 0);
        };
    }
}

#endif /* LANG_PARSER_PARSER_H */
