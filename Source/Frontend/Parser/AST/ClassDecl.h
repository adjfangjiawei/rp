#pragma once
#include "AST.h"
#include <unordered_map>

namespace rp
{
    namespace frontend
    {
        // 基类信息
        struct BaseClass
        {
            std::shared_ptr<Type> type;
            bool isVirtual = false;
            unsigned accessLevel = 0; // public/protected/private
        };

        // 类声明节点
        class ClassDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ClassDecl; }

            bool isStruct = false;   // 是否为struct（默认public）
            bool isUnion = false;    // 是否为union
            bool isFinal = false;    // 是否禁止继承
            bool isAbstract = false; // 是否为抽象类

            // 继承相关
            std::vector<BaseClass> bases; // 基类列表

            // 成员声明
            std::vector<std::shared_ptr<Decl>> members;
            std::vector<std::shared_ptr<FuncDecl>> methods;
            std::vector<std::shared_ptr<VarDecl>> fields;

            // 特殊成员函数
            std::shared_ptr<FuncDecl> defaultConstructor;
            std::shared_ptr<FuncDecl> copyConstructor;
            std::shared_ptr<FuncDecl> moveConstructor;
            std::shared_ptr<FuncDecl> destructor;
            std::shared_ptr<FuncDecl> copyAssignment;
            std::shared_ptr<FuncDecl> moveAssignment;

            // 虚函数表
            struct VTableEntry
            {
                std::shared_ptr<FuncDecl> method;
                unsigned offset;
            };
            std::vector<VTableEntry> vtable;

            // 布局信息
            unsigned size = 0;                                      // 类大小
            unsigned alignment = 0;                                 // 对齐要求
            std::unordered_map<std::string, unsigned> fieldOffsets; // 成员变量偏移量

            // 模板相关
            bool isTemplateSpecialization = false;
            std::vector<std::shared_ptr<TemplateParam>> templateParams;
            std::vector<std::shared_ptr<Type>> templateArgs;

            // 友元声明
            std::vector<std::shared_ptr<Decl>> friends;
        };

        // 类模板声明
        class ClassTemplateDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::TemplateDecl; }

            std::shared_ptr<ClassDecl> classDecl;                    // 模板类声明
            std::vector<std::shared_ptr<TemplateParam>> params;      // 模板参数
            std::vector<std::shared_ptr<ClassDecl>> specializations; // 特化实例
        };

        // 类型别名声明
        class TypeAliasDecl : public Decl
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::VarDecl; }

            std::shared_ptr<Type> underlyingType; // 底层类型
            bool isTemplateAlias = false;         // 是否为模板别名
        };

    } // namespace frontend
} // namespace rp
