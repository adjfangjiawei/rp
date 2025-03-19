#include "UnicodeProcessing.h"

#include <vector>

#include "../Core/UnicodeCore.h"
#include "../Scanner/UTF8Scanner.h"

namespace rp::frontend::unicode {

    UnicodeProcessing::CharacterResult UnicodeProcessing::processCharacter(const std::string &str, size_t start) {
        if (str.empty() || start >= str.length()) {
            return {false, 0, 0, "Empty input or invalid start position"};
        }

        // 处理ASCII字符
        unsigned char first = static_cast<unsigned char>(str[start]);
        if (first < 0x80) {
            return {true, first, 1, ""};
        }

        // 处理UTF-8字符
        return processUtf8Character(str, start);
    }

    UnicodeProcessing::CharacterResult UnicodeProcessing::processUtf8Character(const std::string &str, size_t start) {
        size_t bytesConsumed;
        if (!validateUtf8Sequence(str, start, bytesConsumed)) {
            return {false, 0, 0, "Invalid UTF-8 sequence"};
        }

        // 解码UTF-8序列
        uint32_t codepoint = 0;
        unsigned char first = static_cast<unsigned char>(str[start]);

        if (first < 0x80) {
            codepoint = first;
        } else if ((first & 0xE0) == 0xC0) {
            // 2字节序列
            codepoint = ((first & 0x1F) << 6) | (static_cast<unsigned char>(str[start + 1]) & 0x3F);
        } else if ((first & 0xF0) == 0xE0) {
            // 3字节序列
            codepoint = ((first & 0x0F) << 12) | ((static_cast<unsigned char>(str[start + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(str[start + 2]) & 0x3F);
        } else if ((first & 0xF8) == 0xF0) {
            // 4字节序列
            codepoint = ((first & 0x07) << 18) | ((static_cast<unsigned char>(str[start + 1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(str[start + 2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(str[start + 3]) & 0x3F);
        }

        return {true, codepoint, bytesConsumed, ""};
    }

    UnicodeProcessing::StringResult UnicodeProcessing::processUtf8String(const std::string &str,
                                                                         size_t start,
                                                                         size_t length) {
        if (str.empty() || start >= str.length()) {
            return {false, "", 0, "Empty input or invalid start position"};
        }

        if (start + length > str.length()) {
            length = str.length() - start;
        }

        std::string result;
        size_t pos = start;
        size_t endPos = start + length;

        while (pos < endPos) {
            size_t bytesConsumed;
            if (!validateUtf8Sequence(str, pos, bytesConsumed)) {
                return {false, "", pos - start, "Invalid UTF-8 sequence at position " + std::to_string(pos)};
            }

            // 复制有效的UTF-8序列到结果字符串
            result.append(str.substr(pos, bytesConsumed));
            pos += bytesConsumed;
        }

        return {true, result, pos - start, ""};
    }

    bool UnicodeProcessing::validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed) {
        if (str.empty() || start >= str.length()) {
            bytesConsumed = 0;
            return false;
        }

        unsigned char first = static_cast<unsigned char>(str[start]);
        if (first < 0x80) {
            // ASCII字符
            bytesConsumed = 1;
            return true;
        }

        // 确定UTF-8序列的长度
        int expectedLength;
        if ((first & 0xE0) == 0xC0)
            expectedLength = 2;  // 110xxxxx
        else if ((first & 0xF0) == 0xE0)
            expectedLength = 3;  // 1110xxxx
        else if ((first & 0xF8) == 0xF0)
            expectedLength = 4;  // 11110xxx
        else {
            // 无效的UTF-8起始字节
            bytesConsumed = 0;
            return false;
        }

        // 检查是否有足够的字节
        if (start + expectedLength > str.length()) {
            bytesConsumed = 0;
            return false;
        }

        // 验证后续字节
        for (int i = 1; i < expectedLength; i++) {
            unsigned char byte = static_cast<unsigned char>(str[start + i]);
            if ((byte & 0xC0) != 0x80) {  // 不是10xxxxxx格式
                bytesConsumed = 0;
                return false;
            }
        }

        bytesConsumed = expectedLength;
        return true;
    }

    bool UnicodeProcessing::validateUtf8String(const std::string &str, std::string &error) {
        size_t pos = 0;
        while (pos < str.length()) {
            size_t bytesConsumed;
            if (!validateUtf8Sequence(str, pos, bytesConsumed)) {
                error = "Invalid UTF-8 sequence at position " + std::to_string(pos);
                return false;
            }
            pos += bytesConsumed;
        }
        return true;
    }

    size_t UnicodeProcessing::utf8Length(const std::string &str) {
        size_t length = 0;
        size_t pos = 0;
        while (pos < str.length()) {
            size_t bytesConsumed;
            if (validateUtf8Sequence(str, pos, bytesConsumed)) {
                length++;
                pos += bytesConsumed;
            } else {
                pos++;  // 跳过无效字节
            }
        }
        return length;
    }

    std::string_view UnicodeProcessing::getNextUtf8Char(const std::string &str, size_t &pos) {
        if (pos >= str.length()) {
            return std::string_view();
        }

        size_t bytesConsumed;
        if (validateUtf8Sequence(str, pos, bytesConsumed)) {
            std::string_view result(str.data() + pos, bytesConsumed);
            pos += bytesConsumed;
            return result;
        }

        // 对于无效序列，返回单个字节
        std::string_view result(str.data() + pos, 1);
        pos++;
        return result;
    }

    bool UnicodeProcessing::isValidUtf8(const std::string &str) {
        std::string error;
        return validateUtf8String(str, error);
    }

    int UnicodeProcessing::getCharWidth(uint32_t codepoint) {
        // 实现基本的字符宽度计算
        if (codepoint == 0) return 0;
        if (codepoint < 0x20) return 0;                            // 控制字符
        if (codepoint < 0x7F) return 1;                            // ASCII
        if (codepoint < 0xA0) return 0;                            // C1控制字符
        if (codepoint >= 0x1100 && codepoint <= 0x115F) return 2;  // Hangul Jamo
        if (codepoint >= 0x2E80 && codepoint <= 0x9FFF) return 2;  // CJK
        if (codepoint >= 0xAC00 && codepoint <= 0xD7A3) return 2;  // Hangul Syllables
        if (codepoint >= 0xF900 && codepoint <= 0xFAFF) return 2;  // CJK Compatibility Ideographs
        if (codepoint >= 0xFE10 && codepoint <= 0xFE19) return 2;  // Vertical Forms
        if (codepoint >= 0xFE30 && codepoint <= 0xFE6F) return 2;  // CJK Compatibility Forms
        if (codepoint >= 0xFF00 && codepoint <= 0xFF60) return 2;  // Fullwidth Forms
        if (codepoint >= 0xFFE0 && codepoint <= 0xFFE6) return 2;  // Fullwidth Forms
        return 1;
    }

    // Unicode正规化和转换函数的基本实现
    // Unicode组合字符数据结构
    struct CombiningCharacter {
        uint32_t base;       // 基础字符
        uint32_t combining;  // 组合字符
        uint32_t composed;   // 组合后的字符
    };

    // 部分常用的组合字符映射（这只是一个示例，实际需要更完整的数据）
    static const CombiningCharacter combiningChars[] = {
        // Latin-1 补充
        {0x0041, 0x0300, 0x00C0},  // A + ` -> À
        {0x0041, 0x0301, 0x00C1},  // A + ´ -> Á
        {0x0041, 0x0302, 0x00C2},  // A + ˆ -> Â
        {0x0041, 0x0303, 0x00C3},  // A + ˜ -> Ã
        {0x0041, 0x0308, 0x00C4},  // A + ¨ -> Ä
        {0x0041, 0x030A, 0x00C5},  // A + ˚ -> Å
        {0x0045, 0x0300, 0x00C8},  // E + ` -> È
        {0x0045, 0x0301, 0x00C9},  // E + ´ -> É
        {0x0045, 0x0302, 0x00CA},  // E + ˆ -> Ê
        {0x0045, 0x0308, 0x00CB},  // E + ¨ -> Ë
        // 可以添加更多组合字符映射
    };

    // 查找组合字符的辅助函数
    static uint32_t findComposedChar(uint32_t base, uint32_t combining) {
        for (const auto &combo : combiningChars) {
            if (combo.base == base && combo.combining == combining) {
                return combo.composed;
            }
        }
        return 0;  // 未找到组合
    }

    std::string UnicodeProcessing::normalize(const std::string &str, bool compose) {
        std::vector<uint32_t> codepoints;

        // 首先将UTF-8字符串转换为码点序列
        size_t pos = 0;
        while (pos < str.length()) {
            CharacterResult result = processCharacter(str, pos);
            if (result.success) {
                codepoints.push_back(result.value);
                pos += result.consumed;
            } else {
                pos++;  // 跳过无效字符
            }
        }

        if (compose) {
            // NFC: 规范组合
            std::vector<uint32_t> composed;
            for (size_t i = 0; i < codepoints.size(); i++) {
                if (i + 1 < codepoints.size()) {
                    uint32_t composedChar = findComposedChar(codepoints[i], codepoints[i + 1]);
                    if (composedChar != 0) {
                        composed.push_back(composedChar);
                        i++;  // 跳过下一个字符
                        continue;
                    }
                }
                composed.push_back(codepoints[i]);
            }
            codepoints = std::move(composed);
        }

        // 将码点序列转换回UTF-8字符串
        std::string result;
        for (uint32_t codepoint : codepoints) {
            if (codepoint < 0x80) {
                result += static_cast<char>(codepoint);
            } else if (codepoint < 0x800) {
                result += static_cast<char>(0xC0 | (codepoint >> 6));
                result += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else if (codepoint < 0x10000) {
                result += static_cast<char>(0xE0 | (codepoint >> 12));
                result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (codepoint & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (codepoint >> 18));
                result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }

        return result;
    }

    // Unicode大小写映射数据结构
    struct CaseMapping {
        uint32_t from;
        uint32_t to;
    };

    // 部分常用的大小写映射（这只是一个示例，实际需要更完整的数据）
    static const CaseMapping upperCaseMap[] = {
        // Latin-1
        {0x00E0, 0x00C0},  // à -> À
        {0x00E1, 0x00C1},  // á -> Á
        {0x00E2, 0x00C2},  // â -> Â
        {0x00E3, 0x00C3},  // ã -> Ã
        {0x00E4, 0x00C4},  // ä -> Ä
        {0x00E5, 0x00C5},  // å -> Å
        // 可以添加更多映射
    };

    static const CaseMapping lowerCaseMap[] = {
        // Latin-1
        {0x00C0, 0x00E0},  // À -> à
        {0x00C1, 0x00E1},  // Á -> á
        {0x00C2, 0x00E2},  // Â -> â
        {0x00C3, 0x00E3},  // Ã -> ã
        {0x00C4, 0x00E4},  // Ä -> ä
        {0x00C5, 0x00E5},  // Å -> å
        // 可以添加更多映射
    };

    // 在映射表中查找对应字符的辅助函数
    static uint32_t findCaseMapping(uint32_t codepoint, const CaseMapping *map, size_t mapSize) {
        for (size_t i = 0; i < mapSize; i++) {
            if (map[i].from == codepoint) {
                return map[i].to;
            }
        }
        return 0;  // 未找到映射
    }

    std::string UnicodeProcessing::toUpper(const std::string &str) {
        std::vector<uint32_t> codepoints;

        // 转换为码点序列
        size_t pos = 0;
        while (pos < str.length()) {
            CharacterResult result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;

                // ASCII转换
                if (cp >= 'a' && cp <= 'z') {
                    cp = cp - 'a' + 'A';
                } else {
                    // Unicode转换
                    uint32_t mapped = findCaseMapping(cp, upperCaseMap, sizeof(upperCaseMap) / sizeof(CaseMapping));
                    if (mapped != 0) {
                        cp = mapped;
                    }
                }

                codepoints.push_back(cp);
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 转换回UTF-8
        std::string result;
        for (uint32_t cp : codepoints) {
            if (cp < 0x80) {
                result += static_cast<char>(cp);
            } else if (cp < 0x800) {
                result += static_cast<char>(0xC0 | (cp >> 6));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                result += static_cast<char>(0xE0 | (cp >> 12));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (cp >> 18));
                result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }

        return result;
    }

    std::string UnicodeProcessing::toLower(const std::string &str) {
        std::vector<uint32_t> codepoints;

        // 转换为码点序列
        size_t pos = 0;
        while (pos < str.length()) {
            CharacterResult result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;

                // ASCII转换
                if (cp >= 'A' && cp <= 'Z') {
                    cp = cp - 'A' + 'a';
                } else {
                    // Unicode转换
                    uint32_t mapped = findCaseMapping(cp, lowerCaseMap, sizeof(lowerCaseMap) / sizeof(CaseMapping));
                    if (mapped != 0) {
                        cp = mapped;
                    }
                }

                codepoints.push_back(cp);
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 转换回UTF-8
        std::string result;
        for (uint32_t cp : codepoints) {
            if (cp < 0x80) {
                result += static_cast<char>(cp);
            } else if (cp < 0x800) {
                result += static_cast<char>(0xC0 | (cp >> 6));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                result += static_cast<char>(0xE0 | (cp >> 12));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (cp >> 18));
                result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }

        return result;
    }

    std::string UnicodeProcessing::toTitleCase(const std::string &str) {
        if (str.empty()) return str;

        std::vector<uint32_t> codepoints;
        bool firstChar = true;

        // 转换为码点序列
        size_t pos = 0;
        while (pos < str.length()) {
            CharacterResult result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;

                if (firstChar) {
                    // 第一个字符转换为大写
                    if (cp >= 'a' && cp <= 'z') {
                        cp = cp - 'a' + 'A';
                    } else {
                        uint32_t mapped = findCaseMapping(cp, upperCaseMap, sizeof(upperCaseMap) / sizeof(CaseMapping));
                        if (mapped != 0) {
                            cp = mapped;
                        }
                    }
                    firstChar = false;
                } else {
                    // 其他字符转换为小写
                    if (cp >= 'A' && cp <= 'Z') {
                        cp = cp - 'A' + 'a';
                    } else {
                        uint32_t mapped = findCaseMapping(cp, lowerCaseMap, sizeof(lowerCaseMap) / sizeof(CaseMapping));
                        if (mapped != 0) {
                            cp = mapped;
                        }
                    }
                }

                codepoints.push_back(cp);
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 转换回UTF-8
        std::string result;
        for (uint32_t cp : codepoints) {
            if (cp < 0x80) {
                result += static_cast<char>(cp);
            } else if (cp < 0x800) {
                result += static_cast<char>(0xC0 | (cp >> 6));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                result += static_cast<char>(0xE0 | (cp >> 12));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            } else {
                result += static_cast<char>(0xF0 | (cp >> 18));
                result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
                result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                result += static_cast<char>(0x80 | (cp & 0x3F));
            }
        }

        return result;
    }

}  // namespace rp::frontend::unicode
