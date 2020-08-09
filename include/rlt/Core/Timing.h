#ifndef RLT_CORE_TIMING_H
#define RLT_CORE_TIMING_H

#include <chrono>
#include <tuple>

#include <cstdint>

namespace rlt {
    template<typename F, typename... ArgTypes>
    float calcTime(const F &f, ArgTypes... args);
}

#endif /* RLT_CORE_TIMING_H */