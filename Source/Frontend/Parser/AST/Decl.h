#pragma once
#include "AST.h"
#include <vector>
#include <memory>
#include <string>

namespace rp
{
    namespace frontend
    {

        // 访问控制
        enum class AccessSpecifier
        {
            Public,
            Protected,
            Private
        };

        // 存储类型说明符
        enum class StorageClass
        {
            None,
            Static,
            Extern,
            Thread_local
        };

        // 基本声明类
        class DeclBase : public Decl
        {
        public:
            StorageClass storage = StorageClass::None;
            bool isInline = false;
            bool isConstexpr = false;
            bool isVirtual = false;
            std::vector<std::string> attributes; // 属性列表
        };

        // 变量声明
        class VarDecl : public DeclBase
        {
        public:
            std::shared_ptr<Type> type;
            std::shared_ptr<Expr> initializer;
            bool isConstant = false;

            ASTNodeKind getKind() const override { return ASTNodeKind::VarDecl; }
        };

        // 函数参数声明
        class ParmVarDecl : public VarDecl
        {
        public:
            bool hasDefaultArg = false;
            std::shared_ptr<Expr> defaultArg;
        };

        // 函数声明
        class FunctionDecl : public DeclBase
        {
        public:
            std::shared_ptr<Type> returnType;
            std::vector<std::shared_ptr<ParmVarDecl>> params;
            std::shared_ptr<Stmt> body;
            bool isVariadic = false;
            bool isCoroutine = false;

            ASTNodeKind getKind() const override { return ASTNodeKind::FuncDecl; }
        };

        // 类声明
        class ClassDecl : public DeclBase
        {
        public:
            enum class ClassKind
            {
                Class,
                Struct,
                Union
            };

            ClassKind classKind;
            std::vector<std::shared_ptr<Type>> bases;   // 基类列表
            std::vector<std::shared_ptr<Decl>> members; // 成员声明
            bool isPacked = false;

            ASTNodeKind getKind() const override { return ASTNodeKind::ClassDecl; }
        };

        // 模板参数
        class TemplateParameter : public DeclBase
        {
        public:
            enum class ParamKind
            {
                Type,
                NonType,
                Template
            };

            ParamKind paramKind;
            std::shared_ptr<Type> paramType; // 用于非类型模板参数
            std::shared_ptr<Expr> defaultArg;
        };

        // 模板声明
        class TemplateDecl : public DeclBase
        {
        public:
            std::vector<std::shared_ptr<TemplateParameter>> params;
            std::shared_ptr<Decl> templatedDecl; // 被模板化的声明
            bool isVariadic = false;

            ASTNodeKind getKind() const override { return ASTNodeKind::TemplateDecl; }
        };

        // 枚举声明
        class EnumDecl : public DeclBase
        {
        public:
            std::shared_ptr<Type> underlyingType;
            std::vector<std::pair<std::string, std::shared_ptr<Expr>>> enumerators;
            bool isScoped = false; // 是否是作用域枚举(enum class)

            ASTNodeKind getKind() const override { return ASTNodeKind::EnumDecl; }
        };

        // 友元声明
        class FriendDecl : public DeclBase
        {
        public:
            std::shared_ptr<Decl> friendDecl;

            ASTNodeKind getKind() const override { return ASTNodeKind::FriendDecl; }
        };

        // 类型别名声明
        class TypeAliasDecl : public DeclBase
        {
        public:
            std::shared_ptr<Type> underlyingType;

            ASTNodeKind getKind() const override { return ASTNodeKind::TypeAliasDecl; }
        };

    } // namespace frontend
} // namespace rp
