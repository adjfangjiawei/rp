
#pragma once

#include <string_view>
#include <unordered_map>

#include "BaseScanner.h"

namespace rp {
    namespace frontend {

        class OperatorScanner : public BaseScanner {
          public:
            explicit OperatorScanner(std::shared_ptr<DiagnosticEngine> diagEngine);

            // 运算符和标点符号扫描
            Token scanOperatorOrPunctuation();

          private:
            // 运算符查找表
            using OperatorMap = std::unordered_map<std::string_view, TokenKind>;
            static const OperatorMap singleCharOperators;
            static const OperatorMap doubleCharOperators;
            static const OperatorMap tripleCharOperators;

            // 运算符处理
            Token handleSingleCharOperator(char c);
            Token handleDoubleCharOperator(char first, char second);
            Token handleTripleCharOperator(char first, char second, char third);

            // 错误处理
            void reportInvalidOperator(char c, size_t column);
            void skipInvalidOperators();

            // 辅助函数
            bool isOperatorStart(char c) const;
            bool isValidOperatorContinuation(char c) const;
            std::string_view getOperatorText(size_t start, size_t length) const;

            // 运算符长度限制
            static constexpr size_t MAX_OPERATOR_LENGTH = 3;

            // 初始化查找表
            static OperatorMap initSingleCharOperators();
            static OperatorMap initDoubleCharOperators();
            static OperatorMap initTripleCharOperators();
        };

    }  // namespace frontend
}  // namespace rp
