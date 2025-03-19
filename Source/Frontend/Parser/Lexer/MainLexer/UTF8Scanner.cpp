
#include "UTF8Scanner.h"

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        std::string UTF8Scanner::scanUTF8Sequence() {
            // 确保unicodeScanner已初始化
            if (!unicodeScanner) {
                unicodeScanner = std::make_unique<unicode::UTF8Scanner>(std::string(source, sourceLength));
            }

            // 设置scanner的当前位置
            unicodeScanner->setPosition(currentPos);

            // 使用Unicode模块扫描UTF-8序列
            std::string result = unicodeScanner->scanUTF8Sequence();

            if (!result.empty()) {
                // 更新位置信息
                currentPos += result.length();
                currentColumn++;  // UTF-8字符计为一列
            }

            return result;
        }

        void UTF8Scanner::reportInvalidUTF8() {
            diagnostics->report(
                DiagnosticLevel::Error,
                {filename, static_cast<unsigned int>(currentLine), static_cast<unsigned int>(currentColumn)},
                "Invalid UTF-8 sequence in identifier");
        }

        void UTF8Scanner::skipInvalidUTF8() {
            if (!unicodeScanner) {
                unicodeScanner = std::make_unique<unicode::UTF8Scanner>(std::string(source, sourceLength));
            }

            unicodeScanner->setPosition(currentPos);
            unicodeScanner->skipInvalidUTF8();

            // 更新位置
            size_t newPos = unicodeScanner->position();
            currentColumn += (newPos - currentPos);
            currentPos = newPos;
        }

        uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
            if (!unicodeScanner) {
                unicodeScanner = std::make_unique<unicode::UTF8Scanner>(std::string(source, sourceLength));
            }

            unicodeScanner->setPosition(currentPos);
            return unicodeScanner->decodeUTF8Sequence(first);
        }

    }  // namespace frontend
}  // namespace rp
