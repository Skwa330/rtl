#ifndef LANG_CORE_TIMING_H
#define LANG_CORE_TIMING_H

#include <chrono>
#include <tuple>

#include <cstdint>

namespace lang {
    template<typename F, typename... ArgTypes>
    float calcTime(const F &f, ArgTypes... args);
}

#endif /* LANG_CORE_TIMING_H */