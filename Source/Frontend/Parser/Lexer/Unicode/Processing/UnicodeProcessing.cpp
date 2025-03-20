#include "UnicodeProcessing.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "../Core/UnicodeCore.h"
#include "../Encoding/UnicodeEncoding.h"

namespace rp::frontend::unicode {

    namespace {
        // 使用更高效的数据结构和更完整的映射表
        struct CaseMapping {
            uint32_t from;
            uint32_t to;
        };

        // 大写映射表（按from值排序）
        constexpr CaseMapping UPPER_CASE_MAP[] = {
            // Basic Latin
            {'a', 'A'},
            {'b', 'B'},
            {'c', 'C'},
            {'d', 'D'},
            {'e', 'E'},
            {'f', 'F'},
            {'g', 'G'},
            {'h', 'H'},
            {'i', 'I'},
            {'j', 'J'},
            {'k', 'K'},
            {'l', 'L'},
            {'m', 'M'},
            {'n', 'N'},
            {'o', 'O'},
            {'p', 'P'},
            {'q', 'Q'},
            {'r', 'R'},
            {'s', 'S'},
            {'t', 'T'},
            {'u', 'U'},
            {'v', 'V'},
            {'w', 'W'},
            {'x', 'X'},
            {'y', 'Y'},
            {'z', 'Z'},
            // Latin-1 Supplement
            {0x00E0, 0x00C0},  // à -> À
            {0x00E1, 0x00C1},  // á -> Á
            {0x00E2, 0x00C2},  // â -> Â
            {0x00E3, 0x00C3},  // ã -> Ã
            {0x00E4, 0x00C4},  // ä -> Ä
            {0x00E5, 0x00C5},  // å -> Å
            {0x00E6, 0x00C6},  // æ -> Æ
            {0x00E7, 0x00C7},  // ç -> Ç
            {0x00E8, 0x00C8},  // è -> È
            {0x00E9, 0x00C9},  // é -> É
            {0x00EA, 0x00CA},  // ê -> Ê
            {0x00EB, 0x00CB},  // ë -> Ë
            {0x00EC, 0x00CC},  // ì -> Ì
            {0x00ED, 0x00CD},  // í -> Í
            {0x00EE, 0x00CE},  // î -> Î
            {0x00EF, 0x00CF},  // ï -> Ï
            // 添加更多映射...
        };

        // 小写映射表（按from值排序）
        constexpr CaseMapping LOWER_CASE_MAP[] = {
            // Basic Latin
            {'A', 'a'},
            {'B', 'b'},
            {'C', 'c'},
            {'D', 'd'},
            {'E', 'e'},
            {'F', 'f'},
            {'G', 'g'},
            {'H', 'h'},
            {'I', 'i'},
            {'J', 'j'},
            {'K', 'k'},
            {'L', 'l'},
            {'M', 'm'},
            {'N', 'n'},
            {'O', 'o'},
            {'P', 'p'},
            {'Q', 'q'},
            {'R', 'r'},
            {'S', 's'},
            {'T', 't'},
            {'U', 'u'},
            {'V', 'v'},
            {'W', 'w'},
            {'X', 'x'},
            {'Y', 'y'},
            {'Z', 'z'},
            // Latin-1 Supplement
            {0x00C0, 0x00E0},  // À -> à
            {0x00C1, 0x00E1},  // Á -> á
            {0x00C2, 0x00E2},  // Â -> â
            {0x00C3, 0x00E3},  // Ã -> ã
            {0x00C4, 0x00E4},  // Ä -> ä
            {0x00C5, 0x00E5},  // Å -> å
            {0x00C6, 0x00E6},  // Æ -> æ
            {0x00C7, 0x00E7},  // Ç -> ç
            {0x00C8, 0x00E8},  // È -> è
            {0x00C9, 0x00E9},  // É -> é
            {0x00CA, 0x00EA},  // Ê -> ê
            {0x00CB, 0x00EB},  // Ë -> ë
            {0x00CC, 0x00EC},  // Ì -> ì
            {0x00CD, 0x00ED},  // Í -> í
            {0x00CE, 0x00EE},  // Î -> î
            {0x00CF, 0x00EF},  // Ï -> ï
            // 添加更多映射...
        };

        // 使用二分查找在映射表中查找字符
        uint32_t findCaseMapping(uint32_t codepoint, const CaseMapping *map, size_t size) {
            size_t left = 0;
            size_t right = size - 1;

            while (left <= right) {
                size_t mid = (left + right) / 2;
                if (map[mid].from == codepoint) {
                    return map[mid].to;
                }
                if (map[mid].from < codepoint) {
                    left = mid + 1;
                } else {
                    if (mid == 0) break;
                    right = mid - 1;
                }
            }
            return codepoint;  // 如果没有找到映射，返回原始码点
        }

        // 快速ASCII大小写转换表
        constexpr uint8_t ASCII_CASE_MAP[128] = {
            0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,
            19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,
            38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,
            57,  58,  59,  60,  61,  62,  63,  64,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107,
            108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91,  92,  93,  94,
            95,  96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,
            82,  83,  84,  85,  86,  87,  88,  89,  90,  123, 124, 125, 126, 127};
    }  // namespace

    // 组合字符映射
    struct CombiningChar {
        uint32_t base;       // 基础字符
        uint32_t combining;  // 组合字符
        uint32_t result;     // 组合结果
    };

    namespace {
        // 按base和combining排序的组合字符映射表
        constexpr CombiningChar COMBINING_CHARS[] = {
            // Latin字母基本组合
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
            {0x0049, 0x0300, 0x00CC},  // I + ` -> Ì
            {0x0049, 0x0301, 0x00CD},  // I + ´ -> Í
            {0x0049, 0x0302, 0x00CE},  // I + ˆ -> Î
            {0x0049, 0x0308, 0x00CF},  // I + ¨ -> Ï
            {0x004F, 0x0300, 0x00D2},  // O + ` -> Ò
            {0x004F, 0x0301, 0x00D3},  // O + ´ -> Ó
            {0x004F, 0x0302, 0x00D4},  // O + ˆ -> Ô
            {0x004F, 0x0303, 0x00D5},  // O + ˜ -> Õ
            {0x004F, 0x0308, 0x00D6},  // O + ¨ -> Ö
            {0x0055, 0x0300, 0x00D9},  // U + ` -> Ù
            {0x0055, 0x0301, 0x00DA},  // U + ´ -> Ú
            {0x0055, 0x0302, 0x00DB},  // U + ˆ -> Û
            {0x0055, 0x0308, 0x00DC},  // U + ¨ -> Ü
            {0x0059, 0x0301, 0x00DD},  // Y + ´ -> Ý
            // 小写字母组合
            {0x0061, 0x0300, 0x00E0},  // a + ` -> à
            {0x0061, 0x0301, 0x00E1},  // a + ´ -> á
            {0x0061, 0x0302, 0x00E2},  // a + ˆ -> â
            {0x0061, 0x0303, 0x00E3},  // a + ˜ -> ã
            {0x0061, 0x0308, 0x00E4},  // a + ¨ -> ä
            {0x0061, 0x030A, 0x00E5},  // a + ˚ -> å
            {0x0065, 0x0300, 0x00E8},  // e + ` -> è
            {0x0065, 0x0301, 0x00E9},  // e + ´ -> é
            {0x0065, 0x0302, 0x00EA},  // e + ˆ -> ê
            {0x0065, 0x0308, 0x00EB},  // e + ¨ -> ë
            {0x0069, 0x0300, 0x00EC},  // i + ` -> ì
            {0x0069, 0x0301, 0x00ED},  // i + ´ -> í
            {0x0069, 0x0302, 0x00EE},  // i + ˆ -> î
            {0x0069, 0x0308, 0x00EF},  // i + ¨ -> ï
            {0x006F, 0x0300, 0x00F2},  // o + ` -> ò
            {0x006F, 0x0301, 0x00F3},  // o + ´ -> ó
            {0x006F, 0x0302, 0x00F4},  // o + ˆ -> ô
            {0x006F, 0x0303, 0x00F5},  // o + ˜ -> õ
            {0x006F, 0x0308, 0x00F6},  // o + ¨ -> ö
            {0x0075, 0x0300, 0x00F9},  // u + ` -> ù
            {0x0075, 0x0301, 0x00FA},  // u + ´ -> ú
            {0x0075, 0x0302, 0x00FB},  // u + ˆ -> û
            {0x0075, 0x0308, 0x00FC},  // u + ¨ -> ü
            {0x0079, 0x0301, 0x00FD},  // y + ´ -> ý
            {0x0079, 0x0308, 0x00FF},  // y + ¨ -> ÿ
            // 添加更多组合...
        };

        // 使用二分查找在组合字符映射表中查找
        uint32_t findCombiningChar(uint32_t base, uint32_t combining) {
            size_t left = 0;
            size_t right = sizeof(COMBINING_CHARS) / sizeof(CombiningChar) - 1;

            while (left <= right) {
                size_t mid = (left + right) / 2;
                const CombiningChar &curr = COMBINING_CHARS[mid];

                if (curr.base == base) {
                    if (curr.combining == combining) {
                        return curr.result;
                    }
                    if (curr.combining < combining) {
                        left = mid + 1;
                    } else {
                        if (mid == 0) break;
                        right = mid - 1;
                    }
                } else if (curr.base < base) {
                    left = mid + 1;
                } else {
                    if (mid == 0) break;
                    right = mid - 1;
                }
            }

            return 0;  // 没有找到组合
        }

        // 检查字符是否为组合标记
        bool isCombiningMark(uint32_t codepoint) {
            // 常见的组合标记范围
            return (codepoint >= 0x0300 && codepoint <= 0x036F) ||  // Combining Diacritical Marks
                   (codepoint >= 0x1AB0 && codepoint <= 0x1AFF) ||  // Combining Diacritical Marks Extended
                   (codepoint >= 0x1DC0 && codepoint <= 0x1DFF) ||  // Combining Diacritical Marks Supplement
                   (codepoint >= 0x20D0 && codepoint <= 0x20FF) ||  // Combining Diacritical Marks for Symbols
                   (codepoint >= 0xFE20 && codepoint <= 0xFE2F);    // Combining Half Marks
        }
    }  // namespace

    UnicodeProcessing::CharacterResult UnicodeProcessing::processCharacter(const std::string &str, size_t start) {
        // 边界检查
        if (str.empty() || start >= str.length()) {
            return {false, 0, 0, "Empty input or invalid start position: " + std::to_string(start)};
        }

        // 使用Core模块验证UTF-8序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, start);
        if (!info.valid) {
            std::stringstream ss;
            ss << "Invalid UTF-8 sequence at position " << start << ": " << info.error << " (byte: 0x" << std::hex
               << std::uppercase << static_cast<int>(static_cast<unsigned char>(str[start])) << ")";
            return {false, 0, 0, ss.str()};
        }

        return {true, info.codepoint, info.length, ""};
    }

    UnicodeProcessing::CharacterResult UnicodeProcessing::processUtf8Character(const std::string &str, size_t start) {
        return processCharacter(str, start);
    }

    UnicodeProcessing::StringResult UnicodeProcessing::processUtf8String(const std::string &str,
                                                                         size_t start,
                                                                         size_t length) {
        // 边界检查
        if (str.empty() || start >= str.length()) {
            return {false, "", 0, "Empty input or invalid start position: " + std::to_string(start)};
        }

        // 调整长度
        if (start + length > str.length()) {
            length = str.length() - start;
        }

        // 预分配空间（假设大多数字符是ASCII）
        std::string result;
        result.reserve(length);

        size_t pos = start;
        size_t endPos = start + length;
        size_t processedBytes = 0;

        while (pos < endPos) {
            // 处理单个字符
            auto charResult = processCharacter(str, pos);
            if (!charResult.success) {
                std::stringstream ss;
                ss << "Failed to process UTF-8 string at position " << pos << " (processed " << processedBytes
                   << " bytes): " << charResult.error;
                return {false, "", processedBytes, ss.str()};
            }

            // 编码并添加到结果
            std::string encoded = UnicodeEncoding::codePointToUtf8(charResult.value);
            if (encoded.empty()) {
                std::stringstream ss;
                ss << "Failed to encode Unicode codepoint U+" << std::hex << std::uppercase << charResult.value
                   << " at position " << pos;
                return {false, "", processedBytes, ss.str()};
            }

            result += encoded;
            pos += charResult.consumed;
            processedBytes += charResult.consumed;
        }

        return {true, result, processedBytes, ""};
    }

    bool UnicodeProcessing::validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed) {
        // 边界检查
        if (start >= str.length()) {
            bytesConsumed = 0;
            return false;
        }

        // 使用Core模块验证序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, start);
        bytesConsumed = info.valid ? info.length : 0;
        return info.valid;
    }

    bool UnicodeProcessing::validateUtf8String(const std::string &str, std::string &error) {
        if (str.empty()) {
            return true;  // 空字符串是有效的UTF-8
        }

        size_t pos = 0;
        size_t totalBytes = 0;

        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (!info.valid) {
                std::stringstream ss;
                ss << "Invalid UTF-8 sequence at position " << pos << " (after " << totalBytes
                   << " valid bytes): " << info.error << std::endl
                   << "Byte sequence:";

                // 显示出错位置的字节序列（最多显示8个字节）
                for (size_t i = 0; i < 8 && pos + i < str.length(); ++i) {
                    ss << " " << std::hex << std::uppercase
                       << static_cast<int>(static_cast<unsigned char>(str[pos + i]));
                }

                error = ss.str();
                return false;
            }
            pos += info.length;
            totalBytes += info.length;
        }

        return true;
    }

    size_t UnicodeProcessing::utf8Length(const std::string &str) {
        if (str.empty()) {
            return 0;
        }

        size_t length = 0;
        size_t pos = 0;

        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (info.valid) {
                length++;
                pos += info.length;
            } else {
                // 对于无效序列，每个字节计为一个字符
                pos++;
                length++;
            }
        }

        return length;
    }

    std::string_view UnicodeProcessing::getNextUtf8Char(const std::string &str, size_t &pos) {
        if (pos >= str.length()) {
            return std::string_view();
        }

        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
        if (info.valid) {
            std::string_view result(str.data() + pos, info.length);
            pos += info.length;
            return result;
        }

        // 对于无效序列，返回单个字节并前进一个位置
        std::string_view result(str.data() + pos, 1);
        pos++;
        return result;
    }

    bool UnicodeProcessing::isValidUtf8(const std::string &str) {
        std::string error;
        return validateUtf8String(str, error);
    }

    int UnicodeProcessing::getCharWidth(uint32_t codepoint) {
        if (!UnicodeCore::isValidCodepoint(codepoint)) {
            return 0;
        }

        // 控制字符和特殊字符
        if (codepoint < 0x20 || (codepoint >= 0x7F && codepoint < 0xA0)) {
            return 0;
        }

        // ASCII字符
        if (codepoint < 0x7F) {
            return 1;
        }

        // 东亚宽字符范围
        if ((codepoint >= 0x1100 && codepoint <= 0x115F) ||  // Hangul Jamo
            (codepoint >= 0x2E80 && codepoint <= 0x9FFF) ||  // CJK
            (codepoint >= 0xAC00 && codepoint <= 0xD7A3) ||  // Hangul Syllables
            (codepoint >= 0xF900 && codepoint <= 0xFAFF) ||  // CJK Compatibility Ideographs
            (codepoint >= 0xFE10 && codepoint <= 0xFE19) ||  // Vertical Forms
            (codepoint >= 0xFE30 && codepoint <= 0xFE6F) ||  // CJK Compatibility Forms
            (codepoint >= 0xFF00 && codepoint <= 0xFF60) ||  // Fullwidth Forms
            (codepoint >= 0xFFE0 && codepoint <= 0xFFE6)) {  // Fullwidth Forms
            return 2;
        }

        // 其他Unicode字符
        return 1;
    }

    std::string UnicodeProcessing::normalize(const std::string &str, bool compose) {
        if (str.empty()) {
            return str;
        }

        std::vector<uint32_t> codepoints;
        codepoints.reserve(str.length());  // 预分配空间

        // 解码UTF-8字符串
        size_t pos = 0;
        while (pos < str.length()) {
            auto result = processCharacter(str, pos);
            if (result.success) {
                codepoints.push_back(result.value);
                pos += result.consumed;
            } else {
                // 对于无效字符，添加原始字节
                codepoints.push_back(static_cast<unsigned char>(str[pos]));
                pos++;
            }
        }

        if (compose) {
            // 执行组合
            std::vector<uint32_t> composed;
            composed.reserve(codepoints.size());

            for (size_t i = 0; i < codepoints.size(); ++i) {
                uint32_t current = codepoints[i];

                // 检查下一个字符是否是组合标记
                if (i + 1 < codepoints.size() && isCombiningMark(codepoints[i + 1])) {
                    // 尝试组合字符
                    uint32_t combined = findCombiningChar(current, codepoints[i + 1]);
                    if (combined != 0) {
                        composed.push_back(combined);
                        i++;  // 跳过组合标记
                        continue;
                    }
                }
                composed.push_back(current);
            }
            codepoints = std::move(composed);
        }

        // 编码回UTF-8
        std::string result;
        result.reserve(str.length() * 2);  // 预留足够空间
        for (uint32_t cp : codepoints) {
            std::string encoded = UnicodeEncoding::codePointToUtf8(cp);
            if (!encoded.empty()) {
                result += encoded;
            }
        }

        return result;
    }

    std::string UnicodeProcessing::toUpper(const std::string &str) {
        if (str.empty()) {
            return str;
        }

        std::string result;
        result.reserve(str.length() * 2);  // 预留足够空间

        size_t pos = 0;
        while (pos < str.length()) {
            auto charResult = processCharacter(str, pos);
            if (charResult.success) {
                uint32_t cp = charResult.value;

                // ASCII快速路径
                if (cp < 128) {
                    result += static_cast<char>(ASCII_CASE_MAP[cp]);
                    pos += charResult.consumed;
                    continue;
                }

                // 使用二分查找在大写映射表中查找
                uint32_t upperCp = findCaseMapping(cp, UPPER_CASE_MAP, sizeof(UPPER_CASE_MAP) / sizeof(CaseMapping));

                // 编码并添加到结果
                std::string encoded = UnicodeEncoding::codePointToUtf8(upperCp);
                if (!encoded.empty()) {
                    result += encoded;
                }
                pos += charResult.consumed;
            } else {
                // 对于无效字符，保持原样
                result += str[pos];
                pos++;
            }
        }

        return result;
    }

    std::string UnicodeProcessing::toLower(const std::string &str) {
        if (str.empty()) {
            return str;
        }

        std::string result;
        result.reserve(str.length() * 2);  // 预留足够空间

        size_t pos = 0;
        while (pos < str.length()) {
            auto charResult = processCharacter(str, pos);
            if (charResult.success) {
                uint32_t cp = charResult.value;

                // ASCII快速路径
                if (cp < 128) {
                    result += static_cast<char>(ASCII_CASE_MAP[cp]);
                    pos += charResult.consumed;
                    continue;
                }

                // 使用二分查找在小写映射表中查找
                uint32_t lowerCp = findCaseMapping(cp, LOWER_CASE_MAP, sizeof(LOWER_CASE_MAP) / sizeof(CaseMapping));

                // 编码并添加到结果
                std::string encoded = UnicodeEncoding::codePointToUtf8(lowerCp);
                if (!encoded.empty()) {
                    result += encoded;
                }
                pos += charResult.consumed;
            } else {
                // 对于无效字符，保持原样
                result += str[pos];
                pos++;
            }
        }

        return result;
    }

    std::string UnicodeProcessing::toTitleCase(const std::string &str) {
        if (str.empty()) {
            return str;
        }

        std::string result;
        result.reserve(str.length() * 2);  // 预留足够空间

        size_t pos = 0;
        bool firstChar = true;
        bool afterSpace = true;  // 用于处理多个单词

        while (pos < str.length()) {
            auto charResult = processCharacter(str, pos);
            if (charResult.success) {
                uint32_t cp = charResult.value;

                // 检查是否是空白字符
                if (UnicodeCore::getUtf8SequenceInfo(str, pos).codepoint == ' ') {
                    result += ' ';
                    afterSpace = true;
                    pos++;
                    continue;
                }

                if (firstChar || afterSpace) {
                    // 首字符或空格后的字符转换为大写
                    uint32_t upperCp =
                        findCaseMapping(cp, UPPER_CASE_MAP, sizeof(UPPER_CASE_MAP) / sizeof(CaseMapping));
                    std::string encoded = UnicodeEncoding::codePointToUtf8(upperCp);
                    if (!encoded.empty()) {
                        result += encoded;
                    }
                    firstChar = false;
                    afterSpace = false;
                } else {
                    // 其他字符转换为小写
                    uint32_t lowerCp =
                        findCaseMapping(cp, LOWER_CASE_MAP, sizeof(LOWER_CASE_MAP) / sizeof(CaseMapping));
                    std::string encoded = UnicodeEncoding::codePointToUtf8(lowerCp);
                    if (!encoded.empty()) {
                        result += encoded;
                    }
                }
                pos += charResult.consumed;
            } else {
                // 对于无效字符，保持原样
                result += str[pos];
                pos++;
            }
        }

        return result;
    }

}  // namespace rp::frontend::unicode
