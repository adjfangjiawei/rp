#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 变量声明节点
        class VarDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::VarDecl; }

            std::shared_ptr<Type> type;        // 变量类型
            std::shared_ptr<Expr> initializer; // 初始化表达式
            bool isStatic = false;             // 是否为静态变量
            bool isThreadLocal = false;        // 是否为线程局部存储
            bool isExtern = false;             // 是否为外部变量
            bool isMutable = false;            // 是否可变（用于const类型）

            // 存储类型
            enum class StorageClass
            {
                None,
                Auto,
                Register,
                Static,
                Extern,
                ThreadLocal
            } storage = StorageClass::None;

            // 对齐要求
            std::shared_ptr<Expr> alignment;

            // 用于类成员变量
            bool isMember = false;
            unsigned fieldOffset = 0;   // 在类中的偏移量
            bool isBitField = false;    // 是否为位域
            unsigned bitFieldWidth = 0; // 位域宽度
        };

        // 数组变量声明
        class ArrayVarDecl : public VarDecl
        {
        public:
            std::vector<std::shared_ptr<Expr>> dimensions; // 数组维度
            bool hasVLADimension = false;                  // 是否包含可变长度数组维度
        };

        // 静态断言声明
        class StaticAssertDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::VarDecl; }

            std::shared_ptr<Expr> condition; // 断言条件
            std::string message;             // 断言消息
        };

    } // namespace frontend
} // namespace rp
