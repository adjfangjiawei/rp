#pragma once
#include "AST/Stmt.h"
#include "ExprParser.h"
#include "Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>
#include <stack>

namespace rp
{
    namespace frontend
    {

        class StmtParser
        {
        public:
            StmtParser(Lexer &lexer, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), exprParser(lexer, diag), diagnostics(diag) {}

            // 解析语句
            std::shared_ptr<Stmt> parseStatement();

            // 解析复合语句（代码块）
            std::shared_ptr<CompoundStmt> parseCompoundStmt();

        private:
            Lexer &lexer;
            ExprParser exprParser;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 控制流上下文跟踪
            struct ControlFlowContext
            {
                bool inLoop = false;
                bool inSwitch = false;
            };
            std::stack<ControlFlowContext> controlFlowStack;

            // 解析各种语句
            std::shared_ptr<Stmt> parseDeclarationStmt();
            std::shared_ptr<Stmt> parseExpressionStmt();
            std::shared_ptr<Stmt> parseIfStmt();
            std::shared_ptr<Stmt> parseWhileStmt();
            std::shared_ptr<Stmt> parseDoWhileStmt();
            std::shared_ptr<Stmt> parseForStmt();
            std::shared_ptr<Stmt> parseRangeForStmt();
            std::shared_ptr<Stmt> parseSwitchStmt();
            std::shared_ptr<Stmt> parseCaseStmt();
            std::shared_ptr<Stmt> parseDefaultStmt();
            std::shared_ptr<Stmt> parseBreakStmt();
            std::shared_ptr<Stmt> parseContinueStmt();
            std::shared_ptr<Stmt> parseReturnStmt();
            std::shared_ptr<Stmt> parseGotoStmt();
            std::shared_ptr<Stmt> parseLabelStmt();
            std::shared_ptr<Stmt> parseTryStmt();

            // 辅助函数
            void pushControlFlow(bool inLoop, bool inSwitch);
            void popControlFlow();
            bool isInLoop() const;
            bool isInSwitch() const;

            // 错误处理和恢复
            void skipUntilStatement();
            void reportError(const std::string &message);

            // 检查是否是语句的开始
            bool isStatementStart(TokenKind kind) const;
        };

    } // namespace frontend
} // namespace rp
