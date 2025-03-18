#include "UnicodeEncoding.h"

#include "../Core/UnicodeCore.h"

namespace rp::frontend::unicode {

    std::string UnicodeEncoding::encodeUtf8(uint32_t codepoint) {
        // 首先验证码点的有效性
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return "";  // 返回空字符串表示无效码点
        }

        // 获取所需的UTF-8字节数
        size_t byteCount = UnicodeCore::getUtf8ByteCount(codepoint);
        if (byteCount == 0) {
            return "";  // 无效的码点范围
        }

        std::string result;
        result.reserve(byteCount);  // 预分配精确需要的空间

        // 根据UTF-8编码规则进行编码
        switch (byteCount) {
            case 1:
                // 1字节编码 (0xxxxxxx)
                result.push_back(static_cast<char>(codepoint));
                break;

            case 2:
                // 2字节编码 (110xxxxx 10xxxxxx)
                result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                break;

            case 3:
                // 3字节编码 (1110xxxx 10xxxxxx 10xxxxxx)
                result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                break;

            case 4:
                // 4字节编码 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
                result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
                break;

            default:
                return "";  // 不应该发生
        }

        // 验证生成的UTF-8序列
        size_t bytesRead;
        uint32_t decodedCodepoint = utf8ToCodePoint(result, bytesRead);
        if (decodedCodepoint != codepoint || bytesRead != byteCount) {
            return "";  // 编码验证失败
        }

        return result;
    }

    bool UnicodeEncoding::decodeUtf8(const std::string &utf8, std::vector<uint32_t> &codepoints) {
        codepoints.clear();

        // 估计码点数量（UTF-8字符平均长度约为1.5字节）
        codepoints.reserve(utf8.length() * 2 / 3);

        size_t i = 0;
        while (i < utf8.length()) {
            // 获取UTF-8序列信息
            auto seqInfo = UnicodeCore::getUtf8SequenceInfo(utf8, i);

            // 检查序列是否有效
            if (!seqInfo.valid) {
                codepoints.clear();
                return false;
            }

            // 添加有效的码点
            codepoints.push_back(seqInfo.codepoint);
            i += seqInfo.length;
        }

        // 收缩容器到实际大小
        codepoints.shrink_to_fit();
        return true;
    }

    uint32_t UnicodeEncoding::utf8ToCodePoint(std::string_view sv, size_t &bytesRead) {
        bytesRead = 0;
        if (sv.empty()) {
            return 0;  // 空序列
        }

        // 获取首字节
        unsigned char firstByte = static_cast<unsigned char>(sv[0]);

        // 获取序列长度
        size_t length = UnicodeCore::getUtf8SequenceLength(firstByte);
        if (length == 0) {
            return 0;  // 无效的UTF-8首字节
        }

        // 检查序列完整性
        if (length > sv.length()) {
            return 0;  // 不完整的UTF-8序列
        }

        uint32_t codepoint;

        // 处理ASCII字符的快速路径
        if (length == 1) {
            codepoint = firstByte;
            bytesRead = 1;
            return codepoint;
        }

        // 验证后续字节
        for (size_t i = 1; i < length; i++) {
            if (!UnicodeCore::isUtf8ContinuationByte(static_cast<unsigned char>(sv[i]))) {
                return 0;  // 无效的后续字节
            }
        }

        // 根据UTF-8编码规则解码多字节序列
        switch (length) {
            case 2:
                // 2字节序列：110xxxxx 10xxxxxx
                codepoint = ((firstByte & 0x1F) << 6) | (static_cast<unsigned char>(sv[1]) & 0x3F);
                // 检查是否为过长编码
                if (codepoint < 0x80) {
                    return 0;
                }
                break;

            case 3:
                // 3字节序列：1110xxxx 10xxxxxx 10xxxxxx
                codepoint = ((firstByte & 0x0F) << 12) | ((static_cast<unsigned char>(sv[1]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(sv[2]) & 0x3F);
                // 检查是否为过长编码或代理对码点
                if (codepoint < 0x800 || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                    return 0;
                }
                break;

            case 4:
                // 4字节序列：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                codepoint = ((firstByte & 0x07) << 18) | ((static_cast<unsigned char>(sv[1]) & 0x3F) << 12) |
                            ((static_cast<unsigned char>(sv[2]) & 0x3F) << 6) |
                            (static_cast<unsigned char>(sv[3]) & 0x3F);
                // 检查是否为过长编码或超出Unicode范围
                if (codepoint < 0x10000 || codepoint > 0x10FFFF) {
                    return 0;
                }
                break;

            default:
                return 0;  // 不应该发生
        }

        // 验证解码出的码点
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return 0;
        }

        bytesRead = length;
        return codepoint;
    }

    std::string UnicodeEncoding::codePointToUtf8(uint32_t codePoint) {
        // 直接使用encodeUtf8，它已经包含了所有必要的验证
        return encodeUtf8(codePoint);
    }

    std::tuple<uint32_t, size_t> UnicodeEncoding::getMultiByteChar(const std::string &str, size_t start) {
        // 边界检查
        if (start >= str.length()) {
            return {0, 0};
        }

        // 使用UnicodeCore获取序列信息
        auto seqInfo = UnicodeCore::getUtf8SequenceInfo(str, start);

        // 如果序列无效，返回错误值
        if (!seqInfo.valid) {
            return {0, 0};
        }

        return {seqInfo.codepoint, seqInfo.length};
    }

    bool UnicodeEncoding::isMultiByteChar(const std::string &str, size_t start) {
        // 边界检查
        if (start >= str.length()) {
            return false;
        }

        unsigned char firstByte = static_cast<unsigned char>(str[start]);

        // 检查是否是有效的UTF-8首字节且不是ASCII字符
        if (!UnicodeCore::isValidUtf8FirstByte(firstByte)) {
            return false;
        }

        // 检查是否是多字节字符（首字节最高位为1）
        return (firstByte & 0x80) != 0;
    }

    size_t UnicodeEncoding::getMultiByteCharLength(unsigned char firstByte) {
        // 使用UnicodeCore的序列长度检查
        return UnicodeCore::getUtf8SequenceLength(firstByte);
    }

}  // namespace rp::frontend::unicode
