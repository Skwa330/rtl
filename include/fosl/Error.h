#ifndef _SDC_ERROR_H_
#define _SDC_ERROR_H_

#include <exception>
#include <string_view>

#include <cstdint>

namespace fosl {
    class Error : public std::exception {
    public:
        enum class Type {
            Lexical,
            Syntactical,
            Semantic,
            Intermediate
        };

        enum class Priority {
            Info,
            Warning,
            Error,
            Fatal
        };
    private:
        Type type;
        Priority priority;

        std::string_view moduleName;
        std::uint32_t line, lexpos;

        std::string message;
    public:
        Error(Type type, Priority priority, const std::string_view &moduleName, std::uint32_t line, std::uint32_t lexpos, const std::string_view &message);

        Type getType() const;
        Priority getPriority() const;

        const std::string_view &getModuleName() const;

        std::uint32_t getLine() const;
        std::uint32_t getLexpos() const;

        const std::string &getMessage() const;
        const char *what() const;
    };
}

#endif /* _KNW_ERROR_H_  */
