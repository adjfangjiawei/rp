#include "UTF8Scanner.h"

#include <stdexcept>

namespace rp::frontend::unicode {

    std::string UTF8Scanner::scanUTF8Sequence() {
        if (!hasMore()) {
            return "";
        }

        std::string sequence;
        char first = advance();
        sequence += first;

        // 获取需要的后续字节数
        int continuationBytes = 0;
        unsigned char uc = static_cast<unsigned char>(first);

        if ((uc & 0x80) == 0) {
            // ASCII字符
            return sequence;
        } else if ((uc & 0xE0) == 0xC0) {
            // 2字节序列
            continuationBytes = 1;
        } else if ((uc & 0xF0) == 0xE0) {
            // 3字节序列
            continuationBytes = 2;
        } else if ((uc & 0xF8) == 0xF0) {
            // 4字节序列
            continuationBytes = 3;
        } else {
            // 无效的UTF-8序列
            reportInvalidUTF8("Invalid UTF-8 start byte");
            return sequence;
        }

        // 读取后续字节
        for (int i = 0; i < continuationBytes && hasMore(); ++i) {
            char next = peek();
            if (!isValidUTF8Continuation(next)) {
                reportInvalidUTF8("Invalid UTF-8 continuation byte");
                break;
            }
            sequence += advance();
        }

        return sequence;
    }

    uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
        unsigned char uc = static_cast<unsigned char>(first);
        uint32_t codepoint = 0;
        int continuationBytes = 0;

        // 确定序列长度和初始位
        if ((uc & 0x80) == 0) {
            return uc;
        } else if ((uc & 0xE0) == 0xC0) {
            codepoint = uc & 0x1F;
            continuationBytes = 1;
        } else if ((uc & 0xF0) == 0xE0) {
            codepoint = uc & 0x0F;
            continuationBytes = 2;
        } else if ((uc & 0xF8) == 0xF0) {
            codepoint = uc & 0x07;
            continuationBytes = 3;
        } else {
            reportInvalidUTF8("Invalid UTF-8 start byte");
            return 0xFFFD;  // Unicode替换字符
        }

        // 读取后续字节
        size_t originalPosition = position();
        for (int i = 0; i < continuationBytes && hasMore(); ++i) {
            char next = peek();
            if (!isValidUTF8Continuation(next)) {
                reportInvalidUTF8("Invalid UTF-8 continuation byte");
                setPosition(originalPosition);
                return 0xFFFD;
            }
            advance();
            codepoint = (codepoint << 6) | (static_cast<unsigned char>(next) & 0x3F);
        }

        // 验证码点范围
        if ((codepoint > 0x10FFFF) ||                     // 超出Unicode范围
            (codepoint >= 0xD800 && codepoint <= 0xDFFF)  // 代理对范围
        ) {
            reportInvalidUTF8("Invalid Unicode codepoint");
            return 0xFFFD;
        }

        return codepoint;
    }

    void UTF8Scanner::skipInvalidUTF8() {
        // 跳过当前的无效字节，直到找到有效的UTF-8序列起始字节
        while (hasMore()) {
            char c = peek();
            unsigned char uc = static_cast<unsigned char>(c);

            // 检查是否是有效的UTF-8起始字节
            if ((uc & 0x80) == 0 ||     // ASCII
                (uc & 0xE0) == 0xC0 ||  // 2字节序列起始
                (uc & 0xF0) == 0xE0 ||  // 3字节序列起始
                (uc & 0xF8) == 0xF0) {  // 4字节序列起始
                break;
            }

            advance();  // 跳过无效字节
        }
    }

    void UTF8Scanner::reportInvalidUTF8(const std::string& message) { lastError_ = {message, position()}; }

}  // namespace rp::frontend::unicode
