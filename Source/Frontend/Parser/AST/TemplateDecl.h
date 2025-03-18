#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 模板参数类型
        enum class TemplateParamKind
        {
            Type,    // 类型参数
            NonType, // 非类型参数
            Template // 模板模板参数
        };

        // 模板参数基类
        class TemplateParam : public Decl
        {
        public:
            TemplateParamKind paramKind;
            bool isParameterPack = false; // 是否为参数包
            unsigned depth = 0;           // 模板嵌套深度
            unsigned index = 0;           // 参数索引
        };

        // 类型模板参数
        class TemplateTypeParam : public TemplateParam
        {
        public:
            std::shared_ptr<Type> defaultArg; // 默认类型参数
        };

        // 非类型模板参数
        class NonTypeTemplateParam : public TemplateParam
        {
        public:
            std::shared_ptr<Type> paramType;  // 参数类型
            std::shared_ptr<Expr> defaultArg; // 默认值
        };

        // 模板模板参数
        class TemplateTemplateParam : public TemplateParam
        {
        public:
            std::vector<std::shared_ptr<TemplateParam>> params; // 模板参数
            std::shared_ptr<TemplateDecl> defaultArg;           // 默认模板
        };

        // 模板参数包
        class TemplateParamPack : public TemplateParam
        {
        public:
            std::vector<std::shared_ptr<TemplateParam>> params; // 展开的参数
        };

        // 模板特化
        class TemplateSpecialization : public Decl
        {
        public:
            std::shared_ptr<Decl> specializedTemplate; // 被特化的模板
            std::vector<std::shared_ptr<Type>> args;   // 特化参数
        };

        // 变参模板支持
        class VariadicTemplateDecl : public Decl
        {
        public:
            std::shared_ptr<TemplateParamPack> paramPack; // 参数包
            std::shared_ptr<Decl> pattern;                // 展开模式
        };

        // 模板别名
        class TemplateAliasDecl : public Decl
        {
        public:
            std::vector<std::shared_ptr<TemplateParam>> params; // 模板参数
            std::shared_ptr<Type> underlyingType;               // 底层类型
        };

        // 常量模板
        class ValueTemplateDecl : public Decl
        {
        public:
            std::vector<std::shared_ptr<TemplateParam>> params; // 模板参数
            std::shared_ptr<Expr> value;                        // 常量值表达式
        };

    } // namespace frontend
} // namespace rp
