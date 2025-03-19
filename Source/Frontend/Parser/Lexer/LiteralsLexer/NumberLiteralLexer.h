#pragma once

#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Base/NumberLiteralBase.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class NumberLiteralLexer {
          public:
            explicit NumberLiteralLexer(std::shared_ptr<DiagnosticEngine> diagnostics) : diagnostics(diagnostics) {}

            // 设置源代码信息
            void setSource(const char *src, size_t length, const std::string &file) {
                source = src;
                sourceLength = length;
                filename = file;
            }

            // 扫描并返回下一个token
            Token scan();

            // 当前位置信息
            size_t currentPos = 0;
            size_t currentLine = 1;
            size_t currentColumn = 1;

          private:
            const char *source = nullptr;
            size_t sourceLength = 0;
            std::string filename;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 处理所有类型的数字字面量
            bool processNumberLiteral(const std::string &input, size_t &pos, NumberValue &value, std::string &error);

            // 验证数字字面量的基本格式
            bool validateNumberLiteral(const std::string &str, std::string &error);

            // 检查数字前缀并选择合适的处理器
            bool checkPrefixAndProcess(const std::string &input, size_t &pos, NumberValue &value, std::string &error);

            // 检查是否是浮点数
            bool isFloatingPointNumber(const std::string &input, size_t pos);

            // 处理类型后缀
            bool processTypeSuffix(const std::string &input, size_t &pos, NumberValue &value, std::string &error);

            // 验证值范围
            bool validateValueRange(const NumberValue &value, std::string &error);
        };

    }  // namespace frontend
}  // namespace rp
