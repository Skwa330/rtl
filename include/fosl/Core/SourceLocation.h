#ifndef FOSL_CORE_SOURCE_LOCATION_H
#define FOSL_CORE_SOURCE_LOCATION_H

namespace fosl {
    namespace core {
        struct SourceLocation {
            std::string moduleName;
            std::size_t pointer;
            std::uint32_t line, lexpos;
        };
    }
}

#endif /* FOSL_CORE_SOURCE_LOCATION_H */