#include "UTF8Scanner.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../Core/UnicodeCore.h"
#include "../Encoding/UnicodeEncoding.h"

namespace rp::frontend::unicode {

    namespace {
        // UTF-8序列长度常量
        constexpr size_t MAX_UTF8_BYTES = 4;
        constexpr size_t MAX_CONTEXT_LENGTH = 40;  // 错误上下文的最大长度
        constexpr size_t CONTEXT_BEFORE = 20;      // 错误位置之前的上下文长度
        constexpr size_t CONTEXT_AFTER = 20;       // 错误位置之后的上下文长度

        // 错误消息
        constexpr const char* ERROR_INVALID_START = "Invalid UTF-8 start byte";
        constexpr const char* ERROR_INVALID_CONTINUATION = "Invalid UTF-8 continuation byte";
        constexpr const char* ERROR_INCOMPLETE_SEQUENCE = "Incomplete UTF-8 sequence";
        constexpr const char* ERROR_OVERLONG_ENCODING = "Overlong UTF-8 encoding detected";
        constexpr const char* ERROR_INVALID_CODEPOINT = "Invalid Unicode codepoint";
        constexpr const char* ERROR_BUFFER_OVERFLOW = "Buffer overflow detected";
        constexpr const char* ERROR_INVALID_POSITION = "Invalid position in input";
        constexpr const char* ERROR_UNEXPECTED_END = "Unexpected end of input";

        // 辅助函数：格式化字节为十六进制字符串
        std::string formatByte(unsigned char byte) {
            std::stringstream ss;
            ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            return ss.str();
        }

        // 辅助函数：生成错误上下文信息
        std::string generateErrorContext(const std::string& input, size_t position) {
            if (input.empty() || position >= input.length()) {
                return "";
            }

            // 计算上下文范围
            size_t contextStart = (position > CONTEXT_BEFORE) ? position - CONTEXT_BEFORE : 0;
            size_t contextLength = std::min(MAX_CONTEXT_LENGTH, input.length() - contextStart);

            std::stringstream context;
            context << "\nContext:\n";

            // 添加行号
            context << std::setw(6) << (contextStart + 1) << " | ";

            // 添加上下文内容
            std::string contextStr = input.substr(contextStart, contextLength);
            context << contextStr << "\n";

            // 添加错误位置指示器
            context << "       ";  // 对齐with行号
            for (size_t i = contextStart; i < position; ++i) {
                context << (input[i] == '\t' ? '\t' : ' ');
            }
            context << "^";

            // 如果有多字节序列，添加更多指示器
            size_t sequenceLength = 1;
            if (position < input.length()) {
                UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input, position);
                if (info.valid) {
                    sequenceLength = info.length;
                }
            }
            for (size_t i = 1; i < sequenceLength; ++i) {
                context << "~";
            }

            return context.str();
        }
    }  // namespace

    std::string UTF8Scanner::scanUTF8Sequence() {
        if (!hasMore()) {
            reportInvalidUTF8(ERROR_UNEXPECTED_END);
            return "";
        }

        // 获取序列起始位置
        size_t startPos = position();
        unsigned char first = static_cast<unsigned char>(peek());

        // 使用Core模块验证序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, startPos);

        if (!info.valid) {
            std::stringstream errorMsg;
            errorMsg << info.error << " (first byte: " << formatByte(first) << ")";
            reportInvalidUTF8(errorMsg.str());
            advance();                                      // 跳过无效字节
            return std::string(1, static_cast<char>(0xEF))  // 返回UTF-8编码的替换字符
                   + static_cast<char>(0xBF) + static_cast<char>(0xBD);
        }

        // 构建有效的UTF-8序列
        std::string sequence;
        sequence.reserve(info.length);  // 预分配精确的空间

        try {
            for (size_t i = 0; i < info.length; ++i) {
                sequence += advance();
            }
        } catch (const std::exception& e) {
            reportInvalidUTF8(std::string(ERROR_INCOMPLETE_SEQUENCE) + ": " + e.what());
            return std::string(1, static_cast<char>(0xEF))  // 返回UTF-8编码的替换字符
                   + static_cast<char>(0xBF) + static_cast<char>(0xBD);
        }

        return sequence;
    }

    uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
        if (position() == 0) {
            reportInvalidUTF8(ERROR_INVALID_POSITION);
            return 0xFFFD;  // Unicode替换字符
        }

        size_t startPos = position() - 1;  // 减1是因为first已经被读取
        unsigned char firstByte = static_cast<unsigned char>(first);

        // 使用Core模块验证和解码序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, startPos);

        if (!info.valid) {
            std::stringstream errorMsg;
            errorMsg << info.error << " (first byte: " << formatByte(firstByte) << ")";
            reportInvalidUTF8(errorMsg.str());
            return 0xFFFD;  // Unicode替换字符
        }

        try {
            // 移动位置到序列末尾
            setPosition(startPos + info.length);
            return info.codepoint;
        } catch (const std::exception& e) {
            reportInvalidUTF8(std::string(ERROR_INCOMPLETE_SEQUENCE) + ": " + e.what());
            return 0xFFFD;  // Unicode替换字符
        }
    }

    void UTF8Scanner::skipInvalidUTF8() {
        size_t skippedBytes = 0;
        const size_t maxSkip = 1024;  // 防止无限循环

        while (hasMore() && skippedBytes < maxSkip) {
            size_t currentPos = position();
            unsigned char c = static_cast<unsigned char>(peek());

            // 使用Core模块验证当前位置是否是有效的UTF-8序列开始
            if (UnicodeCore::isValidUtf8FirstByte(c)) {
                UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);
                if (info.valid) {
                    // 找到有效序列，停止跳过
                    if (skippedBytes > 0) {
                        std::stringstream ss;
                        ss << "Skipped " << skippedBytes << " invalid bytes";
                        reportInvalidUTF8(ss.str());
                    }
                    return;
                }
            }

            advance();  // 跳过无效字节
            skippedBytes++;
        }

        if (skippedBytes >= maxSkip) {
            reportInvalidUTF8("Too many invalid bytes encountered, stopping skip operation");
        }
    }

    void UTF8Scanner::reportInvalidUTF8(const std::string& message) {
        std::stringstream detailedMessage;
        detailedMessage << "UTF-8 Error: " << message << "\n"
                        << "Position: " << position() << " of " << input_.length();

        // 添加字节信息
        if (hasMore()) {
            detailedMessage << "\nCurrent byte: " << formatByte(static_cast<unsigned char>(peek()));

            // 添加后续字节信息
            size_t remaining = std::min(size_t(3), input_.length() - position() - 1);
            if (remaining > 0) {
                detailedMessage << "\nNext bytes: ";
                for (size_t i = 1; i <= remaining; ++i) {
                    detailedMessage << formatByte(static_cast<unsigned char>(input_[position() + i])) << " ";
                }
            }
        }

        // 添加上下文信息
        detailedMessage << generateErrorContext(input_, position());

        lastError_ = {detailedMessage.str(), position()};
    }

    // 新增的辅助方法

    bool UTF8Scanner::tryPeekCodepoint(uint32_t& codepoint) const {
        try {
            // 增强的输入验证
            if (input_.empty()) {
                return false;
            }

            // 检查是否还有更多字符可读
            if (!hasMore()) {
                return false;
            }

            // 检查位置是否有效
            size_t currentPos = position();
            if (currentPos >= input_.length()) {
                return false;
            }

            // 确保有足够的字符可供UTF-8解码
            size_t remainingBytes = input_.length() - currentPos;
            if (remainingBytes == 0) {
                return false;
            }

            // 预检查第一个字节的有效性
            unsigned char firstByte = static_cast<unsigned char>(input_[currentPos]);
            if (!UnicodeCore::isValidUtf8FirstByte(firstByte)) {
                return false;
            }

            // 获取UTF-8序列信息
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

            // 验证序列的有效性
            if (!info.valid) {
                return false;
            }

            // 确保有足够的字节来完成整个UTF-8序列
            if (currentPos + info.length > input_.length()) {
                return false;
            }

            // 验证码点的有效性
            if (!UnicodeCore::isValidCodepoint(info.codepoint)) {
                return false;
            }

            codepoint = info.codepoint;
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }

    size_t UTF8Scanner::lookAhead(size_t n) const {
        if (n == 0 || !hasMore()) {
            return position();
        }

        size_t pos = position();
        size_t count = 0;
        size_t maxLookAhead = 4 * n;  // 最大前瞻字节数（假设每个字符最多4字节）
        size_t bytesChecked = 0;

        while (count < n && pos < input_.length() && bytesChecked < maxLookAhead) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, pos);
            if (!info.valid) {
                pos++;  // 跳过无效字节
                bytesChecked++;
            } else {
                pos += info.length;
                count++;
                bytesChecked += info.length;
            }
        }

        return pos;
    }

    std::string UTF8Scanner::peekString(size_t length) const {
        if (!hasMore() || length == 0) {
            return "";
        }

        std::string result;
        result.reserve(length * 4);  // 预分配足够的空间（最坏情况）

        size_t pos = position();
        size_t charCount = 0;
        size_t maxBytes = length * 4;  // 防止处理过多字节
        size_t bytesProcessed = 0;

        while (charCount < length && pos < input_.length() && bytesProcessed < maxBytes) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, pos);
            if (!info.valid) {
                // 对于无效序列，添加替换字符
                result += static_cast<char>(0xEF);
                result += static_cast<char>(0xBF);
                result += static_cast<char>(0xBD);
                pos++;
                bytesProcessed++;
            } else {
                result.append(input_.substr(pos, info.length));
                pos += info.length;
                bytesProcessed += info.length;
                charCount++;
            }
        }

        return result;
    }

    bool UTF8Scanner::skipUntil(uint32_t targetCodepoint) {
        if (!UnicodeCore::isValidCodepoint(targetCodepoint)) {
            reportInvalidUTF8("Invalid target codepoint for skipUntil");
            return false;
        }

        size_t maxSkip = input_.length() * 2;  // 防止无限循环
        size_t skipped = 0;

        while (hasMore() && skipped < maxSkip) {
            size_t currentPos = position();
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

            if (info.valid) {
                if (info.codepoint == targetCodepoint) {
                    setPosition(currentPos);
                    return true;
                }
                setPosition(currentPos + info.length);
                skipped += info.length;
            } else {
                advance();  // 跳过无效字节
                skipped++;
            }
        }

        if (skipped >= maxSkip) {
            reportInvalidUTF8("Maximum skip limit reached in skipUntil");
        }
        return false;
    }

    std::string UTF8Scanner::collectUntil(uint32_t targetCodepoint) {
        if (!UnicodeCore::isValidCodepoint(targetCodepoint)) {
            reportInvalidUTF8("Invalid target codepoint for collectUntil");
            return "";
        }

        std::string result;
        size_t start = position();
        size_t maxCollect = input_.length() * 2;  // 防止无限循环
        size_t collected = 0;

        while (hasMore() && collected < maxCollect) {
            size_t currentPos = position();
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(input_, currentPos);

            if (info.valid) {
                if (info.codepoint == targetCodepoint) {
                    break;
                }
                setPosition(currentPos + info.length);
                collected += info.length;
            } else {
                advance();  // 跳过无效字节
                collected++;
            }
        }

        size_t end = position();
        if (end > start) {
            if (collected >= maxCollect) {
                reportInvalidUTF8("Maximum collection limit reached in collectUntil");
            }
            result = input_.substr(start, end - start);
        }

        return result;
    }

}  // namespace rp::frontend::unicode
