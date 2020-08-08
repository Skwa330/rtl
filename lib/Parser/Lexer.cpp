#include "fosl/parser/Lexer.h"
#include <fmt/format.h>

#include "fosl/Error.h"

#include <cctype>

namespace fosl {
    namespace parser {
        void Lexer::next() {
            if (source[pointer++] == '\n') {
                ++line;
                lexpos = 1;
            } else {
                ++lexpos;
            }
        }

        void Lexer::skip() {
            for (;;) {
                while (pointer < source.size() && std::isspace(source[pointer])) {
                    next();
                }

                if (pointer < source.size() && source[pointer] == '#') {
                    while (pointer < source.size() && source[pointer] != '\n') {
                        next();
                    }

                    continue;
                }

                if (pointer + 1 < source.size() && source[pointer] == '/' && source[pointer + 1] == '#') {
                    std::string_view moduleNameStart = moduleName;
                    std::uint32_t lineStart = line, lexposStart = lexpos;

                    next();
                    next();

                    std::size_t balance = 1;

                    while (pointer < source.size() && balance) {
                        if (pointer + 1 < source.size() && source[pointer] == '/' && source[pointer + 1] == '#') {
                            next();
                            next();

                            ++balance;
                        } else if (pointer + 1 < source.size() && source[pointer] == '#' && source[pointer + 1] == '/') {
                            next();
                            next();

                            --balance;
                        } else {
                            next();
                        }
                    }

                    if (balance) {
                        throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleNameStart, lineStart, lexposStart, "unterminated comment.");
                    }

                    continue;
                }

                break;
            }
        }

        void Lexer::once() {
            skip();

            Token token;
            token.moduleName = moduleName;
            token.line = line;
            token.lexpos = lexpos;
            token.length = 0;
            token.text = std::string_view(token.text.data(), 0);
            std::size_t start = pointer;

            if (pointer >= source.size() || !source[pointer]) {
                token.type = TokenType::Eoi;
            } else {
                switch (source[pointer]) {
                    case '(': {
                        next();
                        token.type = TokenType::LeftParen;
                        break;
                    }

                    case ')': {
                        next();
                        token.type = TokenType::RightParen;
                        break;
                    }

                    case '[': {
                        next();
                        token.type = TokenType::LeftBracket;
                        break;
                    }

                    case ']': {
                        next();
                        token.type = TokenType::RightBracket;
                        break;
                    }

                    case '{': {
                        next();
                        token.type = TokenType::LeftBrace;
                        break;
                    }

                    case '}': {
                        next();
                        token.type = TokenType::RightBrace;
                        break;
                    }

                    case '.': {

                        next();

                        if (pointer < source.size() && source[pointer] == '.') {
                            next();
                            token.type = TokenType::DotDot;
                            break;
                        }

                        token.type = TokenType::Dot;
                        break;
                        case ',':
                        next();
                        token.type = TokenType::Comma;
                        break;
                        case ':':
                        next();
                        if (pointer < source.size() && source[pointer] == ':') {
                            next();
                            token.type = TokenType::ColonColon;
                            break;
                        }
                        token.type = TokenType::Colon;
                        break;
                    }

                    case '+': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::AddEqual;
                            break;
                        }
                        token.type = TokenType::Add;
                        break;
                    }

                    case '-': {
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '>') {
                                next();
                                token.type = TokenType::Arrow;
                                break;
                            } else if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::SubtractEqual;
                                break;
                            }
                        }
                        token.type = TokenType::Subtract;
                        break;
                    }

                    case '%': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::ModuloEqual;
                            break;
                        }
                        token.type = TokenType::Modulo;
                        break;
                    }

                    case '*': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::MultiplyEqual;
                            break;
                        }
                        token.type = TokenType::Multiply;
                        break;
                    }

                    case '/': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::DivideEqual;
                            break;
                        }
                        token.type = TokenType::Divide;
                        break;
                    }

                    case '&': {
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::BitAndEqual;
                                break;
                            } else if (source[pointer] == '&') {
                                next();
                                token.type = TokenType::LogicalAnd;
                                break;
                            }
                        }
                        token.type = TokenType::BitAnd;
                        break;
                    }

                    case '^': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::BitXorEqual;
                            break;
                        }
                        token.type = TokenType::BitXor;
                        break;
                    }

                    case '|': {
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::BitOrEqual;
                                break;
                            } else if (source[pointer] == '|') {
                                next();
                                token.type = TokenType::LogicalOr;
                                break;
                            }
                        }
                        token.type = TokenType::BitOr;
                        break;
                    }

                    case '<': {
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '<') {
                                next();
                                if (pointer < source.size() && source[pointer] == '=') {
                                    next();
                                    token.type = TokenType::BitShiftLeftEqual;
                                    break;
                                }
                                token.type = TokenType::BitShiftLeft;
                                break;
                            } else if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::LogicalLessThanEqual;
                                break;
                            }
                        }
                        token.type = TokenType::LogicalLessThan;
                        break;
                    }

                    case '>': {
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '>') {
                                next();
                                if (pointer < source.size() && source[pointer] == '=') {
                                    next();
                                    token.type = TokenType::BitShiftRightEqual;
                                    break;
                                }
                                token.type = TokenType::BitShiftRight;
                                break;
                            } else if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::LogicalGreaterThanEqual;
                                break;
                            }
                        }
                        token.type = TokenType::LogicalGreaterThan;
                        break;
                    }

                    case '~': {
                        next();
                        token.type = TokenType::BitNot;
                        break;
                        case '=':
                        next();
                        if (pointer < source.size()) {
                            if (source[pointer] == '>') {
                                next();
                                token.type = TokenType::BigArrow;
                                break;
                            } else if (source[pointer] == '=') {
                                next();
                                token.type = TokenType::LogicalEqual;
                                break;
                            }
                        }
                        token.type = TokenType::Equal;
                        break;
                    }

                    case '!': {
                        next();
                        if (pointer < source.size() && source[pointer] == '=') {
                            next();
                            token.type = TokenType::LogicalNotEqual;
                            break;
                        }
                        token.type = TokenType::LogicalNot;
                        break;
                    }

                    case '\'':
                    case '"': {
                        char delim = source[pointer];

                        std::string_view moduleNameStart = moduleName;
                        std::uint32_t lineStart = line, lexposStart = lexpos;

                        next();
                        std::size_t length = 0;
                        while (pointer < source.size() && source[pointer] != delim) {
                            if (source[pointer] == '\\') {
                                next();

                                switch (source[pointer]) {
                                    case 'f': {
                                        next();
                                        textBuffer[length++] = '\f';
                                        break;
                                    }

                                    case 'n': {
                                        next();
                                        textBuffer[length++] = '\n';
                                        break;
                                    }

                                    case 'r': {
                                        next();
                                        textBuffer[length++] = '\r';
                                        break;
                                    }

                                    case 'v': {
                                        next();
                                        textBuffer[length++] = '\v';
                                        break;
                                    }

                                    case 'x': {
                                        next();
                                        if (pointer + 1 >= source.size()) {
                                            throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, "\\x must be followed by exactly two hex digits.");
                                        }

                                        std::uint8_t hex = 0;
                                        for (std::size_t i = 0; i < 2; i++) {
                                            hex *= 16;

                                            if (std::tolower(source[pointer]) >= 'a' && std::tolower(source[pointer]) <= 'f') {
                                                hex += 10 + (source[pointer] - 'a');
                                            } else if (source[pointer] >= '0' && source[pointer] <= '9') {
                                                hex += source[pointer] - '0';
                                            } else {
                                                throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, fmt::format("invalid hex digit '{}'.", source[pointer]));
                                            }

                                            next();
                                        }

                                        textBuffer[length++] = *(char*)&hex;
                                        break;
                                    }

                                    case 'u': {
                                        next();
                                        if (pointer + 1 >= source.size()) {
                                            throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, "\\u must be followed by exactly two hex digits.");
                                        }

                                        std::uint16_t hex = 0;
                                        for (std::size_t i = 0; i < 4; i++) {
                                            hex *= 16;

                                            if (std::tolower(source[pointer]) >= 'a' && std::tolower(source[pointer]) <= 'f') {
                                                hex += 10 + (source[pointer] - 'a');
                                            } else if (source[pointer] >= '0' && source[pointer] <= '9') {
                                                hex += source[pointer] - '0';
                                            } else {
                                                throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, fmt::format("invalid hex digit '{}'.", source[pointer]));
                                            }

                                            next();
                                        }

                                        textBuffer[length++] = *(char*)&hex;
                                        break;
                                    }

                                    case 'U': {
                                        next();
                                        if (pointer + 1 >= source.size()) {
                                            throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, "\\U must be followed by exactly two hex digits.");
                                        }

                                        std::uint32_t hex = 0;
                                        for (std::size_t i = 0; i < 8; i++) {
                                            hex *= 16;

                                            if (std::tolower(source[pointer]) >= 'a' && std::tolower(source[pointer]) <= 'f') {
                                                hex += 10 + (source[pointer] - 'a');
                                            } else if (source[pointer] >= '0' && source[pointer] <= '9') {
                                                hex += source[pointer] - '0';
                                            } else {
                                                throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, fmt::format("invalid hex digit '{}'.", source[pointer]));
                                            }

                                            next();
                                        }

                                        textBuffer[length++] = *(char*)&hex;
                                        break;
                                    }

                                    default: {
                                        char c = source[pointer];
                                        next();
                                        textBuffer[length++] = c;
                                        break;
                                    }
                                }
                            } else {
                                textBuffer[length++] = source[pointer];
                                next();
                            }
                        }

                        if (pointer >= source.size() || source[pointer] != delim) {
                            throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleNameStart, lineStart, lexposStart, "unterminated literal.");
                        }

                        next();
                        token.text = std::string_view(textBuffer, length);
                        if (delim == '"') {
                            token.type = TokenType::String;
                        } else if (delim == '\'') {
                            token.type = TokenType::Character;
                        }

                        break;
                    }

                    default:
                    if (std::isalpha(source[pointer]) || source[pointer] == '_') {
                        std::size_t begin = pointer;

                        while (pointer < source.size() && (std::isalnum(source[pointer]) || source[pointer] == '_')) {
                            next();
                        }

                        auto is = [&](const char *s) {
                            return std::memcmp(textBuffer, s, token.text.size() * sizeof(char)) == 0;
                        };

                        switch (token.text.size()) {
                            case 2: {
                                if (is("if")) token.type = TokenType::KwIf;
                                else if (is("for")) token.type = TokenType::KwFor;
                                else goto name;

                                break;
                            }

                            case 3: {
                                if (is("val")) token.type = TokenType::KwVal;
                                else if (is("var")) token.type = TokenType::KwVar;
                                else if (is("fun")) token.type = TokenType::KwFun;
                                else if (is("any")) token.type = TokenType::KwAny;
                                else if (is("int")) token.type = TokenType::KwInt64;
                                else goto name;

                                break;
                            }

                            case 4: {
                                if (is("true")) token.type = TokenType::KwTrue;
                                else if (is("int8")) token.type = TokenType::KwInt8;
                                else if (is("elif")) token.type = TokenType::KwElif;
                                else if (is("else")) token.type = TokenType::KwElse;
                                else if (is("enum")) token.type = TokenType::KwEnum;
                                else goto name;

                                break;
                            }

                            case 5: {
                                if (is("false")) token.type = TokenType::KwFalse;
                                else if (is("int16")) token.type = TokenType::KwInt16;
                                else if (is("int32")) token.type = TokenType::KwInt32;
                                else if (is("int64")) token.type = TokenType::KwInt64;
                                else if (is("uint8")) token.type = TokenType::KwUInt8;
                                else if (is("float")) token.type = TokenType::KwFloat32;
                                else if (is("while")) token.type = TokenType::KwWhile;
                                else if (is("break")) token.type = TokenType::KwBreak;
                                else if (is("union")) token.type = TokenType::KwUnion;
                                else goto name;

                                break;
                            }

                            case 6: {
                                if (is("uint16")) token.type = TokenType::KwUInt16;
                                else if (is("uint32")) token.type = TokenType::KwUInt32;
                                else if (is("uint64")) token.type = TokenType::KwUInt64;
                                else if (is("import")) token.type = TokenType::KwImport;
                                else if (is("switch")) token.type = TokenType::KwSwitch;
                                else if (is("struct")) token.type = TokenType::KwStruct;
                                else goto name;

                                break;
                            }

                            case 7: {
                                if (is("boolean")) token.type = TokenType::KwBoolean;
                                else if (is("float32")) token.type = TokenType::KwFloat32;
                                else if (is("float64")) token.type = TokenType::KwFloat64;
                                else if (is("size_of")) token.type = TokenType::KwSizeOf;
                                else if (is("private")) token.type = TokenType::KwPrivate;
                                else goto name;

                                break;
                            }

                            case 8: {
                                if (is("continue")) token.type = TokenType::KwContinue;
                                else goto name;

                                break;
                            }

                            case 9: {
                                if (is("namespace")) token.type = TokenType::KwNamespace;
                                else goto name;

                                break;
                            }

                            default: {
                                goto name;
                            }
                        }

                        break;

                        name:
                        token.type = TokenType::Name;
                        break;
                    } else if (std::isdigit(source[pointer])) {
                        std::uint64_t &integer = *(std::uint64_t*)(&token.litrl);
                        double &decimal = *(double*)(&token.litrl);

                        integer = 0;

                        if (pointer + 2 < source.size() && source[pointer] == '0' && std::tolower(source[pointer + 1]) == 'x' && std::isxdigit(source[pointer + 2])) {
                            next();
                            next();

                            while (pointer < source.size() && std::isxdigit(source[pointer])) {
                                integer *= 16;

                                if (std::isalpha(source[pointer])) {
                                    integer += 10 + (std::tolower(source[pointer]) - 'a');
                                } else if (std::isdigit(source[pointer])) {
                                    integer += source[pointer] - '0';
                                }

                                next();
                            }

                            token.type = TokenType::Integer;
                        } else if (pointer + 2 < source.size() && source[pointer] == '0' && std::tolower(source[pointer + 1]) == 'o' && (source[pointer + 2] >= '0' && source[pointer + 2] <= '7')) {
                            next();
                            next();

                            while (pointer < source.size() && (source[pointer] >= '0' && source[pointer] <= '7')) {
                                integer *= 8;
                                integer += source[pointer] - '0';

                                next();
                            }

                            token.type = TokenType::Integer;
                        } else if (pointer + 2 < source.size() && source[pointer] == '0' && std::tolower(source[pointer + 1] == 'b') && (source[pointer + 2] == '0' || source[pointer + 2] == '1')) {
                            next();
                            next();

                            while (pointer < source.size() && (source[pointer] == '0' || source[pointer] == '1')) {
                                integer *= 2;
                                integer += source[pointer] - '0';

                                next();
                            }

                            token.type = TokenType::Integer;
                        } else {
                            while (pointer < source.size() && std::isdigit(source[pointer])) {
                                integer *= 10;
                                integer += source[pointer] - '0';
                                next();
                            }

                            if (pointer + 1 < source.size() && source[pointer] == '.' && std::isdigit(source[pointer + 1])) {
                                next();
                                double fractional = 0, weight = 1;

                                while (pointer < source.size() && std::isdigit(source[pointer])) {
                                    weight /= 10;
                                    fractional += (source[pointer] - '0') * weight;
                                    next();
                                }

                                decimal = (double)integer + fractional;
                                token.type = TokenType::Decimal;
                            } else {
                                token.type = TokenType::Integer;
                            }

                            break;
                        }
                    } else {
                        throw Error(Error::Type::Lexical, Error::Priority::Fatal, moduleName, line, lexpos, "invalid token");
                        break;
                    }
                }
            }

            token.length = pointer - start;
            if (!token.text.size()) token.text = std::string_view(token.text.data(), token.length);
            if (!token.text.data())
                token.text = std::string_view(&source[start], token.text.size());
            tokens.push_back(token);
        }

        void Lexer::initFromSource(const std::string &moduleName, const std::string_view &source) {
            this->moduleName = moduleName;
            this->source = source;
        }

        void Lexer::initFromFile(const std::string &filepath) {
            std::FILE *file = std::fopen(filepath.c_str(), "rt");
            std::fseek(file, 0, SEEK_END);
            std::size_t size = std::ftell(file);
            std::rewind(file);

            char *buffer = new char[size];
            std::memset(buffer, 0, size * sizeof(char));
            std::fread(buffer, 1, size, file);

            initFromSource(filepath, buffer);

            delete[] buffer;
        }

        const Token &Lexer::peek(std::size_t count) {
            while (step + count >= tokens.size()) {
                once();
            }

            return tokens[step + count];
        }

        void Lexer::eat(std::size_t count) {
            tokens.erase(tokens.begin() + step, tokens.begin() + count + step);
        }

        void Lexer::setLine(std::uint32_t line) {
            this->line = line;
        }

        void Lexer::setLexpos(std::uint32_t lexpos) {
            this->lexpos = lexpos;
        }

        void Lexer::setPointer(std::size_t pointer) {
            this->pointer = pointer;
        }

        void Lexer::setStep(std::size_t step) {
            this->step = step;
        }

        void Lexer::setModuleName(const std::string &moduleName) {
            this->moduleName = moduleName;
        }

        void Lexer::setSource(const std::string_view &source) {
            this->source = source;
        }

        std::uint32_t Lexer::getLine() const {
            return line;
        }

        std::uint32_t Lexer::getLexpos() const {
            return lexpos;
        }

        std::size_t Lexer::getPointer() const {
            return pointer;
        }

        std::size_t Lexer::getStep() const {
            return step;
        }

        const std::string &Lexer::getModuleName() const {
            return moduleName;
        }

        const std::string &Lexer::getSource() const {
            return source;
        }
    }
}
