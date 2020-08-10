#ifndef RLT_PARSER_PARSER_H
#define RLT_PARSER_PARSER_H

#include "rlt/Parser/Lexer.h"
#include "rlt/Parser/AST.h"

#include "rlt/Core/Error.h"

#include <utility>

namespace rlt {
    namespace parser {
        struct Types {
            std::shared_ptr<ASTBuiltinType> autoType; // Used for inference
            std::shared_ptr<ASTBuiltinType> noneType;
            std::shared_ptr<ASTBuiltinType> boolType;

            std::shared_ptr<ASTBuiltinType> i8Type;
            std::shared_ptr<ASTBuiltinType> i16Type;
            std::shared_ptr<ASTBuiltinType> i32Type;
            std::shared_ptr<ASTBuiltinType> i64Type;

            std::shared_ptr<ASTBuiltinType> u8Type;
            std::shared_ptr<ASTBuiltinType> u16Type;
            std::shared_ptr<ASTBuiltinType> u32Type;
            std::shared_ptr<ASTBuiltinType> u64Type;

            std::shared_ptr<ASTBuiltinType> f32Type;
            std::shared_ptr<ASTBuiltinType> f64Type;
        };

        using MatchType = std::pair<std::size_t, bool>;

        class Parser {
        private:
            std::vector<std::shared_ptr<ASTNode>>& nodes;
            std::unique_ptr<Lexer> lexer;
            std::shared_ptr<Types> types;

            core::Error error; // This is just how we save errors in the matching functions so we can actually report the correct thing. :)
        public:
            Parser(std::vector<std::shared_ptr<ASTNode>>& nodes);

            void initFromSource(const std::string &moduleName, const std::string &source);
            void initFromFile(const std::string &filepath);

            const std::unique_ptr<Lexer>& getLexer() const;
            const std::shared_ptr<Types>& getTypes() const;

            void parseSyntaxTree();

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

#endif /* RLT_PARSER_PARSER_H */
