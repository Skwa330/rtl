#ifndef RTL_PARSER_LEXER_H
#define RTL_PARSER_LEXER_H

#include <utility>
#include <variant>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

#include "rtl/Core/Error.h"
#include "rtl/Core/SourceLocation.h"

namespace rtl {
    namespace parser {
        enum class TokenType {
            Invalid,
            Eoi,

            LeftParen,
            RightParen,

            LeftBracket,
            RightBracket,

            LeftBrace,
            RightBrace,

            Dot,
            DotDot,

            Comma,

            Dollar,

            Arrow,
            BigArrow,

            SemiColon,

            Colon,
            ColonColon,

            Add,
            AddEqual,

            Subtract,
            SubtractEqual,

            Modulo,
            ModuloEqual,

            Multiply,
            MultiplyEqual,

            Divide,
            DivideEqual,

            BitAnd,
            BitAndEqual,

            BitXor,
            BitXorEqual,

            BitOr,
            BitOrEqual,

            BitShiftLeft,
            BitShiftLeftEqual,

            BitShiftRight,
            BitShiftRightEqual,

            BitNot,

            Equal,

            LogicalAnd,
            LogicalOr,

            LogicalEqual,

            LogicalNot,
            LogicalNotEqual,

            LogicalLessThan,
            LogicalLessThanEqual,
            LogicalGreaterThan,
            LogicalGreaterThanEqual,

            Integer,
            Decimal,
            String,
            Character,

            Name,

            KwVal,
            KwVar,

            KwAny,

            KwNone,

            KwBool,
            KwTrue,
            KwFalse,

            KwI8,
            KwI16,
            KwI32,
            KwI64,

            KwU8,
            KwU16,
            KwU32,
            KwU64,

            KwF32,
            KwF64,

            KwAs,

            KwImport,
            KwNamespace,

            KwFun,
            KwReturn,

            KwIf,
            KwElif,
            KwElse,

            KwWhile,
            KwFor,

            KwContinue,
            KwBreak,

            KwSwitch,

            KwStruct,
            KwEnum,
            KwUnion,

            KwSizeOf
        };

        struct Token {
            TokenType type;
            core::SourceLocation begin, end;

            std::string_view text;

            std::variant < std::string, std::uint64_t, double > litrl; // we have to have std::string here because when we peek multiple tokens ahead the textBuffer gets messed up :()
        };

        class Lexer {
        private:
            char textBuffer[8192];

            std::vector < Token > tokens;

            void next();
            void skip();

            void once();
        public:
            core::SourceLocation sourceLocation;
            std::string source;

            void initFromSource(const std::string & moduleName, const std::string & source);
            void initFromFile(const std::string & filepath);
            // We are using a std::string ^^ here because the filepath has to be null-terminated

            const Token & peek(std::size_t count = 0);
            void eat(std::size_t count = 1);
        };
    }
}

#endif /* RTL_PARSER_LEXER_H */