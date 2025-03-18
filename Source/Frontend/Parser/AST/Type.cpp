#include "Type.h"
#include <sstream>

namespace rp
{
    namespace frontend
    {
        // BuiltinType实现
        std::string BuiltinType::toString() const
        {
            switch (kind)
            {
            case TypeKind::Void:
                return "void";
            case TypeKind::Bool:
                return "bool";
            case TypeKind::Char:
                return "char";
            case TypeKind::WChar:
                return "wchar";
            case TypeKind::Char8:
                return "char8";
            case TypeKind::Char16:
                return "char16";
            case TypeKind::Char32:
                return "char32";
            case TypeKind::SChar:
                return "signed char";
            case TypeKind::UChar:
                return "unsigned char";
            case TypeKind::Short:
                return "short";
            case TypeKind::UShort:
                return "unsigned short";
            case TypeKind::Int:
                return "int";
            case TypeKind::UInt:
                return "unsigned int";
            case TypeKind::Long:
                return "long";
            case TypeKind::ULong:
                return "unsigned long";
            case TypeKind::LongLong:
                return "long long";
            case TypeKind::ULongLong:
                return "unsigned long long";
            case TypeKind::Float:
                return "float";
            case TypeKind::Double:
                return "double";
            case TypeKind::LongDouble:
                return "long double";
            case TypeKind::Nullptr:
                return "nullptr_t";
            default:
                return "unknown";
            }
        }

        size_t BuiltinType::getSize() const
        {
            switch (kind)
            {
            case TypeKind::Void:
                return 0;
            case TypeKind::Bool:
                return 1;
            case TypeKind::Char:
            case TypeKind::SChar:
            case TypeKind::UChar:
            case TypeKind::Char8:
                return 1;
            case TypeKind::Char16:
                return 2;
            case TypeKind::WChar:
            case TypeKind::Char32:
                return 4;
            case TypeKind::Short:
            case TypeKind::UShort:
                return 2;
            case TypeKind::Int:
            case TypeKind::UInt:
                return 4;
            case TypeKind::Long:
            case TypeKind::ULong:
                return 8;
            case TypeKind::LongLong:
            case TypeKind::ULongLong:
                return 8;
            case TypeKind::Float:
                return 4;
            case TypeKind::Double:
                return 8;
            case TypeKind::LongDouble:
                return 16;
            case TypeKind::Nullptr:
                return 8;
            default:
                return 0;
            }
        }

        // UserDefinedType实现
        std::string UserDefinedType::toString() const
        {
            return decl ? decl->name : "undefined";
        }

        bool UserDefinedType::isComplete() const
        {
            return decl && decl->isDefinition;
        }

        size_t UserDefinedType::getSize() const
        {
            // 对于类类型，返回类的大小
            if (auto classDecl = std::dynamic_pointer_cast<ClassDecl>(decl))
            {
                return classDecl->size;
            }
            return 0;
        }

        // ArrayType实现
        std::string ArrayType::toString() const
        {
            std::stringstream ss;
            ss << elementType->toString() << "[";
            if (sizeExpr)
            {
                // TODO: 实现表达式求值以获取数组大小的字符串表示
                ss << "N";
            }
            ss << "]";
            return ss.str();
        }

        bool ArrayType::isComplete() const
        {
            return elementType->isComplete() && (sizeExpr || isVLA);
        }

        size_t ArrayType::getSize() const
        {
            if (!isComplete())
                return 0;
            // TODO: 实现表达式求值以获取数组大小
            return elementType->getSize() * 10; // 临时实现
        }

        // PointerType实现
        std::string PointerType::toString() const
        {
            return pointeeType->toString() + "*";
        }

        size_t PointerType::getSize() const
        {
            return 8; // 64位系统上的指针大小
        }

        // ReferenceType实现
        std::string ReferenceType::toString() const
        {
            return referentType->toString() + (isRValueRef ? "&&" : "&");
        }

        size_t ReferenceType::getSize() const
        {
            return 8; // 引用在实现上通常是指针
        }

        // FunctionType实现
        std::string FunctionType::toString() const
        {
            std::stringstream ss;
            ss << returnType->toString() << "(";
            for (size_t i = 0; i < paramTypes.size(); ++i)
            {
                if (i > 0)
                    ss << ", ";
                ss << paramTypes[i]->toString();
            }
            if (isVariadic)
            {
                if (!paramTypes.empty())
                    ss << ", ";
                ss << "...";
            }
            ss << ")";
            if (isNoexcept)
                ss << " noexcept";
            return ss.str();
        }

        size_t FunctionType::getSize() const
        {
            return 8; // 函数指针的大小
        }

        // TupleType实现
        std::string TupleType::toString() const
        {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < elementTypes.size(); ++i)
            {
                if (i > 0)
                    ss << ", ";
                ss << elementTypes[i]->toString();
            }
            ss << "]";
            return ss.str();
        }

        bool TupleType::isComplete() const
        {
            for (const auto &type : elementTypes)
            {
                if (!type->isComplete())
                    return false;
            }
            return true;
        }

        size_t TupleType::getSize() const
        {
            size_t size = 0;
            for (const auto &type : elementTypes)
            {
                size += type->getSize();
            }
            return size;
        }

        // TemplateType实现
        std::string TemplateType::toString() const
        {
            std::stringstream ss;
            ss << templateDecl->name << "<";
            for (size_t i = 0; i < templateArgs.size(); ++i)
            {
                if (i > 0)
                    ss << ", ";
                ss << templateArgs[i]->toString();
            }
            ss << ">";
            return ss.str();
        }

        bool TemplateType::isComplete() const
        {
            // 模板类型完整性取决于模板参数和模板声明
            if (!templateDecl)
                return false;
            for (const auto &arg : templateArgs)
            {
                if (!arg->isComplete())
                    return false;
            }
            return true;
        }

        size_t TemplateType::getSize() const
        {
            // TODO: 实现模板实例化后的类型大小计算
            return 0;
        }

        // VariadicType实现
        std::string VariadicType::toString() const
        {
            std::stringstream ss;
            for (size_t i = 0; i < expandedTypes.size(); ++i)
            {
                if (i > 0)
                    ss << ", ";
                ss << expandedTypes[i]->toString();
            }
            return ss.str();
        }

        size_t VariadicType::getSize() const
        {
            size_t size = 0;
            for (const auto &type : expandedTypes)
            {
                size += type->getSize();
            }
            return size;
        }

        // QualifiedType实现
        std::string QualifiedType::toString() const
        {
            std::string result = underlyingType->toString();
            if (isConst)
                result = "const " + result;
            if (isVolatile)
                result = "volatile " + result;
            return result;
        }

        bool QualifiedType::isComplete() const
        {
            return underlyingType->isComplete();
        }

        size_t QualifiedType::getSize() const
        {
            return underlyingType->getSize();
        }

        // MemberPointerType实现
        std::string MemberPointerType::toString() const
        {
            return classType->toString() + "::*" + pointeeType->toString();
        }

        size_t MemberPointerType::getSize() const
        {
            return 16; // 成员指针通常需要两个指针大小
        }

    } // namespace frontend
} // namespace rp
