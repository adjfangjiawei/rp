#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 复合语句（代码块）
        class CompoundStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CompoundStmt; }

            std::vector<std::shared_ptr<Stmt>> statements;
            bool createsScope = true; // 是否创建新的作用域
        };

        // if语句
        class IfStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::IfStmt; }

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Stmt> thenBranch;
            std::shared_ptr<Stmt> elseBranch;     // 可能为nullptr
            std::shared_ptr<VarDecl> condVarDecl; // 条件变量声明（if with init）
        };

        // while语句
        class WhileStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::WhileStmt; }

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Stmt> body;
            bool isDoWhile = false;
        };

        // for语句
        class ForStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ForStmt; }

            std::shared_ptr<Stmt> init;      // 初始化语句
            std::shared_ptr<Expr> condition; // 条件表达式
            std::shared_ptr<Expr> increment; // 增量表达式
            std::shared_ptr<Stmt> body;      // 循环体
        };

        // 范围for语句
        class RangeForStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ForStmt; }

            std::shared_ptr<VarDecl> loopVar;  // 循环变量
            std::shared_ptr<Expr> rangeExpr;   // 范围表达式
            std::shared_ptr<Stmt> body;        // 循环体
            std::shared_ptr<VarDecl> beginVar; // 范围for的begin迭代器
            std::shared_ptr<VarDecl> endVar;   // 范围for的end迭代器
        };

        // switch语句
        class SwitchStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::SwitchStmt; }

            std::shared_ptr<Expr> condition;
            std::shared_ptr<Stmt> body;
            std::vector<std::shared_ptr<CaseStmt>> cases;
            std::shared_ptr<CaseStmt> defaultCase;
            std::shared_ptr<VarDecl> condVarDecl; // switch条件变量声明
        };

        // case语句
        class CaseStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CaseStmt; }

            std::shared_ptr<Expr> caseValue; // nullptr表示default
            std::shared_ptr<Stmt> subStmt;
            std::shared_ptr<SwitchStmt> switchStmt; // 所属的switch语句
        };

        // return语句
        class ReturnStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ReturnStmt; }
            bool isTerminator() const override { return true; }

            std::vector<std::shared_ptr<Expr>> returnValues; // 支持多返回值
            bool isCoroutineReturn = false;                  // 是否为协程return
        };

        // break语句
        class BreakStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::BreakStmt; }
            bool isTerminator() const override { return true; }

            std::shared_ptr<Stmt> target; // break的目标语句
        };

        // continue语句
        class ContinueStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ContinueStmt; }
            bool isTerminator() const override { return true; }

            std::shared_ptr<Stmt> target; // continue的目标语句
        };

        // 协程await语句
        class CoAwaitStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CoAwaitStmt; }

            std::shared_ptr<Expr> awaitExpr;
            std::shared_ptr<CoroutineState> state;
            bool isInitialSuspend = false; // 是否为初始挂起点
            bool isFinalSuspend = false;   // 是否为最终挂起点
        };

        // 协程yield语句
        class CoYieldStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CoYieldStmt; }

            std::shared_ptr<Expr> yieldExpr;
            std::shared_ptr<CoroutineState> state;
        };

        // try语句
        class TryStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::TryStmt; }

            std::shared_ptr<CompoundStmt> tryBlock;
            std::vector<std::shared_ptr<CatchStmt>> handlers;
        };

        // catch语句
        class CatchStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::CatchStmt; }

            std::shared_ptr<VarDecl> exceptionDecl; // 异常变量声明
            std::shared_ptr<CompoundStmt> handler;  // 处理代码块
            bool isCatchAll = false;                // 是否为catch(...)
        };

        // throw语句
        class ThrowStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ThrowStmt; }
            bool isTerminator() const override { return true; }

            std::shared_ptr<Expr> throwExpr; // 抛出的表达式
            bool isRethrow = false;          // 是否为重新抛出
        };

        // 声明语句（用于在语句位置的声明）
        class DeclStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::DeclStmt; }

            std::vector<std::shared_ptr<Decl>> decls;
        };

        // 表达式语句
        class ExprStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ExprStmt; }

            std::shared_ptr<Expr> expr;
        };

        // 空语句
        class NullStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::NullStmt; }
        };

        // 标签语句
        class LabelStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::LabelStmt; }

            std::string labelName;
            std::shared_ptr<Stmt> subStmt;
        };

        // goto语句
        class GotoStmt : public Stmt
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::GotoStmt; }
            bool isTerminator() const override { return true; }

            std::string labelName;
            std::shared_ptr<LabelStmt> target;
        };

    } // namespace frontend
} // namespace rp
