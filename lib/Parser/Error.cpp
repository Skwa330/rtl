#include "lang/Parser/Error.h"

namespace lang {
    namespace parser {
        Error::Error(Type type, const SourceLocation &sourceLocation, const std::string_view &message) {
            this->type = type;

            this->sourceLocation = sourceLocation;
            this->message = message;
        }

        Error::Type Error::getType() const {
            return type;
        }

        const SourceLocation &Error::getSourceLocation() const {
            return sourceLocation;
        }

        const std::string &Error::getMessage() const {
            return message;
        }

        const char *Error::what() const {
            return message.c_str();
        }
    }
}
