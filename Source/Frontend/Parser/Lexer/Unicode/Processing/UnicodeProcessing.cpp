#include "UnicodeProcessing.h"

#include <unordered_map>
#include <vector>

#include "../Core/UnicodeCore.h"
#include "../Encoding/UnicodeEncoding.h"

namespace rp::frontend::unicode {

    namespace {
        // 使用unordered_map提高查找效率
        std::unordered_map<uint32_t, uint32_t> createUpperMap() {
            std::unordered_map<uint32_t, uint32_t> map;
            // Basic Latin
            for (uint32_t i = 'a'; i <= 'z'; ++i) {
                map[i] = i - 'a' + 'A';
            }
            // Latin-1 Supplement
            map[0x00E0] = 0x00C0;  // à -> À
            map[0x00E1] = 0x00C1;  // á -> Á
            map[0x00E2] = 0x00C2;  // â -> Â
            map[0x00E3] = 0x00C3;  // ã -> Ã
            map[0x00E4] = 0x00C4;  // ä -> Ä
            map[0x00E5] = 0x00C5;  // å -> Å
            // 添加更多映射...
            return map;
        }

        std::unordered_map<uint32_t, uint32_t> createLowerMap() {
            std::unordered_map<uint32_t, uint32_t> map;
            // Basic Latin
            for (uint32_t i = 'A'; i <= 'Z'; ++i) {
                map[i] = i - 'A' + 'a';
            }
            // Latin-1 Supplement
            map[0x00C0] = 0x00E0;  // À -> à
            map[0x00C1] = 0x00E1;  // Á -> á
            map[0x00C2] = 0x00E2;  // Â -> â
            map[0x00C3] = 0x00E3;  // Ã -> ã
            map[0x00C4] = 0x00E4;  // Ä -> ä
            map[0x00C5] = 0x00E5;  // Å -> å
            // 添加更多映射...
            return map;
        }

        // 静态映射表
        const std::unordered_map<uint32_t, uint32_t> upperCaseMap = createUpperMap();
        const std::unordered_map<uint32_t, uint32_t> lowerCaseMap = createLowerMap();

    }  // namespace

    // 组合字符映射
    struct CombiningPair {
        uint32_t base;
        uint32_t combining;

        bool operator==(const CombiningPair &other) const { return base == other.base && combining == other.combining; }
    };
}  // namespace rp::frontend::unicode

namespace std {
    template <>
    struct hash<rp::frontend::unicode::CombiningPair> {
        size_t operator()(const rp::frontend::unicode::CombiningPair &pair) const {
            return hash<uint64_t>()((static_cast<uint64_t>(pair.base) << 32) | pair.combining);
        }
    };
}  // namespace std

namespace rp::frontend::unicode {
    namespace {
        std::unordered_map<CombiningPair, uint32_t> createCombiningMap() {
            std::unordered_map<CombiningPair, uint32_t> map;
            map[{0x0041, 0x0300}] = 0x00C0;  // A + ` -> À
            map[{0x0041, 0x0301}] = 0x00C1;  // A + ´ -> Á
            map[{0x0041, 0x0302}] = 0x00C2;  // A + ˆ -> Â
            // 添加更多组合...
            return map;
        }

        const auto combiningMap = createCombiningMap();
    }  // namespace

    UnicodeProcessing::CharacterResult UnicodeProcessing::processCharacter(const std::string &str, size_t start) {
        if (str.empty() || start >= str.length()) {
            return {false, 0, 0, "Empty input or invalid start position"};
        }

        // 使用Core模块验证UTF-8序列
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, start);
        if (!info.valid) {
            return {false, 0, 0, info.error};
        }

        return {true, info.codepoint, info.length, ""};
    }

    UnicodeProcessing::CharacterResult UnicodeProcessing::processUtf8Character(const std::string &str, size_t start) {
        return processCharacter(str, start);
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
        result.reserve(length);  // 预分配空间
        size_t pos = start;
        size_t endPos = start + length;

        while (pos < endPos) {
            auto charResult = processCharacter(str, pos);
            if (!charResult.success) {
                return {false, "", pos - start, charResult.error};
            }

            // 使用Encoding模块进行UTF-8编码
            result += UnicodeEncoding::codePointToUtf8(charResult.value);
            pos += charResult.consumed;
        }

        return {true, result, pos - start, ""};
    }

    bool UnicodeProcessing::validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed) {
        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, start);
        bytesConsumed = info.valid ? info.length : 0;
        return info.valid;
    }

    bool UnicodeProcessing::validateUtf8String(const std::string &str, std::string &error) {
        size_t pos = 0;
        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (!info.valid) {
                error = "Invalid UTF-8 sequence at position " + std::to_string(pos) + ": " + info.error;
                return false;
            }
            pos += info.length;
        }
        return true;
    }

    size_t UnicodeProcessing::utf8Length(const std::string &str) {
        size_t length = 0;
        size_t pos = 0;
        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (info.valid) {
                length++;
                pos += info.length;
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

        UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
        if (info.valid) {
            std::string_view result(str.data() + pos, info.length);
            pos += info.length;
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
                pos++;  // 跳过无效字符
            }
        }

        if (compose) {
            // 执行组合
            std::vector<uint32_t> composed;
            composed.reserve(codepoints.size());

            for (size_t i = 0; i < codepoints.size(); ++i) {
                if (i + 1 < codepoints.size()) {
                    CombiningPair pair = {codepoints[i], codepoints[i + 1]};
                    auto it = combiningMap.find(pair);
                    if (it != combiningMap.end()) {
                        composed.push_back(it->second);
                        i++;  // 跳过下一个字符
                        continue;
                    }
                }
                composed.push_back(codepoints[i]);
            }
            codepoints = std::move(composed);
        }

        // 编码回UTF-8
        std::string result;
        result.reserve(str.length());
        for (uint32_t cp : codepoints) {
            result += UnicodeEncoding::codePointToUtf8(cp);
        }

        return result;
    }

    std::string UnicodeProcessing::toUpper(const std::string &str) {
        std::vector<uint32_t> codepoints;
        codepoints.reserve(str.length());

        // 解码UTF-8字符串
        size_t pos = 0;
        while (pos < str.length()) {
            auto result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;
                auto it = upperCaseMap.find(cp);
                codepoints.push_back(it != upperCaseMap.end() ? it->second : cp);
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 编码回UTF-8
        std::string result;
        result.reserve(str.length());
        for (uint32_t cp : codepoints) {
            result += UnicodeEncoding::codePointToUtf8(cp);
        }

        return result;
    }

    std::string UnicodeProcessing::toLower(const std::string &str) {
        std::vector<uint32_t> codepoints;
        codepoints.reserve(str.length());

        // 解码UTF-8字符串
        size_t pos = 0;
        while (pos < str.length()) {
            auto result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;
                auto it = lowerCaseMap.find(cp);
                codepoints.push_back(it != lowerCaseMap.end() ? it->second : cp);
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 编码回UTF-8
        std::string result;
        result.reserve(str.length());
        for (uint32_t cp : codepoints) {
            result += UnicodeEncoding::codePointToUtf8(cp);
        }

        return result;
    }

    std::string UnicodeProcessing::toTitleCase(const std::string &str) {
        if (str.empty()) return str;

        std::vector<uint32_t> codepoints;
        codepoints.reserve(str.length());
        bool firstChar = true;

        // 解码UTF-8字符串
        size_t pos = 0;
        while (pos < str.length()) {
            auto result = processCharacter(str, pos);
            if (result.success) {
                uint32_t cp = result.value;
                if (firstChar) {
                    auto it = upperCaseMap.find(cp);
                    codepoints.push_back(it != upperCaseMap.end() ? it->second : cp);
                    firstChar = false;
                } else {
                    auto it = lowerCaseMap.find(cp);
                    codepoints.push_back(it != lowerCaseMap.end() ? it->second : cp);
                }
                pos += result.consumed;
            } else {
                pos++;
            }
        }

        // 编码回UTF-8
        std::string result;
        result.reserve(str.length());
        for (uint32_t cp : codepoints) {
            result += UnicodeEncoding::codePointToUtf8(cp);
        }

        return result;
    }

}  // namespace rp::frontend::unicode
