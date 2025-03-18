#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 函数参数声明
        class ParmVarDecl : public VarDecl
        {
        public:
            bool hasDefaultArg = false;
            std::shared_ptr<Expr> defaultArg;
            unsigned paramIndex = 0;
        };

        // 函数声明基类
        class FunctionDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::FuncDecl; }

            std::vector<std::shared_ptr<ParmVarDecl>> params; // 参数列表
            std::shared_ptr<Type> returnType;                 // 返回类型
            std::shared_ptr<Stmt> body;                       // 函数体

            bool isInline = false;    // 是否内联
            bool isVirtual = false;   // 是否虚函数
            bool isPure = false;      // 是否纯虚函数
            bool isConstexpr = false; // 是否常量表达式函数
            bool isNoexcept = false;  // 是否不抛出异常
            bool isVariadic = false;  // 是否变参函数
            bool isDefaulted = false; // 是否使用默认实现
            bool isDeleted = false;   // 是否已删除

            // 协程相关
            bool isCoroutine = false;
            std::shared_ptr<CoroutineState> coroutineState;

            // 多返回值支持
            std::vector<std::shared_ptr<Type>> multipleReturnTypes;

            // 函数重载
            std::vector<std::shared_ptr<FunctionDecl>> overloads;

            // 模板相关
            bool isTemplateSpecialization = false;
            std::vector<std::shared_ptr<TemplateParam>> templateParams;
            std::vector<std::shared_ptr<Type>> templateArgs;
        };

        // 成员函数声明
        class CXXMethodDecl : public FunctionDecl
        {
        public:
            std::shared_ptr<ClassDecl> parent; // 所属类
            bool isStatic = false;             // 是否静态成员函数
            bool isConst = false;              // 是否const成员函数
            bool isVolatile = false;           // 是否volatile成员函数
            bool isVirtual = false;            // 是否虚函数
            unsigned vtableIndex = 0;          // 虚函数表索引
        };

        // 构造函数声明
        class ConstructorDecl : public CXXMethodDecl
        {
        public:
            bool isCopy = false;                                // 是否拷贝构造函数
            bool isMove = false;                                // 是否移动构造函数
            std::vector<std::shared_ptr<Expr>> initializerList; // 初始化列表
        };

        // 析构函数声明
        class DestructorDecl : public CXXMethodDecl
        {
        public:
            bool isVirtual = false; // 是否虚析构函数
        };

        // 函数模板声明
        class FunctionTemplateDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::TemplateDecl; }

            std::shared_ptr<FunctionDecl> functionDecl;                 // 模板函数声明
            std::vector<std::shared_ptr<TemplateParam>> params;         // 模板参数
            std::vector<std::shared_ptr<FunctionDecl>> specializations; // 特化实例
        };

        // Lambda表达式声明
        class LambdaDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::FuncDecl; }

            std::shared_ptr<FunctionDecl> callOperator;     // operator()
            std::vector<std::shared_ptr<VarDecl>> captures; // 捕获变量
            bool isMutable = false;                         // 是否可修改捕获变量
        };

    } // namespace frontend
} // namespace rp
