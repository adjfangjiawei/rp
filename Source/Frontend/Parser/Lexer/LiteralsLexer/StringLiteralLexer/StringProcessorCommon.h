#ifndef STRING_PROCESSOR_COMMON_H
#define STRING_PROCESSOR_COMMON_H

#include <string>
#include <vector>

namespace rp {
    namespace frontend {

        // 共享的UTF-8处理结果结构体
        struct UTF8ProcessResult {
            bool success;
            size_t consumed;
            std::string content;
            std::vector<std::string> warnings;
            std::string error;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // STRING_PROCESSOR_COMMON_H
