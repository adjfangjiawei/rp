#pragma once
#include <string_view>

namespace rp {
    namespace frontend {

        /**
         * 类型系统Token的详细分类
         * 用于在类型检查和代码生成阶段处理类型相关的Token
         */
        enum class TypeTokenKind {
            // 基本类型
            Void,      // void类型
            Bool,      // bool类型
            Char,      // char类型
            Short,     // short类型
            Int,       // int类型
            Long,      // long类型
            Float,     // float类型
            Double,    // double类型
            Signed,    // signed修饰符
            Unsigned,  // unsigned修饰符

            // 扩展字符类型
            Char8_t,   // char8_t (C++20)
            Char16_t,  // char16_t (C++11)
            Char32_t,  // char32_t (C++11)
            WChar_t,   // wchar_t

            // 存储类说明符
            Auto,          // auto (类型推导)
            Register,      // register
            Static,        // static
            Extern,        // extern
            Thread_local,  // thread_local (C++11)
            Mutable,       // mutable

            // 类型限定符
            Const,     // const
            Volatile,  // volatile
            Restrict,  // restrict (C99)

            // 函数说明符
            Inline,     // inline
            Virtual,    // virtual
            Explicit,   // explicit
            Constexpr,  // constexpr (C++11)
            Consteval,  // consteval (C++20)
            Constinit,  // constinit (C++20)

            // 类型特性
            Typedef,   // typedef
            Typename,  // typename
            Decltype,  // decltype (C++11)
            Alignas,   // alignas (C++11)

            // 类型运算
            Sizeof,   // sizeof
            Alignof,  // alignof (C++11)
            Typeof,   // typeof (GNU扩展)

            // 复合类型说明符
            Class,   // class
            Struct,  // struct
            Union,   // union
            Enum,    // enum

            // 类型模板
            Template,  // template

            // 概念和约束 (C++20)
            Concept,  // concept
            Requires  // requires
        };

        /**
         * 获取TypeTokenKind的字符串表示
         */
        std::string_view getTypeTokenKindName(TypeTokenKind kind);

        /**
         * 检查TypeTokenKind是否是基本类型
         */
        bool isBasicType(TypeTokenKind kind);

        /**
         * 检查TypeTokenKind是否是类型修饰符
         */
        bool isTypeModifier(TypeTokenKind kind);

        /**
         * 检查TypeTokenKind是否是存储类说明符
         */
        bool isStorageClassSpecifier(TypeTokenKind kind);

    }  // namespace frontend
}  // namespace rp
