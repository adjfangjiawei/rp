
#include "ErrorRecovery.h"

#include <algorithm>

namespace rp {
    namespace frontend {

        ErrorRecovery::RecoveryState ErrorRecovery::initializeRecovery() {
            RecoveryState state;
            state.strategy = Strategy::SkipToNextToken;
            state.syncToken = TokenKind::Invalid;
            state.inRecovery = false;
            state.errorCount = 0;
            return state;
        }

        bool ErrorRecovery::recover(const std::string &source, size_t &currentPos, RecoveryState &state) {
            if (!state.inRecovery) {
                state.inRecovery = true;
                state.errorCount++;
            }

            // 根据不同的策略执行恢复
            switch (state.strategy) {
                case Strategy::SkipToNextStatement:
                    // 跳过直到找到语句边界
                    while (currentPos < source.length()) {
                        if (source[currentPos] == ';' || source[currentPos] == '}') {
                            currentPos++;
                            state.inRecovery = false;
                            return true;
                        }
                        currentPos++;
                    }
                    break;

                case Strategy::SkipToNextToken:
                    // 跳过空白字符和无效字符
                    while (currentPos < source.length()) {
                        if (!std::isspace(source[currentPos])) {
                            state.inRecovery = false;
                            return true;
                        }
                        currentPos++;
                    }
                    break;

                case Strategy::SynchronizeToDelimiter:
                    // 跳过直到找到分隔符
                    while (currentPos < source.length()) {
                        char c = source[currentPos];
                        if (c == ';' || c == '{' || c == '}' || c == ')' || c == ']') {
                            currentPos++;
                            state.inRecovery = false;
                            return true;
                        }
                        currentPos++;
                    }
                    break;

                case Strategy::PanicMode:
                    // 紧急模式：跳过多个token直到找到同步点
                    return skipToSynchronizationPoint(source, currentPos, state.skipUntil);
            }

            return false;
        }

        bool ErrorRecovery::isSynchronizationPoint(TokenKind kind) {
            switch (kind) {
                case TokenKind::Semicolon:
                case TokenKind::LBrace:
                case TokenKind::RBrace:
                case TokenKind::Keyword_Class:
                case TokenKind::Keyword_Struct:
                case TokenKind::Keyword_Enum:
                case TokenKind::Keyword_If:
                case TokenKind::Keyword_While:
                case TokenKind::Keyword_For:
                case TokenKind::Keyword_Return:
                    return true;
                default:
                    return false;
            }
        }

        ErrorRecovery::Strategy ErrorRecovery::getRecoveryStrategy(TokenKind currentToken,
                                                                   const std::vector<TokenKind> &context) {
            // 根据当前token和上下文选择合适的恢复策略
            if (context.empty()) {
                return Strategy::SkipToNextToken;
            }

            // 在块内部时使用SynchronizeToDelimiter
            if (std::find(context.begin(), context.end(), TokenKind::LBrace) != context.end()) {
                return Strategy::SynchronizeToDelimiter;
            }

            // 在语句中时跳到下一个语句
            if (isStatementBoundary(currentToken)) {
                return Strategy::SkipToNextStatement;
            }

            // 其他情况使用紧急模式
            return Strategy::PanicMode;
        }

        void ErrorRecovery::resetErrorCount(RecoveryState &state) {
            state.errorCount = 0;
            state.inRecovery = false;
        }

        bool ErrorRecovery::skipToSynchronizationPoint(const std::string &source,
                                                       size_t &currentPos,
                                                       const std::vector<TokenKind> &syncTokens) {
            while (currentPos < source.length()) {
                char c = source[currentPos];

                // 检查是否到达任何同步token
                if (std::find(syncTokens.begin(), syncTokens.end(), TokenKind::Semicolon) != syncTokens.end() &&
                    c == ';') {
                    currentPos++;
                    return true;
                }

                // 检查块级边界
                if (c == '{' || c == '}') {
                    currentPos++;
                    return true;
                }

                currentPos++;
            }
            return false;
        }

        bool ErrorRecovery::isStatementBoundary(TokenKind kind) {
            switch (kind) {
                case TokenKind::Semicolon:
                case TokenKind::RBrace:
                case TokenKind::Keyword_If:
                case TokenKind::Keyword_While:
                case TokenKind::Keyword_For:
                case TokenKind::Keyword_Return:
                case TokenKind::Keyword_Break:
                case TokenKind::Keyword_Continue:
                    return true;
                default:
                    return false;
            }
        }

        bool ErrorRecovery::isDelimiter(TokenKind kind) {
            switch (kind) {
                case TokenKind::Semicolon:
                case TokenKind::Comma:
                case TokenKind::LBrace:
                case TokenKind::RBrace:
                case TokenKind::LParen:
                case TokenKind::RParen:
                case TokenKind::LSquare:
                case TokenKind::RSquare:
                    return true;
                default:
                    return false;
            }
        }

    }  // namespace frontend
}  // namespace rp
