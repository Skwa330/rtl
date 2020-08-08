#ifndef FOSL_PARSER_LEXER_H
#define FOSL_PARSER_LEXER_H

#include <utility> // for std::pair
#include <type_traits> // for std::aligned_union
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace fosl {
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
            std::string_view moduleName;
            std::uint32_t line, lexpos;
            std::size_t length;

            std::string_view text;

            std::aligned_union<0, std::uint64_t, double>::type litrl;
        };

        class Lexer {
        private:
            std::uint32_t line = 1, lexpos = 1;
            std::size_t pointer = 0;

            std::size_t step = 0;

            std::string moduleName;
            std::string source;

            char textBuffer[8192];

            std::vector<Token> tokens;

            void next();
            void skip();

            void once();
        public:
            void initFromSource(const std::string &moduleName, const std::string_view &source);
            void initFromFile(const std::string &filepath);

            const Token &peek(std::size_t count = 0);
            void eat(std::size_t count = 1);

            void setLine(std::uint32_t line);
            void setLexpos(std::uint32_t lexpos);

            void setPointer(std::size_t pointer);

            void setStep(std::size_t step);

            void setModuleName(const std::string &moduleName);
            void setSource(const std::string_view &source);

            std::uint32_t getLine() const;
            std::uint32_t getLexpos() const;

            std::size_t getPointer() const;

            std::size_t getStep() const;

            const std::string &getModuleName() const;
            const std::string &getSource() const;
        };
    }
}

#endif /* _KNW_PARSE_LEXER_H_ */
