#pragma once

namespace rp {
    namespace frontend {

        // 类型系统Token类型枚举
        enum class TypeTokenKind {
            // 基本类型关键字
            Void,
            Bool,
            Char,
            Short,
            Int,
            Long,
            Float,
            Double,
            Signed,
            Unsigned,

            // 存储类说明符
            Auto,
            Register,
            Extern,
            Mutable,

            // 函数说明符
            Inline,
            Explicit,
            Friend,

            // 类型修饰符
            Volatile,
            Restrict,

            // 类型特性
            Typedef,
            Sizeof,
            Typeof,
            Alignof,
        };

    }  // namespace frontend
}  // namespace rp
