#ifndef RLT_PARSER_SOURCE_LOCATION_H
#define RLT_PARSER_SOURCE_LOCATION_H

#include <string>
#include <cstddef>
#include <cstdint>

namespace rlt {
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

#endif /* RLT_PARSER_SOURCE_LOCATION_H */