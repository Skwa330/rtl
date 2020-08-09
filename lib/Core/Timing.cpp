#include "rlt/Core/Timing.h"

namespace rlt {
    namespace core {
        template<typename F, typename... ArgTypes>
        float calcTime(const F &f, ArgTypes... args) {
            std::int64_t start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            f(args...);
            std::int64_t end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            return (float)(end - start) / 1000000.0f;
        }
    }
}