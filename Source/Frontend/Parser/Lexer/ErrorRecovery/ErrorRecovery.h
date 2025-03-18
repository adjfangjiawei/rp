
#pragma once
#include <string>
#include <vector>

#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class ErrorRecovery {
          public:
            // 错误恢复策略
            enum class Strategy {
                SkipToNextStatement,     // 跳到下一个语句
                SkipToNextToken,         // 跳到下一个有效token
                SynchronizeToDelimiter,  // 同步到下一个分隔符
                PanicMode                // 紧急模式，跳过多个token直到找到同步点
            };

            // 错误恢复状态
            struct RecoveryState {
                Strategy strategy;
                TokenKind syncToken;               // 同步用的token类型
                bool inRecovery;                   // 是否正在恢复
                int errorCount;                    // 错误计数
                std::vector<TokenKind> skipUntil;  // 跳过直到遇到这些token
            };

            // 初始化错误恢复
            static RecoveryState initializeRecovery();

            // 执行错误恢复
            static bool recover(const std::string &source, size_t &currentPos, RecoveryState &state);

            // 检查是否是同步点
            static bool isSynchronizationPoint(TokenKind kind);

            // 获取适当的恢复策略
            static Strategy getRecoveryStrategy(TokenKind currentToken, const std::vector<TokenKind> &context);

            // 重置错误计数
            static void resetErrorCount(RecoveryState &state);

          private:
            // 跳过直到找到同步点
            static bool skipToSynchronizationPoint(const std::string &source,
                                                   size_t &currentPos,
                                                   const std::vector<TokenKind> &syncTokens);

            // 检查token是否是语句边界
            static bool isStatementBoundary(TokenKind kind);

            // 检查token是否是分隔符
            static bool isDelimiter(TokenKind kind);
        };

    }  // namespace frontend
}  // namespace rp
