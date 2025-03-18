#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 二元运算符类型
        enum class BinaryOperatorKind
        {
            Add,
            Sub,
            Mul,
            Div,
            Mod, // 算术运算符
            And,
            Or,
            Xor,
            Shl,
            Shr, // 位运算符
            LAnd,
            LOr, // 逻辑运算符
            Eq,
            NE,
            LT,
            GT,
            LE,
            GE, // 比较运算符
            Assign,
            AddAssign,
            SubAssign, // 赋值运算符
            MulAssign,
            DivAssign,
            ModAssign,
            AndAssign,
            OrAssign,
            XorAssign,
            ShlAssign,
            ShrAssign,
            Comma // 逗号运算符
        };

        // 一元运算符类型
        enum class UnaryOperatorKind
        {
            Plus,
            Minus,
            Not,
            LNot, // +, -, ~, !
            PreInc,
            PreDec, // ++x, --x
            PostInc,
            PostDec, // x++, x--
            AddrOf,
            Deref, // &, *
            Sizeof,
            Alignof // sizeof, alignof
        };

        // 二元表达式
        class BinaryExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::BinaryExpr; }

            BinaryOperatorKind opcode;
            std::shared_ptr<Expr> left;
            std::shared_ptr<Expr> right;
            bool isCompoundAssignment = false; // 是否为复合赋值
        };

        // 一元表达式
        class UnaryExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::UnaryExpr; }

            UnaryOperatorKind opcode;
            std::shared_ptr<Expr> operand;
            bool isPostfix = false; // 是否为后缀运算符
        };

        // 字面量表达式
        class LiteralExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::LiteralExpr; }

            enum class LiteralKind
            {
                Integer,
                Float,
                Boolean,
                Character,
                String,
                Null,
                This,
                True,
                False
            } kind;

            std::string value; // 字面量的字符串表示
        };

        // 函数调用表达式
        class CallExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CallExpr; }

            std::shared_ptr<Expr> callee;
            std::vector<std::shared_ptr<Expr>> args;
            bool isConstructorCall = false; // 是否为构造函数调用
            bool isOperatorCall = false;    // 是否为运算符函数调用
        };

        // 成员访问表达式
        class MemberExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::MemberExpr; }

            std::shared_ptr<Expr> base;
            std::string memberName;
            bool isArrow = false; // 是否为->访问
        };

        // Lambda表达式
        class LambdaExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::LambdaExpr; }

            std::shared_ptr<LambdaDecl> lambda;
            bool isMutable = false;
            std::vector<std::shared_ptr<VarDecl>> captures;
        };

        // 类型转换表达式
        class CastExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CastExpr; }

            enum class CastKind
            {
                Static,      // static_cast
                Dynamic,     // dynamic_cast
                Const,       // const_cast
                Reinterpret, // reinterpret_cast
                CCast,       // C风格转换
                Implicit     // 隐式转换
            } castKind;

            std::shared_ptr<Expr> operand;
            std::shared_ptr<Type> targetType;
        };

        // 条件表达式（三目运算符）
        class ConditionalExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ConditionalExpr; }

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Expr> trueExpr;
            std::shared_ptr<Expr> falseExpr;
        };

        // 协程表达式
        class CoroutineExpr : public Expr
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CoroutineExpr; }

            enum class CoroutineKind
            {
                CoAwait, // co_await
                CoYield, // co_yield
                CoReturn // co_return
            } kind;

            std::shared_ptr<Expr> operand;
            std::shared_ptr<CoroutineState> state;
        };

    } // namespace frontend
} // namespace rp
