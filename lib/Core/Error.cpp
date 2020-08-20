#include "rtl/Core/Error.h"

namespace rtl {
    namespace core {
        Error::Error(Type type, const std::string_view &source, const SourceLocation &begin, const SourceLocation &end, const std::string_view &message) {
            this->type = type;

            this->source = source;

            this->begin = begin;
            this->end = end;

            this->message = message;
        }

        Error::Type Error::getType() const {
            return type;
        }

        const std::string_view &Error::getSource() const {
            return source;
        }

        const SourceLocation &Error::getBegin() const {
            return begin;
        }

        const SourceLocation &Error::getEnd() const {
            return end;
        }

        const std::string &Error::getMessage() const {
            return message;
        }

        const char *Error::what() const noexcept {
            return message.c_str();
        }
    }
}
