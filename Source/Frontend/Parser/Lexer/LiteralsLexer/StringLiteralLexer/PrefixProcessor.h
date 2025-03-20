
#ifndef RP_PREFIX_PROCESSOR_H
#define RP_PREFIX_PROCESSOR_H

#include <string>

#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer/StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        class PrefixProcessor {
          public:
            // 解析字符串前缀
            static std::tuple<StringPrefix, size_t> parsePrefix(const std::string& source,
                                                                size_t currentPos,
                                                                size_t sourceLength);

            // 检查是否为有效的字符串前缀
            static bool isValidPrefix(const std::string& prefix);

            // 获取前缀的字符串表示
            static std::string getPrefixString(StringPrefix prefix);

          private:
            // 检查给定位置是否跟着引号
            static bool hasQuoteAfter(const std::string& source, size_t pos, size_t sourceLength);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_PREFIX_PROCESSOR_H
