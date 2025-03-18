
#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class KeywordManager {
          public:
            // 关键字检查
            static bool isKeyword(std::string_view text, TokenKind& kind);
            static bool isContextualKeyword(std::string_view text, TokenKind& kind);

            // 关键字验证
            static bool isValidKeyword(std::string_view text);
            static bool requiresContext(TokenKind kind);

            // 关键字信息
            static size_t getMinKeywordLength() { return minKeywordLength; }
            static size_t getMaxKeywordLength() { return maxKeywordLength; }

          private:
            // 关键字映射
            using KeywordMap = std::unordered_map<std::string_view, TokenKind>;
            static KeywordMap keywords;
            static KeywordMap contextualKeywords;

            // 关键字长度范围
            static size_t minKeywordLength;
            static size_t maxKeywordLength;

            // 初始化控制
            static std::once_flag initFlag;
            static void initialize();
            static void initializeKeywordMap();
            static void initializeContextualKeywordMap();
            static void updateKeywordLengths();

            // 禁用构造和赋值
            KeywordManager() = delete;
            KeywordManager(const KeywordManager&) = delete;
            KeywordManager& operator=(const KeywordManager&) = delete;
        };

    }  // namespace frontend
}  // namespace rp
