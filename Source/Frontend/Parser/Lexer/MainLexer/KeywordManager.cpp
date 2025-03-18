
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

namespace rp {
    namespace frontend {

        std::unordered_map<std::string, TokenKind> KeywordManager::keywords;

        void KeywordManager::initialize() {
            if (keywords.empty()) {
                initializeKeywordMap();
            }
        }

        bool KeywordManager::isKeyword(const std::string& text, TokenKind& kind) {
            auto it = keywords.find(text);
            if (it != keywords.end()) {
                kind = it->second;
                return true;
            }
            return false;
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
        }

    }  // namespace frontend
}  // namespace rp
