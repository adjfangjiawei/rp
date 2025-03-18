#include "TypeParser.h"

namespace rp
{
    namespace frontend
    {

        std::shared_ptr<Type> TypeParser::parseType()
        {
            // 解析类型限定符
            bool isConst = false;
            parseTypeQualifiers(isConst);

            // 解析基本类型
            std::shared_ptr<Type> type = parseBasicType();
            if (!type)
                return nullptr;

            // 设置const限定符
            type->isConst = isConst;

            // 解析后缀类型修饰符（指针、引用、数组等）
            while (true)
            {
                switch (lexer.currentToken().kind)
                {
                case TokenKind::Star:
                {
                    // 指针类型
                    lexer.nextToken(); // 消费'*'
                    auto ptrType = std::make_shared<PointerType>();
                    ptrType->pointeeType = type;

                    // 检查指针的const限定符
                    bool ptrIsConst = false;
                    parseTypeQualifiers(ptrIsConst);
                    ptrType->isConst = ptrIsConst;

                    type = ptrType;
                    break;
                }

                case TokenKind::Amp:
                {
                    // 左值引用
                    lexer.nextToken(); // 消费'&'
                    auto refType = std::make_shared<ReferenceType>();
                    refType->refKind = ReferenceType::RefKind::LValue;
                    refType->referentType = type;
                    type = refType;
                    break;
                }

                case TokenKind::AmpAmp:
                {
                    // 右值引用
                    lexer.nextToken(); // 消费'&&'
                    auto refType = std::make_shared<ReferenceType>();
                    refType->refKind = ReferenceType::RefKind::RValue;
                    refType->referentType = type;
                    type = refType;
                    break;
                }

                case TokenKind::LSquare:
                {
                    // 数组类型
                    lexer.nextToken(); // 消费'['

                    auto arrayType = std::make_shared<ArrayType>();
                    arrayType->elementType = type;

                    // 解析可选的数组大小
                    if (lexer.currentToken().kind != TokenKind::RSquare)
                    {
                        // TODO: 解析常量表达式作为数组大小
                    }

                    if (lexer.currentToken().kind != TokenKind::RSquare)
                    {
                        reportError("Expected ']' in array type");
                        return nullptr;
                    }
                    lexer.nextToken(); // 消费']'

                    type = arrayType;
                    break;
                }

                case TokenKind::Less:
                {
                    // 模板类型
                    auto templateType = parseTemplateType();
                    if (!templateType)
                        return nullptr;
                    templateType->templateType = type;
                    type = templateType;
                    break;
                }

                default:
                    return type;
                }
            }
        }

        std::shared_ptr<Type> TypeParser::parseBasicType()
        {
            auto basicType = std::make_shared<BasicType>();

            switch (lexer.currentToken().kind)
            {
            case TokenKind::Keyword_void:
                basicType->typeKind = BasicTypeKind::Void;
                break;

            case TokenKind::Keyword_bool:
                basicType->typeKind = BasicTypeKind::Bool;
                break;

            case TokenKind::Keyword_char:
                basicType->typeKind = BasicTypeKind::Char;
                break;

            case TokenKind::Keyword_short:
                basicType->typeKind = BasicTypeKind::Short;
                break;

            case TokenKind::Keyword_int:
                basicType->typeKind = BasicTypeKind::Int;
                break;

            case TokenKind::Keyword_long:
                basicType->typeKind = BasicTypeKind::Long;
                if (lexer.peekToken().kind == TokenKind::Keyword_long)
                {
                    lexer.nextToken(); // 消费第二个'long'
                    basicType->typeKind = BasicTypeKind::LongLong;
                }
                break;

            case TokenKind::Keyword_float:
                basicType->typeKind = BasicTypeKind::Float;
                break;

            case TokenKind::Keyword_double:
                basicType->typeKind = BasicTypeKind::Double;
                break;

            case TokenKind::Identifier:
                // 用户定义类型
                return parseUserDefinedType();

            default:
                reportError("Expected type specifier");
                return nullptr;
            }

            lexer.nextToken(); // 消费类型关键字
            return basicType;
        }

        std::shared_ptr<Type> TypeParser::parseUserDefinedType()
        {
            // 解析可能的作用域限定符
            std::vector<std::string> scopePath;

            do
            {
                if (lexer.currentToken().kind != TokenKind::Identifier)
                {
                    reportError("Expected identifier in qualified name");
                    return nullptr;
                }

                scopePath.push_back(lexer.currentToken().text);
                lexer.nextToken(); // 消费标识符

                if (lexer.currentToken().kind != TokenKind::ColonColon)
                    break;

                lexer.nextToken(); // 消费'::'
            } while (true);

            auto userType = std::make_shared<UserDefinedType>();
            userType->name = scopePath.back();
            scopePath.pop_back();
            userType->scopePath = std::move(scopePath);

            return userType;
        }

        std::shared_ptr<Type> TypeParser::parseTemplateType()
        {
            lexer.nextToken(); // 消费'<'

            auto templateType = std::make_shared<TemplateSpecializationType>();

            // 解析模板参数
            while (true)
            {
                auto argType = parseType();
                if (!argType)
                    return nullptr;

                templateType->args.push_back(argType);

                if (lexer.currentToken().kind != TokenKind::Comma)
                    break;

                lexer.nextToken(); // 消费','
            }

            if (lexer.currentToken().kind != TokenKind::Greater)
            {
                reportError("Expected '>' in template argument list");
                return nullptr;
            }
            lexer.nextToken(); // 消费'>'

            return templateType;
        }

        bool TypeParser::parseTypeQualifiers(bool &isConst)
        {
            while (true)
            {
                switch (lexer.currentToken().kind)
                {
                case TokenKind::Keyword_const:
                    isConst = true;
                    lexer.nextToken();
                    break;

                default:
                    return true;
                }
            }
        }

        void TypeParser::reportError(const std::string &message)
        {
            if (diagnostics)
            {
                diagnostics->report(DiagnosticLevel::Error,
                                    SourceLocation{lexer.getCurrentLocation()},
                                    message);
            }
        }

    } // namespace frontend
} // namespace rp
