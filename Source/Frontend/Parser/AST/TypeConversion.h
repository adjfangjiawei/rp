#pragma once
#include "Type.h"
#include <memory>

namespace rp
{
    namespace frontend
    {
        // 转换种类
        enum class ConversionKind
        {
            // 标准转换
            NoConversion,        // 无需转换
            LValueToRValue,      // 左值到右值
            ArrayToPointer,      // 数组到指针
            FunctionToPointer,   // 函数到指针
            NullToPointer,       // nullptr到指针
            NullToMemberPointer, // nullptr到成员指针

            // 数值转换
            IntegralPromotion,  // 整数提升
            FloatingPromotion,  // 浮点提升
            IntegralConversion, // 整数转换
            FloatingConversion, // 浮点转换
            FloatingIntegral,   // 浮点整数转换
            PointerConversion,  // 指针转换
            PointerToBoolean,   // 指针到布尔

            // 限定符转换
            QualificationConversion, // const/volatile转换

            // 用户定义转换
            ConstructorConversion, // 构造函数转换
            ConversionFunction,    // 转换函数

            // 其他转换
            DerivedToBase,     // 派生类到基类
            BaseToVirtualBase, // 基类到虚基类
            ReferenceBinding,  // 引用绑定
            RValueToLValue,    // 右值到左值（特殊情况）
        };

        // 转换等级
        enum class ConversionRank
        {
            Exact,             // 精确匹配
            Promotion,         // 提升
            Conversion,        // 标准转换
            QualificationConv, // 限定符转换
            UserDefined,       // 用户定义转换
            Ellipsis,          // 变参转换
            Invalid            // 无效转换
        };

        // 转换结果
        struct ConversionResult
        {
            bool isValid = false;                               // 转换是否有效
            ConversionKind kind = ConversionKind::NoConversion; // 转换类型
            ConversionRank rank = ConversionRank::Invalid;      // 转换等级
            std::shared_ptr<Type> resultType;                   // 转换后的类型
            std::shared_ptr<FunctionDecl> converter;            // 用于用户定义转换的函数
            bool requiresTemporary = false;                     // 是否需要临时对象
        };

        // 类型转换检查器
        class TypeConverter
        {
        public:
            // 检查从源类型到目标类型的转换是否可能
            static ConversionResult checkConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to,
                bool allowUserDefined = true);

            // 检查是否可以进行隐式转换
            static ConversionResult checkImplicitConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查是否可以进行显式转换
            static ConversionResult checkExplicitConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 获取类型提升结果
            static std::shared_ptr<Type> getPromotedType(
                const std::shared_ptr<Type> &type);

            // 获取通用类型（用于二元运算符）
            static std::shared_ptr<Type> getCommonType(
                const std::shared_ptr<Type> &left,
                const std::shared_ptr<Type> &right);

        private:
            // 检查标准转换
            static ConversionResult checkStandardConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查用户定义转换
            static ConversionResult checkUserDefinedConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查限定符转换
            static ConversionResult checkQualificationConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查数值转换
            static ConversionResult checkNumericConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查指针转换
            static ConversionResult checkPointerConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查引用转换
            static ConversionResult checkReferenceConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);

            // 检查数组和函数转换
            static ConversionResult checkArrayAndFunctionConversion(
                const std::shared_ptr<Type> &from,
                const std::shared_ptr<Type> &to);
        };

    } // namespace frontend
} // namespace rp
