#ifndef LANG_PARSER_ERROR_H
#define LANG_PARSER_ERROR_H

#include <exception>
#include <string_view>

#include <cstdint>

#include "SourceLocation.h"

namespace lang {
    namespace parser {
        class Error : public std::exception {
        public:
            enum class Type {
                Lexical,
                Syntactical
            };
        private:
            Type type;

            SourceLocation sourceLocation;

            std::string message;
        public:
            Error(Type type, const SourceLocation &sourceLocation, const std::string_view &message);

            Type getType() const;

            const SourceLocation &getSourceLocation() const;

            const std::string &getMessage() const;
            const char *what() const;
        };
    }
}

#endif /* LANG_PARSER_ERROR_H  */
