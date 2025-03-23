#include "PrefixProcessor.h"

namespace rp {
    namespace frontend {

        std::tuple<StringPrefix, std::string, size_t> PrefixProcessor::parsePrefix(const std::string& source,
                                                                                   size_t currentPos,
                                                                                   size_t sourceLength) {
            if (currentPos >= sourceLength) {
                return {StringPrefix::None, "", 0};
            }

            // 获取可能的前缀部分（最多取3个字符，因为最长的前缀是"u8R"）
            size_t maxPrefixLen = std::min(size_t(3), sourceLength - currentPos);
            std::string possiblePrefix = source.substr(currentPos, maxPrefixLen);

            // 使用StringLiteralUtils解析前缀
            auto [prefix, length] = StringLiteralUtils::parseStringPrefix(possiblePrefix);

            // 确保前缀后面跟着引号
            if (prefix != StringPrefix::None && !hasQuoteAfter(source, currentPos + length, sourceLength)) {
                return {StringPrefix::None, "", 0};
            }

            // 返回解析结果
            return {prefix, length > 0 ? source.substr(currentPos, length) : "", length};
        }

        bool PrefixProcessor::hasValidPrefix(const std::string& source, size_t currentPos, size_t sourceLength) {
            auto [prefix, _, length] = parsePrefix(source, currentPos, sourceLength);
            return prefix != StringPrefix::None;
        }

        size_t PrefixProcessor::findQuoteAfterPrefix(const std::string& source,
                                                     size_t currentPos,
                                                     size_t sourceLength) {
            auto [_, __, length] = parsePrefix(source, currentPos, sourceLength);
            size_t quotePos = currentPos + length;
            return (quotePos < sourceLength && hasQuoteAfter(source, quotePos, sourceLength)) ? quotePos
                                                                                              : std::string::npos;
        }

        bool PrefixProcessor::hasQuoteAfter(const std::string& source, size_t pos, size_t sourceLength) {
            if (pos >= sourceLength) {
                return false;
            }
            return source[pos] == '"' || source[pos] == '\'';  // 支持单引号和双引号
        }

    }  // namespace frontend
}  // namespace rp
