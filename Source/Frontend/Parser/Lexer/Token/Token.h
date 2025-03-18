#pragma once

// 包含所有拆分后的头文件
#include "TokenBase.h"
#include "TokenCategory.h"
#include "TokenKind.h"
#include "TokenUtils.h"

// 为了保持向后兼容性，将所有符号导出到原来的命名空间
namespace rp {
    namespace frontend {

        // Token类型判断函数的实现
        inline bool Token::isKeyword() const { return TokenUtils::isKeyword(kind); }

        inline bool Token::isOperator() const { return TokenUtils::isOperator(kind); }

        inline bool Token::isDelimiter() const { return TokenUtils::isDelimiter(kind); }

    }  // namespace frontend
}  // namespace rp
