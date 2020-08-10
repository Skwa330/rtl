#ifndef RLT_CORE_RECORD_H
#define RLT_CORE_RECORD_H

#include <vector>
#include <functional>

namespace rlt {
    namespace core {
        template<typename T>
        class Record {
        private:
            std::vector<std::size_t> records;
        public:
            void add(const T &ty) {
                records.push_back(std::hash<T>()(ty));
            }

            template<typename... ArgTypes>
            void emplace(ArgTypes... args) {
                records.push_back(std::hash<T>()(T(std::forward(args)...)));
            }

            void reserve(std::size_t count) {
                records.reserve(count);
            }

            bool contains(const T &ty) const {
                auto hash = std::hash<T>()(ty);

                for (const auto &record : records) {
                    if (hash == record) return true;
                }

                return false;
            }

            const std::vector<std::size_t> &getRecords() const {
                return records;
            }
        };
    }
}

#endif /* RLT_CORE_RECORD_H */