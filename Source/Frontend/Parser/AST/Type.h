#pragma once
#include "AST.h"

namespace rp
{
    namespace frontend
    {
        // 内置类型
        class BuiltinType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::BuiltinType; }

            enum class TypeKind
            {
                Void,
                Bool,
                Char,
                WChar,
                Char8,
                Char16,
                Char32,
                SChar,
                UChar,
                Short,
                UShort,
                Int,
                UInt,
                Long,
                ULong,
                LongLong,
                ULongLong,
                Float,
                Double,
                LongDouble,
                Nullptr
            } kind;

            std::string toString() const override;
            bool isComplete() const override { return kind != TypeKind::Void; }
            size_t getSize() const override;
        };

        // 用户定义类型
        class UserDefinedType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::UserDefinedType; }

            std::shared_ptr<Decl> decl; // 类型声明
            std::string toString() const override;
            bool isComplete() const override;
            size_t getSize() const override;
        };

        // 数组类型
        class ArrayType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ArrayType; }

            std::shared_ptr<Type> elementType;
            std::shared_ptr<Expr> sizeExpr; // nullptr表示未知大小
            bool isVLA = false;             // 是否为可变长度数组

            std::string toString() const override;
            bool isComplete() const override;
            size_t getSize() const override;
        };

        // 指针类型
        class PointerType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::PointerType; }

            std::shared_ptr<Type> pointeeType;
            bool isSmartPointer = false; // 是否为智能指针

            std::string toString() const override;
            bool isComplete() const override { return true; }
            size_t getSize() const override;
        };

        // 引用类型
        class ReferenceType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::ReferenceType; }

            std::shared_ptr<Type> referentType;
            bool isRValueRef = false; // 是否为右值引用

            std::string toString() const override;
            bool isComplete() const override { return true; }
            size_t getSize() const override;
        };

        // 函数类型
        class FunctionType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::FunctionType; }

            std::shared_ptr<Type> returnType;
            std::vector<std::shared_ptr<Type>> paramTypes;
            bool isVariadic = false;
            bool isNoexcept = false;

            std::string toString() const override;
            bool isComplete() const override { return true; }
            size_t getSize() const override;
        };

        // 元组类型（用于多返回值）
        class TupleType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::TupleType; }

            std::vector<std::shared_ptr<Type>> elementTypes;

            std::string toString() const override;
            bool isComplete() const override;
            size_t getSize() const override;
        };

        // 模板类型
        class TemplateType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::TemplateType; }

            std::shared_ptr<TemplateDecl> templateDecl;
            std::vector<std::shared_ptr<Type>> templateArgs;

            std::string toString() const override;
            bool isComplete() const override;
            size_t getSize() const override;
        };

        // 变参模板类型
        class VariadicType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::VariadicType; }

            std::vector<std::shared_ptr<Type>> expandedTypes;

            std::string toString() const override;
            bool isComplete() const override { return true; }
            size_t getSize() const override;
        };

        // 限定类型（const/volatile）
        class QualifiedType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::QualifiedType; }

            std::shared_ptr<Type> underlyingType;
            bool isConst = false;
            bool isVolatile = false;

            std::string toString() const override;
            bool isComplete() const override;
            size_t getSize() const override;
        };

        // 成员指针类型
        class MemberPointerType : public Type
        {
        public:
            ASTNodeKind getKind() const override { return ASTNodeKind::PointerType; }

            std::shared_ptr<Type> classType;
            std::shared_ptr<Type> pointeeType;

            std::string toString() const override;
            bool isComplete() const override { return true; }
            size_t getSize() const override;
        };

    } // namespace frontend
} // namespace rp
