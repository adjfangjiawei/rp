#pragma once
#include "../AST/AST.h"
#include <memory>

namespace rp
{
    namespace frontend
    {

        class VarDeclParser
        {
        public:
            // 解析变量声明
            std::shared_ptr<Decl> parseVarDecl();

            // 解析变量定义
            std::shared_ptr<Decl> parseVarDef();

            // 解析前向声明
            std::shared_ptr<Decl> parseForwardDecl();

        private:
            // 解析类型说明符
            std::shared_ptr<Type> parseTypeSpecifier();

            // 解析初始化器
            std::shared_ptr<Expr> parseInitializer();
        };

    } // namespace frontend
} // namespace rp
