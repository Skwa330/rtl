#ifndef RTL_CORE_TIMING_H
#define RTL_CORE_TIMING_H

#include <chrono>
#include <tuple>

#include <cstdint>

namespace rtl {
    template<typename F, typename... ArgTypes>
    float calcTime(const F &f, ArgTypes... args);
}

#endif /* RTL_CORE_TIMING_H */