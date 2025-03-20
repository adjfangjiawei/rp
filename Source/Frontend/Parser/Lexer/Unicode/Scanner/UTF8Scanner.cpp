#include "UTF8Scanner.h"

#include <stdexcept>

#include "../Core/UnicodeCore.h"
#include "../Encoding/UnicodeEncoding.h"

namespace rp::frontend::unicode {

    namespace {
        // UTF-8序列长度常量
        constexpr size_t MAX_UTF8_BYTES = 4;

        // 错误消息
        constexpr const char* ERROR_INVALID_START = "Invalid UTF-8 start byte";
        constexpr const char* ERROR_INVALID_CONTINUATION = "Invalid UTF-8 continuation byte";
        constexpr const char* ERROR_INCOMPLETE_SEQUENCE = "Incomplete UTF-8 sequence";
        constexpr const char* ERROR_OVERLONG_ENCODING = "Overlong UTF-8 encoding detected";
        constexpr const char* ERROR_INVALID_CODEPOINT = "Invalid Unicode codepoint";
    }  // namespace

    std::string UTF8Scanner::scanUTF8Sequence() {
        if (!hasMore()) {
            return "";
        }

        // 获取序列起始位置
        size_t startPos = position();
        char first = peek();

        // 使用Core模块验证序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, startPos);

        if (!info.valid) {
            reportInvalidUTF8(info.error);
            advance();  // 跳过无效字节
            return std::string(1, first);
        }

        // 构建有效的UTF-8序列
        std::string sequence;
        sequence.reserve(info.length);
        for (size_t i = 0; i < info.length; ++i) {
            sequence += advance();
        }

        return sequence;
    }

    uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
        size_t startPos = position() - 1;  // 减1是因为first已经被读取

        // 使用Core模块验证和解码序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, startPos);

        if (!info.valid) {
            reportInvalidUTF8(info.error);
            return 0xFFFD;  // Unicode替换字符
        }

        // 移动位置到序列末尾
        setPosition(startPos + info.length);
        return info.codepoint;
    }

    void UTF8Scanner::skipInvalidUTF8() {
        while (hasMore()) {
            size_t currentPos = position();
            char c = peek();

            // 使用Core模块验证当前位置是否是有效的UTF-8序列开始
            if (UnicodeCore::isValidUtf8FirstByte(static_cast<unsigned char>(c))) {
                UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);
                if (info.valid) {
                    break;
                }
            }

            advance();  // 跳过无效字节
        }
    }

    void UTF8Scanner::reportInvalidUTF8(const std::string& message) {
        std::string detailedMessage = message + " at position " + std::to_string(position());

        // 添加上下文信息
        if (position() > 0) {
            detailedMessage += "\nContext: ";
            size_t contextStart = (position() > 10) ? position() - 10 : 0;
            size_t contextLength = std::min(20ul, input_.length() - contextStart);
            detailedMessage += input_.substr(contextStart, contextLength);
            detailedMessage += "\n";
            for (size_t i = contextStart; i < position(); ++i) {
                detailedMessage += " ";
            }
            detailedMessage += "^";
        }

        lastError_ = {detailedMessage, position()};
    }

    // 新增的辅助方法

    bool UTF8Scanner::tryPeekCodepoint(uint32_t& codepoint) const {
        if (!hasMore()) {
            return false;
        }

        size_t currentPos = position();
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

        if (!info.valid) {
            return false;
        }

        codepoint = info.codepoint;
        return true;
    }

    size_t UTF8Scanner::lookAhead(size_t n) const {
        size_t pos = position();
        size_t count = 0;

        while (count < n && pos < input_.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, pos);
            if (!info.valid) {
                pos++;  // 跳过无效字节
            } else {
                pos += info.length;
                count++;
            }
        }

        return pos;
    }

    std::string UTF8Scanner::peekString(size_t length) const {
        if (!hasMore()) {
            return "";
        }

        std::string result;
        size_t pos = position();
        size_t charCount = 0;

        while (charCount < length && pos < input_.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, pos);
            if (!info.valid) {
                result += input_[pos++];  // 添加无效字节
            } else {
                result.append(input_.substr(pos, info.length));
                pos += info.length;
                charCount++;
            }
        }

        return result;
    }

    bool UTF8Scanner::skipUntil(uint32_t targetCodepoint) {
        while (hasMore()) {
            size_t currentPos = position();
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

            if (info.valid) {
                if (info.codepoint == targetCodepoint) {
                    setPosition(currentPos);
                    return true;
                }
                setPosition(currentPos + info.length);
            } else {
                advance();  // 跳过无效字节
            }
        }
        return false;
    }

    std::string UTF8Scanner::collectUntil(uint32_t targetCodepoint) {
        std::string result;
        size_t start = position();

        while (hasMore()) {
            size_t currentPos = position();
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

            if (info.valid) {
                if (info.codepoint == targetCodepoint) {
                    break;
                }
                setPosition(currentPos + info.length);
            } else {
                advance();  // 跳过无效字节
            }
        }

        size_t end = position();
        if (end > start) {
            result = input_.substr(start, end - start);
        }

        return result;
    }

}  // namespace rp::frontend::unicode
