#include "TypeTokenKind.h"

namespace rp {
    namespace frontend {

        std::string_view getTypeTokenKindName(TypeTokenKind kind) {
            switch (kind) {
                case TypeTokenKind::Void:
                    return "void";
                case TypeTokenKind::Bool:
                    return "bool";
                case TypeTokenKind::Char:
                    return "char";
                case TypeTokenKind::Short:
                    return "short";
                case TypeTokenKind::Int:
                    return "int";
                case TypeTokenKind::Long:
                    return "long";
                case TypeTokenKind::Float:
                    return "float";
                case TypeTokenKind::Double:
                    return "double";
                case TypeTokenKind::Signed:
                    return "signed";
                case TypeTokenKind::Unsigned:
                    return "unsigned";
                case TypeTokenKind::Char8_t:
                    return "char8_t";
                case TypeTokenKind::Char16_t:
                    return "char16_t";
                case TypeTokenKind::Char32_t:
                    return "char32_t";
                case TypeTokenKind::WChar_t:
                    return "wchar_t";
                case TypeTokenKind::Auto:
                    return "auto";
                case TypeTokenKind::Register:
                    return "register";
                case TypeTokenKind::Static:
                    return "static";
                case TypeTokenKind::Extern:
                    return "extern";
                case TypeTokenKind::Thread_local:
                    return "thread_local";
                case TypeTokenKind::Mutable:
                    return "mutable";
                case TypeTokenKind::Const:
                    return "const";
                case TypeTokenKind::Volatile:
                    return "volatile";
                case TypeTokenKind::Restrict:
                    return "restrict";
                case TypeTokenKind::Inline:
                    return "inline";
                case TypeTokenKind::Virtual:
                    return "virtual";
                case TypeTokenKind::Explicit:
                    return "explicit";
                case TypeTokenKind::Constexpr:
                    return "constexpr";
                case TypeTokenKind::Consteval:
                    return "consteval";
                case TypeTokenKind::Constinit:
                    return "constinit";
                case TypeTokenKind::Typedef:
                    return "typedef";
                case TypeTokenKind::Typename:
                    return "typename";
                case TypeTokenKind::Decltype:
                    return "decltype";
                case TypeTokenKind::Alignas:
                    return "alignas";
                case TypeTokenKind::Sizeof:
                    return "sizeof";
                case TypeTokenKind::Alignof:
                    return "alignof";
                case TypeTokenKind::Typeof:
                    return "typeof";
                case TypeTokenKind::Class:
                    return "class";
                case TypeTokenKind::Struct:
                    return "struct";
                case TypeTokenKind::Union:
                    return "union";
                case TypeTokenKind::Enum:
                    return "enum";
                case TypeTokenKind::Template:
                    return "template";
                case TypeTokenKind::Concept:
                    return "concept";
                case TypeTokenKind::Requires:
                    return "requires";
                default:
                    return "unknown";
            }
        }

        bool isBasicType(TypeTokenKind kind) {
            switch (kind) {
                case TypeTokenKind::Void:
                case TypeTokenKind::Bool:
                case TypeTokenKind::Char:
                case TypeTokenKind::Short:
                case TypeTokenKind::Int:
                case TypeTokenKind::Long:
                case TypeTokenKind::Float:
                case TypeTokenKind::Double:
                    return true;
                default:
                    return false;
            }
        }

        bool isTypeModifier(TypeTokenKind kind) {
            switch (kind) {
                case TypeTokenKind::Signed:
                case TypeTokenKind::Unsigned:
                case TypeTokenKind::Const:
                case TypeTokenKind::Volatile:
                case TypeTokenKind::Restrict:
                    return true;
                default:
                    return false;
            }
        }

        bool isStorageClassSpecifier(TypeTokenKind kind) {
            switch (kind) {
                case TypeTokenKind::Auto:
                case TypeTokenKind::Register:
                case TypeTokenKind::Static:
                case TypeTokenKind::Extern:
                case TypeTokenKind::Thread_local:
                case TypeTokenKind::Mutable:
                    return true;
                default:
                    return false;
            }
        }

    }  // namespace frontend
}  // namespace rp
