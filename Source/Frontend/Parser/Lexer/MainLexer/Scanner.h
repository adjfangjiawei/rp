
#pragma once

#include <memory>

#include "IdentifierScanner.h"
#include "OperatorScanner.h"

namespace rp {
    namespace frontend {

        class Scanner {
          public:
            Scanner(DiagnosticEngine* diagEngine);

            void setSource(const char* src, size_t length, const std::string& filename);

            // 扫描标识符
            Token scanIdentifier();

            // 扫描操作符和标点符号
            Token scanOperatorOrPunctuation();

            // 获取当前位置信息
            size_t getCurrentPos() const { return identifierScanner.getCurrentPos(); }
            size_t getCurrentLine() const { return identifierScanner.getCurrentLine(); }
            size_t getCurrentColumn() const { return identifierScanner.getCurrentColumn(); }

            // 设置位置信息
            void setPosition(size_t pos, size_t line, size_t column) {
                identifierScanner.setPosition(pos, line, column);
                operatorScanner.setPosition(pos, line, column);
            }

            // 字符判断函数
            bool isIdentifierStart(char c) const { return identifierScanner.isIdentifierStart(c); }

          private:
            IdentifierScanner identifierScanner;
            OperatorScanner operatorScanner;

            // 同步扫描器位置
            void syncScannerPositions();
        };

    }  // namespace frontend
}  // namespace rp
