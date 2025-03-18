
#include "KeywordManager.h"

#include <algorithm>
#include <array>
#include <unordered_set>
#include <vector>
namespace rp {
    namespace frontend {

        // 静态成员初始化
        KeywordManager::KeywordMap KeywordManager::keywords;
        KeywordManager::KeywordMap KeywordManager::contextualKeywords;
        size_t KeywordManager::minKeywordLength = SIZE_MAX;
        size_t KeywordManager::maxKeywordLength = 0;
        std::once_flag KeywordManager::initFlag;

        namespace {
            // 关键字分类
            enum class KeywordCategory {
                Basic,      // 基础关键字
                Type,       // 类型关键字
                Access,     // 访问控制
                Control,    // 控制流
                Modifier,   // 修饰符
                Template,   // 模板相关
                Coroutine,  // 协程相关
                Modern,     // 现代C++
                Attribute   // 属性说明符
            };

            // 关键字信息结构
            struct KeywordInfo {
                TokenKind kind;
                KeywordCategory category;
                const char* description;
                const char* example;
            };

            // 关键字信息映射
            std::unordered_map<std::string_view, KeywordInfo> keywordInfoMap;

            // 初始化关键字信息
            void initializeKeywordInfo() {
                keywordInfoMap = {
                    {"class",
                     {TokenKind::Keyword_Class, KeywordCategory::Basic, "Declares a class type", "class MyClass { };"}},
                    {"struct",
                     {TokenKind::Keyword_Struct,
                      KeywordCategory::Basic,
                      "Declares a structure type",
                      "struct MyStruct { };"}},
                    {"template",
                     {TokenKind::Keyword_Template,
                      KeywordCategory::Template,
                      "Declares a template",
                      "template<typename T> class Container { };"}},
                    // ... 添加更多关键字信息
                };
            }

            // 获取相似关键字
            std::vector<std::string_view> findSimilarKeywords(std::string_view text) {
                std::vector<std::string_view> similar;
                const size_t maxDistance = 2;  // 最大编辑距离

                for (const auto& pair : keywordInfoMap) {
                    size_t distance = 0;
                    const std::string_view& keyword = pair.first;

                    // 简单的编辑距离计算
                    if (std::abs(static_cast<int>(keyword.length()) - static_cast<int>(text.length())) <= maxDistance) {
                        size_t matches = 0;
                        for (size_t i = 0; i < std::min(keyword.length(), text.length()); ++i) {
                            if (keyword[i] == text[i]) {
                                matches++;
                            }
                        }
                        distance = std::max(keyword.length(), text.length()) - matches;

                        if (distance <= maxDistance) {
                            similar.push_back(keyword);
                        }
                    }
                }
                return similar;
            }
        }  // namespace

        bool KeywordManager::isKeyword(std::string_view text, TokenKind& kind) {
            std::call_once(initFlag, initialize);

            // 快速长度检查
            if (text.length() < minKeywordLength || text.length() > maxKeywordLength) {
                return false;
            }

            auto it = keywords.find(text);
            if (it != keywords.end()) {
                kind = it->second;
                return true;
            }
            return false;
        }

        bool KeywordManager::isContextualKeyword(std::string_view text, TokenKind& kind) {
            std::call_once(initFlag, initialize);

            auto it = contextualKeywords.find(text);
            if (it != contextualKeywords.end()) {
                kind = it->second;
                return true;
            }
            return false;
        }

        bool KeywordManager::isValidKeyword(std::string_view text) {
            TokenKind kind;
            return isKeyword(text, kind);
        }

        bool KeywordManager::requiresContext(TokenKind kind) {
            std::call_once(initFlag, initialize);

            for (const auto& pair : contextualKeywords) {
                if (pair.second == kind) {
                    return true;
                }
            }
            return false;
        }

        void KeywordManager::initialize() {
            initializeKeywordMap();
            initializeContextualKeywordMap();
            updateKeywordLengths();
            initializeKeywordInfo();
        }

        void KeywordManager::updateKeywordLengths() {
            for (const auto& pair : keywords) {
                minKeywordLength = std::min(minKeywordLength, pair.first.length());
                maxKeywordLength = std::max(maxKeywordLength, pair.first.length());
            }
        }

        void KeywordManager::initializeKeywordMap() {
            // 基础关键字
            keywords["class"] = TokenKind::Keyword_Class;
            keywords["struct"] = TokenKind::Keyword_Struct;
            keywords["enum"] = TokenKind::Keyword_Enum;
            keywords["union"] = TokenKind::Keyword_Union;

            // 模板相关
            keywords["template"] = TokenKind::Keyword_Template;
            keywords["typename"] = TokenKind::Keyword_Typename;
            keywords["concept"] = TokenKind::Keyword_Concept;
            keywords["requires"] = TokenKind::Keyword_Requires;

            // 访问控制
            keywords["public"] = TokenKind::Keyword_Public;
            keywords["private"] = TokenKind::Keyword_Private;
            keywords["protected"] = TokenKind::Keyword_Protected;

            // 控制流
            keywords["if"] = TokenKind::Keyword_If;
            keywords["else"] = TokenKind::Keyword_Else;
            keywords["while"] = TokenKind::Keyword_While;
            keywords["for"] = TokenKind::Keyword_For;
            keywords["do"] = TokenKind::Keyword_Do;
            keywords["break"] = TokenKind::Keyword_Break;
            keywords["continue"] = TokenKind::Keyword_Continue;
            keywords["return"] = TokenKind::Keyword_Return;

            // 协程相关
            keywords["co_await"] = TokenKind::Keyword_co_await;
            keywords["co_yield"] = TokenKind::Keyword_co_yield;
            keywords["co_return"] = TokenKind::Keyword_co_return;

            // 类型和修饰符
            keywords["const"] = TokenKind::Keyword_Const;
            keywords["static"] = TokenKind::Keyword_Static;
            keywords["virtual"] = TokenKind::Keyword_Virtual;
            keywords["override"] = TokenKind::Keyword_Override;
            keywords["final"] = TokenKind::Keyword_Final;
            keywords["explicit"] = TokenKind::Keyword_Explicit;
            keywords["inline"] = TokenKind::Keyword_Inline;
            keywords["volatile"] = TokenKind::Keyword_Volatile;
            keywords["mutable"] = TokenKind::Keyword_Mutable;
            keywords["register"] = TokenKind::Keyword_Register;
            keywords["extern"] = TokenKind::Keyword_Extern;

            // 类型关键字
            keywords["void"] = TokenKind::Keyword_Void;
            keywords["bool"] = TokenKind::Keyword_Bool;
            keywords["char"] = TokenKind::Keyword_Char;
            keywords["short"] = TokenKind::Keyword_Short;
            keywords["int"] = TokenKind::Keyword_Int;
            keywords["long"] = TokenKind::Keyword_Long;
            keywords["float"] = TokenKind::Keyword_Float;
            keywords["double"] = TokenKind::Keyword_Double;
            keywords["signed"] = TokenKind::Keyword_Signed;
            keywords["unsigned"] = TokenKind::Keyword_Unsigned;

            // C++11及以后
            keywords["alignas"] = TokenKind::Keyword_Alignas;
            keywords["alignof"] = TokenKind::Keyword_Alignof;
            keywords["char16_t"] = TokenKind::Keyword_Char16_t;
            keywords["char32_t"] = TokenKind::Keyword_Char32_t;
            keywords["constexpr"] = TokenKind::Keyword_Constexpr;
            keywords["decltype"] = TokenKind::Keyword_Decltype;
            keywords["noexcept"] = TokenKind::Keyword_Noexcept;
            keywords["nullptr"] = TokenKind::Keyword_Nullptr;
            keywords["static_assert"] = TokenKind::Keyword_Static_assert;
            keywords["thread_local"] = TokenKind::Keyword_Thread_local;

            // C++20
            keywords["char8_t"] = TokenKind::Keyword_Char8_t;
            keywords["consteval"] = TokenKind::Keyword_Consteval;
            keywords["constinit"] = TokenKind::Keyword_Constinit;
            keywords["module"] = TokenKind::Keyword_Module;
            keywords["import"] = TokenKind::Keyword_Import;
            keywords["export"] = TokenKind::Keyword_Export;
        }

        void KeywordManager::initializeContextualKeywordMap() {
            // 属性说明符
            contextualKeywords["deprecated"] = TokenKind::Keyword_Deprecated;
            contextualKeywords["fallthrough"] = TokenKind::Keyword_Fallthrough;
            contextualKeywords["nodiscard"] = TokenKind::Keyword_Nodiscard;
            contextualKeywords["maybe_unused"] = TokenKind::Keyword_Maybe_unused;
        }

    }  // namespace frontend
}  // namespace rp
