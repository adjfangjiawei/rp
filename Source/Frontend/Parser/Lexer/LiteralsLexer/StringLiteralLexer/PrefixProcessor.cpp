
#include "PrefixProcessor.h"

#include <cstring>

namespace rp {
    namespace frontend {

        std::tuple<StringPrefix, size_t> PrefixProcessor::parsePrefix(const std::string& source,
                                                                      size_t currentPos,
                                                                      size_t sourceLength) {
            if (currentPos >= sourceLength) {
                return {StringPrefix::None, 0};
            }

            // 尝试匹配最长的有效前缀
            std::string input = source.substr(currentPos, std::min(size_t(4), sourceLength - currentPos));
            StringPrefix bestPrefix = StringPrefix::None;
            size_t bestLength = 0;

            // 检查所有可能的前缀组合
            struct PrefixInfo {
                const char* str;
                StringPrefix prefix;
            };

            static const PrefixInfo prefixes[] = {{"u8R", StringPrefix::u8R},
                                                  {"u8", StringPrefix::u8},
                                                  {"uR", StringPrefix::uR},
                                                  {"UR", StringPrefix::UR},
                                                  {"LR", StringPrefix::LR},
                                                  {"u", StringPrefix::u},
                                                  {"U", StringPrefix::U},
                                                  {"L", StringPrefix::L},
                                                  {"R", StringPrefix::R}};

            for (const auto& info : prefixes) {
                size_t len = strlen(info.str);
                if (input.length() >= len && input.substr(0, len) == info.str && len > bestLength) {
                    // 确保后面跟着引号
                    if (hasQuoteAfter(source, currentPos + len, sourceLength)) {
                        bestPrefix = info.prefix;
                        bestLength = len;
                    }
                }
            }

            return {bestPrefix, bestLength};
        }

        bool PrefixProcessor::isValidPrefix(const std::string& prefix) {
            static const std::string validPrefixes[] = {"u8R", "u8", "uR", "UR", "LR", "u", "U", "L", "R"};

            for (const auto& valid : validPrefixes) {
                if (prefix == valid) {
                    return true;
                }
            }
            return false;
        }

        std::string PrefixProcessor::getPrefixString(StringPrefix prefix) {
            switch (prefix) {
                case StringPrefix::u8R:
                    return "u8R";
                case StringPrefix::u8:
                    return "u8";
                case StringPrefix::uR:
                    return "uR";
                case StringPrefix::UR:
                    return "UR";
                case StringPrefix::LR:
                    return "LR";
                case StringPrefix::u:
                    return "u";
                case StringPrefix::U:
                    return "U";
                case StringPrefix::L:
                    return "L";
                case StringPrefix::R:
                    return "R";
                default:
                    return "";
            }
        }

        bool PrefixProcessor::hasQuoteAfter(const std::string& source, size_t pos, size_t sourceLength) {
            if (pos >= sourceLength) {
                return false;
            }
            return source[pos] == '"';
        }

    }  // namespace frontend
}  // namespace rp
