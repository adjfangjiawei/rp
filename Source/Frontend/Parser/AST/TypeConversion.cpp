
// 在现有的TypeConversion.cpp文件中添加以下实现

namespace rp
{
    namespace frontend
    {
        namespace
        {
            // 检查类型是否为左值引用
            bool isLValueReference(const std::shared_ptr<Type> &type)
            {
                if (auto refType = std::dynamic_pointer_cast<ReferenceType>(type))
                {
                    return !refType->isRValueRef;
                }
                return false;
            }

            // 检查类型是否为右值引用
            bool isRValueReference(const std::shared_ptr<Type> &type)
            {
                if (auto refType = std::dynamic_pointer_cast<ReferenceType>(type))
                {
                    return refType->isRValueRef;
                }
                return false;
            }

            // 检查表达式是否为左值
            bool isLValue(const std::shared_ptr<Expr> &expr)
            {
                return expr && expr->isLValue;
            }

            // 检查表达式是否为亡值（xvalue）
            bool isXValue(const std::shared_ptr<Expr> &expr)
            {
                // TODO: 实现亡值检查
                return false;
            }

            // 应用引用坍缩规则
            bool applyReferenceFolding(bool isOuterRValue, bool isInnerRValue)
            {
                // 如果内层是左值引用，结果总是左值引用
                if (!isInnerRValue)
                    return false;

                // 如果内外层都是右值引用，结果是右值引用
                // 否则结果是左值引用
                return isOuterRValue && isInnerRValue;
            }
        }

        ConversionResult TypeConverter::checkReferenceConversion(
            const std::shared_ptr<Type> &from,
            const std::shared_ptr<Type> &to)
        {
            auto toRef = std::dynamic_pointer_cast<ReferenceType>(to);
            if (!toRef)
                return ConversionResult{};

            // 获取引用的目标类型
            auto toTarget = toRef->referentType;

            // 如果源类型已经是引用，获取其目标类型
            std::shared_ptr<Type> fromTarget = from;
            if (auto fromRef = std::dynamic_pointer_cast<ReferenceType>(from))
            {
                fromTarget = fromRef->referentType;
            }

            // 1. 引用绑定规则检查
            if (isLValueReference(to))
            {
                // 左值引用绑定规则
                if (!toRef->isConst)
                {
                    // 非const左值引用只能绑定到左值
                    if (!isLValue(nullptr)) // TODO: 需要传入表达式
                        return ConversionResult{};
                }
                else
                {
                    // const左值引用可以绑定到左值或右值
                    // 不需要额外检查
                }
            }
            else
            {
                // 右值引用绑定规则
                if (!isXValue(nullptr) && isLValue(nullptr)) // TODO: 需要传入表达式
                    return ConversionResult{};
            }

            // 2. 引用类型转换检查
            ConversionResult result = checkConversion(fromTarget, toTarget, false);
            if (!result.isValid)
                return result;

            // 3. 引用坍缩规则（用于模板）
            bool isRValue = false;
            if (auto fromRef = std::dynamic_pointer_cast<ReferenceType>(from))
            {
                isRValue = applyReferenceFolding(toRef->isRValueRef, fromRef->isRValueRef);
            }
            else
            {
                isRValue = toRef->isRValueRef;
            }

            // 4. 生命周期延长检查
            bool extendsLifetime = toRef->isConst && isRValueReference(from);

            // 创建转换结果
            return ConversionResult{
                true,
                ConversionKind::ReferenceBinding,
                result.rank,
                to,
                nullptr,
                extendsLifetime};
        }

        // 添加对引用限定符转换的支持
        ConversionResult TypeConverter::checkQualificationConversion(
            const std::shared_ptr<Type> &from,
            const std::shared_ptr<Type> &to)
        {
            // 处理引用类型的限定符
            if (auto fromRef = std::dynamic_pointer_cast<ReferenceType>(from))
            {
                if (auto toRef = std::dynamic_pointer_cast<ReferenceType>(to))
                {
                    // 检查引用目标类型的限定符转换
                    auto result = checkQualificationConversion(
                        fromRef->referentType,
                        toRef->referentType);

                    if (result.isValid)
                    {
                        result.kind = ConversionKind::QualificationConversion;
                        result.resultType = to;
                        return result;
                    }
                }
            }

            // 检查基本类型的限定符
            bool fromConst = from->isConst;
            bool fromVolatile = from->isVolatile;
            bool toConst = to->isConst;
            bool toVolatile = to->isVolatile;

            // 只允许添加限定符，不允许移除
            if ((!toConst && fromConst) || (!toVolatile && fromVolatile))
                return ConversionResult{};

            if (toConst == fromConst && toVolatile == fromVolatile)
                return ConversionResult{
                    true,
                    ConversionKind::NoConversion,
                    ConversionRank::Exact,
                    to,
                    nullptr,
                    false};

            return ConversionResult{
                true,
                ConversionKind::QualificationConversion,
                ConversionRank::QualificationConv,
                to,
                nullptr,
                false};
        }

    } // namespace frontend
} // namespace rp
