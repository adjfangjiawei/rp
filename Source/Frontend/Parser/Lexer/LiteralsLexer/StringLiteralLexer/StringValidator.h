#ifndef RP_STRING_VALIDATOR_H
#define RP_STRING_VALIDATOR_H

#include <string>
#include <vector>

namespace rp {
    namespace frontend {
        struct StringValidationOptions {
            bool allowControlChars = true;    // 是否允许控制字符
            bool strictUTF8 = true;           // 是否严格检查UTF-8
            bool allowInvalidUnicode = true;  // 是否允许无效的Unicode
            bool allowMultiline = true;       // 是否允许多行字符串
            size_t maxLength = 1024 * 1024;   // 最大字符串长度
            size_t maxLines = 114514;         // 最大行数
            bool allowChineseQuotes = true;   // 是否允许中文引号
            bool allowSmartQuotes = true;     // 是否允许智能引号
        };

        class StringValidator {
          public:
            // 构造函数，设置验证选项
            explicit StringValidator(const StringValidationOptions& options);

            // 验证字符串内容
            bool validate(const std::string& content, std::string& error, std::vector<std::string>& warnings);

          private:
            // 验证选项
            StringValidationOptions validationOptions;

            // 检查字符串长度限制
            bool checkLengthLimit(const std::string& content, std::string& error) const;

            // 检查行数限制
            bool checkLineLimit(const std::string& content, std::string& error) const;

            // 检查UTF-8有效性
            bool checkUTF8Validity(const std::string& content, std::string& error) const;

            // 检查控制字符
            void checkControlChars(const std::string& content, std::vector<std::string>& warnings) const;

            // 处理UTF-8字符
            bool processUTF8Char(const std::string& content, size_t& pos, std::string& error) const;

            // 获取默认的最大长度限制
            static size_t getDefaultMaxLength();

            // 获取默认的最大行数限制
            static size_t getDefaultMaxLines();
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_STRING_VALIDATOR_H
