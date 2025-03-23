#ifndef RP_PREFIX_PROCESSOR_H
#define RP_PREFIX_PROCESSOR_H

#include <string>
#include <tuple>

#include "StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        class PrefixProcessor {
          public:
            // 在词法分析过程中解析字符串前缀
            // 返回值：{前缀类型, 原始前缀字符串, 前缀长度}
            static std::tuple<StringPrefix, std::string, size_t> parsePrefix(const std::string& source,
                                                                             size_t currentPos,
                                                                             size_t sourceLength);

            // 检查给定位置的字符串是否以有效的前缀开始
            static bool hasValidPrefix(const std::string& source, size_t currentPos, size_t sourceLength);

            // 获取前缀后第一个引号的位置
            static size_t findQuoteAfterPrefix(const std::string& source, size_t currentPos, size_t sourceLength);

          private:
            // 检查给定位置是否跟着引号（单引号或双引号）
            static bool hasQuoteAfter(const std::string& source, size_t pos, size_t sourceLength);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_PREFIX_PROCESSOR_H
