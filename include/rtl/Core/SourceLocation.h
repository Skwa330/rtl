#ifndef RTL_PARSER_SOURCE_LOCATION_H
#define RTL_PARSER_SOURCE_LOCATION_H

#include <string>
#include <cstddef>
#include <cstdint>

namespace rtl {
    namespace core {
        struct SourceLocation {
            std::string_view moduleName;
            std::size_t pointer;
            std::uint32_t line, lexpos;

            SourceLocation() = default;
            SourceLocation(const std::string_view &moduleName, std::size_t pointer, std::uint32_t line, std::uint32_t lexpos);

            std::string getFormatted() const;
        };
    }
}

#endif /* RTL_PARSER_SOURCE_LOCATION_H */