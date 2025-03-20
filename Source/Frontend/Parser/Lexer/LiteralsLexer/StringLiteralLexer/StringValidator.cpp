
#include "StringValidator.h"

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        StringValidator::StringValidator(const StringValidationOptions& options) : validationOptions(options) {}

        bool StringValidator::validate(const std::string& content,
                                       std::string& error,
                                       std::vector<std::string>& warnings) {
            try {
                // 检查长度限制
                if (!checkLengthLimit(content, error)) {
                    return false;
                }

                // 检查行数限制
                if (!checkLineLimit(content, error)) {
                    return false;
                }

                // 检查UTF-8有效性
                if (validationOptions.strictUTF8) {
                    if (!checkUTF8Validity(content, error)) {
                        return false;
                    }
                }

                // 检查控制字符
                if (!validationOptions.allowControlChars) {
                    checkControlChars(content, warnings);
                }

                return true;
            } catch (const std::exception& e) {
                error = "字符串验证时发生错误: " + std::string(e.what());
                return false;
            }
        }

        bool StringValidator::checkLengthLimit(const std::string& content, std::string& error) const {
            size_t effectiveMaxLength =
                validationOptions.maxLength > 0 ? validationOptions.maxLength : getDefaultMaxLength();

            if (content.length() > effectiveMaxLength) {
                error = std::string("字符串超过最大长度限制（") + std::to_string(effectiveMaxLength) +
                        std::string("字节）");
                return false;
            }
            return true;
        }

        bool StringValidator::checkLineLimit(const std::string& content, std::string& error) const {
            if (!validationOptions.allowMultiline) {
                size_t pos = content.find('\n');
                if (pos != std::string::npos) {
                    error = "不允许多行字符串";
                    return false;
                }
                return true;
            }

            size_t lineCount = 1;
            size_t pos = 0;
            size_t effectiveMaxLines =
                validationOptions.maxLines > 0 ? validationOptions.maxLines : getDefaultMaxLines();

            while ((pos = content.find('\n', pos)) != std::string::npos) {
                lineCount++;
                if (lineCount > effectiveMaxLines) {
                    error = "字符串超过最大行数限制（" + std::to_string(effectiveMaxLines) + "行）";
                    return false;
                }
                pos++;
            }
            return true;
        }

        bool StringValidator::checkUTF8Validity(const std::string& content, std::string& error) const {
            size_t pos = 0;
            while (pos < content.length()) {
                if (!processUTF8Char(content, pos, error)) {
                    return false;
                }
            }
            return true;
        }

        void StringValidator::checkControlChars(const std::string& content, std::vector<std::string>& warnings) const {
            size_t pos = 0;
            while (pos < content.length()) {
                unsigned char c = (unsigned char)content[pos];
                if (c < 0x80) {  // 只检查ASCII范围的控制字符
                    // 允许常见的控制字符
                    if (c < 0x20 && c != '\n' && c != '\r' && c != '\t' && c != '\b' && c != '\f' && c != '\v' &&
                        c != '\a') {
                        warnings.push_back("字符串包含控制字符: ASCII " + std::to_string(c));
                    }
                    pos++;
                } else {
                    // 处理UTF-8多字节字符
                    try {
                        auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(content, pos);
                        pos += (length > 0) ? length : 1;
                    } catch (const std::exception&) {
                        pos++;  // 出错时至少前进一个字节
                    }
                }
            }
        }

        bool StringValidator::processUTF8Char(const std::string& content, size_t& pos, std::string& error) const {
            if ((unsigned char)content[pos] >= 0x80) {
                try {
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(content, pos);
                    if (length == 0 || length > 4) {  // UTF-8最多4字节
                        error = "无效的UTF-8序列在位置 " + std::to_string(pos);
                        return false;
                    }
                    pos += length;
                } catch (const std::exception& e) {
                    error = "处理UTF-8字符时发生错误：" + std::string(e.what());
                    return false;
                }
            } else {
                pos++;
            }
            return true;
        }

        size_t StringValidator::getDefaultMaxLength() {
            return 1024 * 1024;  // 1MB
        }

        size_t StringValidator::getDefaultMaxLines() { return 10000; }

    }  // namespace frontend
}  // namespace rp
