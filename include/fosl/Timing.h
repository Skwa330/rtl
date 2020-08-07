#ifndef _SDC_TIMING_H_
#define _SDC_TIMING_H_

#include <chrono>
#include <tuple>

#include <cstdint>

namespace fosl {
    template<typename F, typename... ArgTypes>
    inline static float calcTime(const F &f, ArgTypes... args) {
        std::int64_t start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        f(args...);
        std::int64_t end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        return (float)(end - start) / 1000000000.0f;
    }
}

#endif /* _KNW_TIMING_H_ */