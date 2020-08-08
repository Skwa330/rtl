#include "lang/Parser/SourceLocation.h"

#include <fmt/format.h>

namespace lang {
    namespace parser {
        SourceLocation::SourceLocation(const std::string &moduleName, std::size_t pointer, std::uint32_t line, std::uint32_t lexpos) : moduleName(moduleName), pointer(pointer), line(line), lexpos(lexpos) {
        }

        std::string SourceLocation::getFormatted() const {
            return fmt::format("{}:{}:{}", moduleName, line, lexpos);
        }
    }
}