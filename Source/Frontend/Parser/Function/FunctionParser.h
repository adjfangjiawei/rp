#pragma once
#include "../AST/AST.h"
#include <memory>
#include <vector>

namespace rp
{
    namespace frontend
    {

        class FunctionParser
        {
        public:
            // 解析函数声明
            std::shared_ptr<Decl> parseFunctionDecl();

            // 解析函数定义
            std::shared_ptr<Decl> parseFunctionDef();

            // 解析Lambda表达式
            std::shared_ptr<Expr> parseLambdaExpr();

            // 解析协程函数
            std::shared_ptr<Decl> parseCoroutine();

        private:
            // 解析参数列表
            std::vector<std::shared_ptr<Decl>> parseParameters();

            // 解析函数体
            std::shared_ptr<Stmt> parseFunctionBody();

            // 解析多返回值
            std::vector<std::shared_ptr<Type>> parseReturnTypes();

            // 解析协程状态机
            void parseCoroutineStateMachine();
        };

    } // namespace frontend
} // namespace rp
