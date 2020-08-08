#ifndef LANG_PARSER_LEXER_H
#define LANG_PARSER_LEXER_H

#include <utility>
#include <type_traits>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

#include "SourceLocation.h"

namespace lang {
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

            KwBoolean,
            KwTrue,
            KwFalse,

            KwInt8,
            KwInt16,
            KwInt32,
            KwInt64,

            KwUInt8,
            KwUInt16,
            KwUInt32,
            KwUInt64,

            KwFloat32,
            KwFloat64,

            KwImport,
            KwNamespace,
            KwPrivate,

            KwFun,

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
            SourceLocation location;
            std::size_t length;

            std::string_view text;

            std::aligned_union < 0, std::uint64_t, double > ::type litrl;
        };

        class Lexer {
        private:
            char textBuffer[8192];

            std::vector < Token > tokens;

            void next();
            void skip();

            void once();
        public:
            SourceLocation sourceLocation;
            std::string source;

            void initFromSource(const std::string & moduleName, const std::string_view & source);
            void initFromFile(const std::string & filepath);
            // We are using a std::string ^^ here because the filepath has to be null-terminated

            const Token & peek(std::size_t count = 0);
            void eat(std::size_t count = 1);
        };
    }
}

#endif /* LANG_PARSER_LEXER_H */