#include "VarDeclParser.h"
#include "../AST/VarDecl.h"
#include "../AST/Type.h"
#include "../Lexer/Token.h"
#include "../Parser.h"
#include <memory>
#include <vector>

namespace rp
{
    namespace frontend
    {

        std::shared_ptr<Decl> VarDeclParser::parseVarDecl()
        {
            auto loc = getCurrentLocation();

            // 解析存储类型说明符（如static, extern等）
            auto storageClass = parseStorageClassIfPresent();

            // 解析类型说明符
            auto type = parseTypeSpecifier();
            if (!type)
            {
                return nullptr;
            }

            // 解析声明符（包括变量名和可能的数组维度等）
            std::string name = parseIdentifier();
            if (name.empty())
            {
                reportError("Expected variable name");
                return nullptr;
            }

            // 创建变量声明节点
            auto varDecl = std::make_shared<VarDecl>(name, type, loc);
            varDecl->setStorageClass(storageClass);

            // 检查是否有多个声明
            while (consumeToken(TokenKind::Comma))
            {
                name = parseIdentifier();
                if (name.empty())
                {
                    reportError("Expected variable name after comma");
                    break;
                }
                auto additionalDecl = std::make_shared<VarDecl>(name, type, getCurrentLocation());
                additionalDecl->setStorageClass(storageClass);
                // 将额外的声明添加到声明列表中
            }

            return varDecl;
        }

        std::shared_ptr<Decl> VarDeclParser::parseVarDef()
        {
            // 首先解析变量声明
            auto decl = parseVarDecl();
            if (!decl)
            {
                return nullptr;
            }

            auto varDecl = std::static_pointer_cast<VarDecl>(decl);

            // 检查是否有初始化器
            if (consumeToken(TokenKind::Equal))
            {
                auto init = parseInitializer();
                if (!init)
                {
                    reportError("Invalid initializer");
                    return nullptr;
                }
                varDecl->setInitializer(init);
            }

            return varDecl;
        }

        std::shared_ptr<Decl> VarDeclParser::parseForwardDecl()
        {
            auto loc = getCurrentLocation();

            // 检查forward关键字
            if (!consumeToken(TokenKind::Forward))
            {
                reportError("Expected 'forward' keyword");
                return nullptr;
            }

            // 解析类型说明符
            auto type = parseTypeSpecifier();
            if (!type)
            {
                return nullptr;
            }

            // 解析声明符
            std::string name = parseIdentifier();
            if (name.empty())
            {
                reportError("Expected variable name in forward declaration");
                return nullptr;
            }

            // 创建前向声明节点
            auto forwardDecl = std::make_shared<ForwardDecl>(name, type, loc);

            return forwardDecl;
        }

        std::shared_ptr<Type> VarDeclParser::parseTypeSpecifier()
        {
            auto loc = getCurrentLocation();

            // 解析基本类型
            TokenKind kind = getCurrentToken();
            switch (kind)
            {
            case TokenKind::Int:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Int, loc);
            case TokenKind::Float:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Float, loc);
            case TokenKind::Double:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Double, loc);
            case TokenKind::Char:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Char, loc);
            case TokenKind::Bool:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Bool, loc);
            case TokenKind::Void:
                consumeToken();
                return std::make_shared<BuiltinType>(BuiltinType::Void, loc);
            case TokenKind::Identifier:
            {
                // 处理用户定义类型
                std::string typeName = parseIdentifier();
                return std::make_shared<UserDefinedType>(typeName, loc);
            }
            default:
                reportError("Expected type specifier");
                return nullptr;
            }
        }

        std::shared_ptr<Expr> VarDeclParser::parseInitializer()
        {
            // 检查是否是列表初始化
            if (consumeToken(TokenKind::LeftBrace))
            {
                std::vector<std::shared_ptr<Expr>> initList;

                // 解析初始化列表
                if (!consumeToken(TokenKind::RightBrace))
                {
                    do
                    {
                        auto expr = parseExpression();
                        if (!expr)
                        {
                            return nullptr;
                        }
                        initList.push_back(expr);
                    } while (consumeToken(TokenKind::Comma));

                    if (!consumeToken(TokenKind::RightBrace))
                    {
                        reportError("Expected '}' after initializer list");
                        return nullptr;
                    }
                }

                return std::make_shared<InitListExpr>(initList, getCurrentLocation());
            }

            // 普通表达式初始化
            return parseExpression();
        }

    } // namespace frontend
} // namespace rp
