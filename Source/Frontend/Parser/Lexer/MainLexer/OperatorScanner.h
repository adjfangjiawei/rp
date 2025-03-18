
#pragma once

#include "BaseScanner.h"

namespace rp {
    namespace frontend {

        class OperatorScanner : public BaseScanner {
          public:
            using BaseScanner::BaseScanner;

            // 运算符和标点符号扫描
            Token scanOperatorOrPunctuation();

          private:
            // 辅助函数
            Token handleSingleCharOperator(char c);
            Token handleDoubleCharOperator(char first, char second);
            Token handleTripleCharOperator(char first, char second, char third);
        };

    }  // namespace frontend
}  // namespace rp
