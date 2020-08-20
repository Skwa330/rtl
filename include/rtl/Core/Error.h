#ifndef RTL_PARSER_ERROR_H
#define RTL_PARSER_ERROR_H

#include <exception>
#include <string_view>

#include <cstdint>

#include "SourceLocation.h"

namespace rtl {
    namespace core {
        class Error : public std::exception {
        public:
            enum class Type {
                Lexical,
                Syntactic,
                Semantic
            };
        private:
            Type type;

            std::string_view source;
            SourceLocation begin, end;

            std::string message;
        public:
            Error() = default;
            Error(Type type, const std::string_view &source, const SourceLocation &begin, const SourceLocation &end, const std::string_view &message);

            Type getType() const;

            const std::string_view &getSource() const;

            const SourceLocation &getBegin() const;
            const SourceLocation &getEnd() const;

            const std::string &getMessage() const;
            const char *what() const noexcept;
        };
    }
}

#endif /* RTL_PARSER_ERROR_H  */
