
#include "UTF8Scanner.h"

#include "Frontend/Parser/Lexer/Utils/Unicode.h"

namespace rp {
    namespace frontend {

        std::string UTF8Scanner::scanUTF8Sequence() {
            std::string result;
            unsigned char first = static_cast<unsigned char>(source[currentPos]);

            // 确定UTF-8序列的长度
            int sequenceLength = 0;
            if ((first & 0xE0) == 0xC0)
                sequenceLength = 2;
            else if ((first & 0xF0) == 0xE0)
                sequenceLength = 3;
            else if ((first & 0xF8) == 0xF0)
                sequenceLength = 4;
            else {
                // 无效的UTF-8起始字节
                return "";
            }

            // 检查是否有足够的字节
            if (currentPos + sequenceLength > sourceLength) {
                return "";
            }

            // 验证后续字节
            for (int i = 1; i < sequenceLength; i++) {
                if (!isValidUTF8Continuation(source[currentPos + i])) {
                    return "";
                }
            }

            // 复制整个UTF-8序列
            result.assign(source + currentPos, sequenceLength);
            currentPos += sequenceLength;
            currentColumn++;  // UTF-8字符计为一列

            return result;
        }

        void UTF8Scanner::reportInvalidUTF8() {
            diagnostics->report(
                DiagnosticLevel::Error,
                {filename, static_cast<unsigned int>(currentLine), static_cast<unsigned int>(currentColumn)},
                "Invalid UTF-8 sequence in identifier");
        }

        void UTF8Scanner::skipInvalidUTF8() {
            // 跳过无效的UTF-8序列，直到找到有效的UTF-8起始字节或ASCII字符
            while (currentPos < sourceLength) {
                unsigned char c = static_cast<unsigned char>(source[currentPos]);
                if (c < 128 || (c & 0xC0) != 0x80) {
                    break;
                }
                currentPos++;
                currentColumn++;
            }
        }

        uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
            unsigned char uc = static_cast<unsigned char>(first);
            uint32_t codepoint = 0;

            // 根据UTF-8编码规则解码
            if ((uc & 0x80) == 0) {
                return uc;
            } else if ((uc & 0xE0) == 0xC0) {
                if (currentPos + 1 >= sourceLength) return 0;
                codepoint = ((uc & 0x1F) << 6) | (static_cast<unsigned char>(source[currentPos + 1]) & 0x3F);
            } else if ((uc & 0xF0) == 0xE0) {
                if (currentPos + 2 >= sourceLength) return 0;
                codepoint = ((uc & 0x0F) << 12) | ((static_cast<unsigned char>(source[currentPos + 1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(source[currentPos + 2]) & 0x3F);
            } else if ((uc & 0xF8) == 0xF0) {
                if (currentPos + 3 >= sourceLength) return 0;
                codepoint = ((uc & 0x07) << 18) | ((static_cast<unsigned char>(source[currentPos + 1]) & 0x3F) << 12) |
                            ((static_cast<unsigned char>(source[currentPos + 2]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(source[currentPos + 3]) & 0x3F);
            }

            return codepoint;
        }

    }  // namespace frontend
}  // namespace rp
