#pragma once
#include "AST/Expr.h"
#include "ExprParser.h"
#include "Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>
#include <vector>

namespace rp
{
    namespace frontend
    {

        class InitializerParser
        {
        public:
            InitializerParser(Lexer &lexer, ExprParser &exprParser, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), exprParser(exprParser), diagnostics(diag) {}

            // 解析初始化器
            std::shared_ptr<Expr> parseInitializer();

        private:
            Lexer &lexer;
            ExprParser &exprParser;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 解析各种初始化形式
            std::shared_ptr<InitListExpr> parseInitializerList();
            std::shared_ptr<Expr> parseConstructorCall();
            std::shared_ptr<Expr> parseAssignmentInit();

            // 错误处理
            void reportError(const std::string &message);
        };

    } // namespace frontend
} // namespace rp
