#include "fosl/Core/Error.h"

namespace fosl {
    Error::Error(Type type, Priority priority, const std::string_view &moduleName, std::uint32_t line, std::uint32_t lexpos, const std::string_view &message) {
        this->type = type;
        this->priority = priority;

        this->moduleName = moduleName;
        this->line = line;
        this->lexpos = lexpos;
        this->message = message;
    }

    Error::Type Error::getType() const {
        return type;
    }

    Error::Priority Error::getPriority() const {
        return priority;
    }

    const std::string_view &Error::getModuleName() const {
        return moduleName;
    }

    std::uint32_t Error::getLine() const {
        return line;
    }

    std::uint32_t Error::getLexpos() const {
        return lexpos;
    }

    const std::string &Error::getMessage() const {
        return message;
    }

    const char *Error::what() const {
        return message.c_str();
    }
}
