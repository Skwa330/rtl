#ifndef RTL_PARSER_PARSER_H
#define RTL_PARSER_PARSER_H

#include "rtl/Parser/Lexer.h"
#include "rtl/Parser/AST.h"

#include "rtl/Core/Error.h"

#include <utility>

/*
      rtl operator precedence -
      higher precedence = lower numbers = more priority

      0 - integer, decimal, string, char name
      1 - function call, subscript or member access
      2 - !, ~, ^, *,
*/

namespace rtl {
    namespace parser {
        using MatchType = std::pair<std::size_t, bool>;

        class Parser {
        private:
            std::vector<std::shared_ptr<ASTNode>>& nodes;
            std::unique_ptr<Lexer> lexer;

            core::Error error; // This is just how we save errors in the matching functions so we can actually report the correct thing. :)
        public:
            Parser(std::vector<std::shared_ptr<ASTNode>>& nodes);

            void initFromSource(const std::string &moduleName, const std::string &source);
            void initFromFile(const std::string &filepath);

            const std::unique_ptr<Lexer>& getLexer() const;

            void parseSyntaxTree();
            MatchType matchSyntaxTree(std::size_t b = 0);

            std::shared_ptr<ASTNode> parseTopLevel(); // This is used so that we aren't copying code for things like namespaces
            MatchType matchTopLevel(std::size_t b = 0);

            Type parseType();
            MatchType matchType(std::size_t b = 0);

            std::shared_ptr<ASTVariableDeclaration> parseVariableDeclaration();
            MatchType matchVariableDeclaration(std::size_t b = 0);

            std::shared_ptr<ASTVariableDefinition> parseVariableDefinition();
            MatchType matchVariableDefinition(std::size_t b = 0);

            std::shared_ptr<ASTNode> parseStatement();
            MatchType matchStatement(std::size_t b = 0);

            std::shared_ptr<ASTBlock> parseBlock();
            MatchType matchBlock(std::size_t b = 0);

            std::shared_ptr<ASTReturn> parseReturn();
            MatchType matchReturn(std::size_t b = 0);

            std::shared_ptr<ASTBreak> parseBreak();
            MatchType matchBreak(std::size_t b = 0);

            std::shared_ptr<ASTContinue> parseContinue();
            MatchType matchContinue(std::size_t b = 0);

            std::shared_ptr<ASTFor> parseFor();
            MatchType matchFor(std::size_t b = 0);

            std::shared_ptr<ASTRange> parseRange();
            MatchType matchRange(std::size_t b = 0);

            std::shared_ptr<ASTWhile> parseWhile();
            MatchType matchWhile(std::size_t b = 0);

            std::shared_ptr<ASTIf> parseIf();
            MatchType matchIf(std::size_t b = 0);

            std::shared_ptr<ASTFunctionHeader> parseFunction();
            MatchType matchFunction(std::size_t b = 0);

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
            std::shared_ptr<ASTNode> parseConversion();
            std::shared_ptr<ASTNode> parseUnary();
            std::shared_ptr<ASTNode> parseCallSubscriptOrMember();
            std::shared_ptr<ASTNode> parseOne();

            std::shared_ptr<ASTNode> parseName();

            MatchType matchParen(std::size_t b = 0);

            MatchType matchExpr(std::size_t b = 0);

            MatchType matchAssignment(std::size_t b = 0);
            MatchType matchLogicalOr(std::size_t b = 0);
            MatchType matchLogicalAnd(std::size_t b = 0);
            MatchType matchDirectComparison(std::size_t b = 0);
            MatchType matchComparison(std::size_t b = 0);
            MatchType matchBitOr(std::size_t b = 0);
            MatchType matchBitXor(std::size_t b = 0);
            MatchType matchBitAnd(std::size_t b = 0);
            MatchType matchBitShift(std::size_t b = 0);
            MatchType matchTerm(std::size_t b = 0);
            MatchType matchFactor(std::size_t b = 0);
            MatchType matchConversion(std::size_t b = 0);
            MatchType matchUnary(std::size_t b = 0);
            MatchType matchCallSubscriptOrMember(std::size_t b = 0);
            MatchType matchOne(std::size_t b = 0);

            MatchType matchName(std::size_t b = 0);
        };
    }
}

#endif /* RTL_PARSER_PARSER_H */
