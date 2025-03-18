#pragma once

// 导出所有Unicode相关功能
#include <string>
#include <string_view>
#include <vector>

#include "Categories/UnicodeCategories.h"
#include "Core/UnicodeCore.h"
#include "Encoding/UnicodeEncoding.h"
#include "Escape/UnicodeEscape.h"
#include "Processing/UnicodeProcessing.h"
#include "Scanner/UTF8Scanner.h"

namespace rp::frontend::unicode {

    /**
     * @brief Unicode处理模块
     *
     * 该模块提供了完整的Unicode字符和字符串处理功能：
     * - Unicode字符分类和属性查询
     * - UTF-8编码和解码
     * - Unicode转义序列处理
     * - Unicode字符串规范化和转换
     * - UTF-8字符串扫描和验证
     */

    // 常用类型别名
    using CodePoint = uint32_t;
    using Char32 = char32_t;
    using StringView = std::string_view;

    // 错误处理结构
    struct UnicodeError {
        std::string message;
        size_t position;
        std::string context;
    };

    // 字符串验证结果
    struct ValidationResult {
        bool valid;
        std::vector<UnicodeError> errors;
    };

    // 便利函数

    /**
     * @brief 验证UTF-8字符串
     * @param str 要验证的字符串
     * @return ValidationResult 验证结果
     */
    inline ValidationResult validateString(const std::string& str) {
        ValidationResult result;
        result.valid = true;

        size_t pos = 0;
        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (!info.valid) {
                result.valid = false;
                UnicodeError error;
                error.message = info.error;
                error.position = pos;

                // 添加上下文信息
                size_t contextStart = (pos > 10) ? pos - 10 : 0;
                size_t contextLength = std::min(20ul, str.length() - contextStart);
                error.context = str.substr(contextStart, contextLength);

                result.errors.push_back(error);
                pos++;  // 跳过无效字节
            } else {
                pos += info.length;
            }
        }

        return result;
    }

    /**
     * @brief 将字符串转换为码点序列
     * @param str UTF-8字符串
     * @return std::vector<CodePoint> 码点序列
     */
    inline std::vector<CodePoint> toCodePoints(const std::string& str) {
        std::vector<CodePoint> codepoints;
        std::vector<uint32_t> temp;
        if (UnicodeEncoding::decodeUtf8(str, temp)) {
            codepoints.assign(temp.begin(), temp.end());
        }
        return codepoints;
    }

    /**
     * @brief 将码点序列转换为UTF-8字符串
     * @param codepoints 码点序列
     * @return std::string UTF-8字符串
     */
    inline std::string fromCodePoints(const std::vector<CodePoint>& codepoints) {
        std::string result;
        for (CodePoint cp : codepoints) {
            result += UnicodeEncoding::encodeUtf8(cp);
        }
        return result;
    }

    /**
     * @brief 格式化Unicode码点
     * @param codepoint Unicode码点
     * @return std::string 格式化的字符串（如"U+0041"）
     */
    inline std::string formatCodePoint(CodePoint codepoint) {
        char buffer[16];
        if (codepoint <= 0xFFFF) {
            snprintf(buffer, sizeof(buffer), "U+%04X", codepoint);
        } else {
            snprintf(buffer, sizeof(buffer), "U+%06X", codepoint);
        }
        return std::string(buffer);
    }

    /**
     * @brief 检查字符串是否只包含指定类别的字符
     * @param str 要检查的字符串
     * @param category 字符类别
     * @return bool 是否所有字符都属于指定类别
     */
    inline bool isCategory(const std::string& str, UnicodeCategories::Category category) {
        size_t pos = 0;
        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (!info.valid) return false;
            if (!UnicodeCategories::isInCategory(info.codepoint, category)) return false;
            pos += info.length;
        }
        return true;
    }

    /**
     * @brief 计算字符串的显示宽度
     * @param str UTF-8字符串
     * @return size_t 显示宽度（考虑全角字符）
     */
    inline size_t displayWidth(const std::string& str) {
        size_t width = 0;
        size_t pos = 0;
        while (pos < str.length()) {
            UnicodeCore::Utf8SequenceInfo info = UnicodeCore::getUtf8SequenceInfo(str, pos);
            if (info.valid) {
                width += UnicodeProcessing::getCharWidth(info.codepoint);
                pos += info.length;
            } else {
                width += 1;
                pos++;
            }
        }
        return width;
    }

    /**
     * @brief 规范化字符串的换行符
     * @param str 输入字符串
     * @param style 换行符风格（"LF", "CRLF", "CR"）
     * @return std::string 规范化后的字符串
     */
    inline std::string normalizeLineEndings(const std::string& str, const std::string& style = "LF") {
        std::string result;
        result.reserve(str.length());

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\r') {
                if (i + 1 < str.length() && str[i + 1] == '\n') {
                    // CRLF
                    if (style == "LF") {
                        result += '\n';
                    } else if (style == "CRLF") {
                        result += "\r\n";
                    } else if (style == "CR") {
                        result += '\r';
                    }
                    ++i;
                } else {
                    // CR
                    if (style == "LF") {
                        result += '\n';
                    } else if (style == "CRLF") {
                        result += "\r\n";
                    } else if (style == "CR") {
                        result += '\r';
                    }
                }
            } else if (str[i] == '\n') {
                // LF
                if (style == "LF") {
                    result += '\n';
                } else if (style == "CRLF") {
                    result += "\r\n";
                } else if (style == "CR") {
                    result += '\r';
                }
            } else {
                result += str[i];
            }
        }

        return result;
    }

}  // namespace rp::frontend::unicode
