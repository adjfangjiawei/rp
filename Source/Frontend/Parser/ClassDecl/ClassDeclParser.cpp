
#include "ClassDeclParser.h"
#include "../AST/ClassDecl.h"
#include "../AST/FunctionDecl.h"
#include "../AST/Type.h"
#include "../TypeParser.h"
#include <cassert>

namespace rp
{
    namespace frontend
    {

        std::shared_ptr<Decl> ClassDeclParser::parseClassDecl()
        {
            auto classDecl = std::make_shared<ClassDecl>();

            // 解析class/struct/union关键字
            Token tok = getCurrentToken();
            if (tok.kind == TokenKind::Keyword_class)
            {
                classDecl->isStruct = false;
            }
            else if (tok.kind == TokenKind::Keyword_struct)
            {
                classDecl->isStruct = true;
            }
            else if (tok.kind == TokenKind::Keyword_union)
            {
                classDecl->isUnion = true;
            }
            else
            {
                reportError("Expected 'class', 'struct' or 'union'");
                return nullptr;
            }
            consumeToken(); // 消费class/struct/union关键字

            // 解析类名
            tok = getCurrentToken();
            if (tok.kind != TokenKind::Identifier)
            {
                reportError("Expected class name");
                return nullptr;
            }
            classDecl->name = std::string(tok.text);
            consumeToken(); // 消费类名

            // 检查是否是模板声明
            if (getCurrentToken().kind == TokenKind::Less)
            {
                classDecl->isTemplateSpecialization = true;
                // 创建模板解析器并解析模板参数
                TemplateParser templateParser(lexer, diagnostics);
                classDecl->templateParams = templateParser.parseTemplateParameters();
                if (classDecl->templateParams.empty())
                {
                    reportError("Failed to parse template parameters");
                    return nullptr;
                }
            }

            // 检查是否有基类
            if (getCurrentToken().kind == TokenKind::Colon)
            {
                consumeToken(); // 消费冒号
                auto baseTypes = parseBaseClasses();
                for (const auto &type : baseTypes)
                {
                    BaseClass base;
                    base.type = type;
                    classDecl->bases.push_back(base);
                }
            }

            // 如果有类体定义
            if (getCurrentToken().kind == TokenKind::LBrace)
            {
                consumeToken(); // 消费左花括号

                // 解析类成员
                while (getCurrentToken().kind != TokenKind::RBrace)
                {
                    if (getCurrentToken().kind == TokenKind::EndOfFile)
                    {
                        reportError("Unexpected end of file in class definition");
                        return nullptr;
                    }

                    auto members = parseMembers();
                    classDecl->members.insert(
                        classDecl->members.end(),
                        members.begin(),
                        members.end());

                    // 对成员进行分类
                    for (const auto &member : members)
                    {
                        if (auto funcDecl = std::dynamic_pointer_cast<FuncDecl>(member))
                        {
                            classDecl->methods.push_back(funcDecl);
                        }
                        else if (auto varDecl = std::dynamic_pointer_cast<VarDecl>(member))
                        {
                            classDecl->fields.push_back(varDecl);
                        }
                    }
                }
                consumeToken(); // 消费右花括号
            }

            return classDecl;
        }

        std::shared_ptr<Decl> ClassDeclParser::parseClassDef()
        {
            // 类定义与类声明类似，但必须有类体
            auto classDecl = std::static_pointer_cast<ClassDecl>(parseClassDecl());
            if (!classDecl || classDecl->members.empty())
            {
                reportError("Class definition must have a body");
                return nullptr;
            }
            return classDecl;
        }

        std::vector<std::shared_ptr<Type>> ClassDeclParser::parseBaseClasses()
        {
            std::vector<std::shared_ptr<Type>> bases;
            TypeParser typeParser(lexer, diagnostics);

            do
            {
                BaseClass baseClass;

                // 解析访问说明符
                Token tok = getCurrentToken();
                if (tok.kind == TokenKind::Keyword_public)
                {
                    baseClass.accessLevel = 0;
                    consumeToken();
                }
                else if (tok.kind == TokenKind::Keyword_protected)
                {
                    baseClass.accessLevel = 1;
                    consumeToken();
                }
                else if (tok.kind == TokenKind::Keyword_private)
                {
                    baseClass.accessLevel = 2;
                    consumeToken();
                }

                // 检查是否是虚继承
                if (getCurrentToken().kind == TokenKind::Keyword_virtual)
                {
                    baseClass.isVirtual = true;
                    consumeToken();
                }

                // 解析基类类型
                auto type = typeParser.parseType();
                if (!type)
                {
                    reportError("Invalid base class type");
                    return {};
                }

                bases.push_back(type);
            } while (getCurrentToken().kind == TokenKind::Comma && consumeToken());

            return bases;
        }

        std::vector<std::shared_ptr<Decl>> ClassDeclParser::parseMembers()
        {
            std::vector<std::shared_ptr<Decl>> members;

            // 首先检查是否有访问说明符
            if (isAccessSpecifier(getCurrentToken().kind))
            {
                parseAccessSpecifier();
            }

            // 解析成员声明
            while (!isAccessSpecifier(getCurrentToken().kind) &&
                   getCurrentToken().kind != TokenKind::RBrace)
            {

                // 检查是否是虚函数
                if (getCurrentToken().kind == TokenKind::Keyword_virtual)
                {
                    auto virtualFunc = parseVirtualFunction();
                    if (virtualFunc)
                    {
                        members.push_back(virtualFunc);
                    }
                    continue;
                }

                // 检查是否是成员函数
                if (isFunctionDeclaration(getCurrentToken()))
                {
                    auto memberFunc = parseMemberFunction();
                    if (memberFunc)
                    {
                        members.push_back(memberFunc);
                    }
                    continue;
                }

                // 否则作为成员变量处理
                TypeParser typeParser(lexer, diagnostics);
                auto type = typeParser.parseType();
                if (!type)
                {
                    reportError("Invalid member variable type");
                    continue;
                }

                Token tok = getCurrentToken();
                if (tok.kind != TokenKind::Identifier)
                {
                    reportError("Expected member variable name");
                    continue;
                }

                auto varDecl = std::make_shared<VarDecl>();
                varDecl->type = type;
                varDecl->name = std::string(tok.text);
                consumeToken(); // 消费变量名

                members.push_back(varDecl);

                // 确保每个成员声明都以分号结束
                if (getCurrentToken().kind != TokenKind::Semicolon)
                {
                    reportError("Expected ';' after member declaration");
                    continue;
                }
                consumeToken(); // 消费分号
            }

            return members;
        }

        std::shared_ptr<Decl> ClassDeclParser::parseVirtualFunction()
        {
            assert(getCurrentToken().kind == TokenKind::Keyword_virtual);
            consumeToken(); // 消费virtual关键字

            auto funcDecl = parseMemberFunction();
            if (!funcDecl)
            {
                return nullptr;
            }

            auto asFuncDecl = std::static_pointer_cast<FuncDecl>(funcDecl);
            asFuncDecl->isVirtual = true;

            // 检查是否是纯虚函数
            if (getCurrentToken().kind == TokenKind::Equal)
            {
                consumeToken(); // 消费等号
                if (getCurrentToken().kind == TokenKind::NumberLiteral &&
                    getCurrentToken().intValue == 0)
                {
                    asFuncDecl->isPureVirtual = true;
                    consumeToken(); // 消费0
                }
                else
                {
                    reportError("Expected '0' for pure virtual function");
                    return nullptr;
                }
            }

            return funcDecl;
        }

        void ClassDeclParser::parseAccessSpecifier()
        {
            Token tok = getCurrentToken();
            if (tok.kind == TokenKind::Keyword_public ||
                tok.kind == TokenKind::Keyword_protected ||
                tok.kind == TokenKind::Keyword_private)
            {
                consumeToken(); // 消费访问说明符

                if (getCurrentToken().kind != TokenKind::Colon)
                {
                    reportError("Expected ':' after access specifier");
                    return;
                }
                consumeToken(); // 消费冒号
            }
        }

        std::shared_ptr<Decl> ClassDeclParser::parseMemberFunction()
        {
            auto funcDecl = std::make_shared<FuncDecl>();

            // 解析函数返回类型
            TypeParser typeParser(lexer, diagnostics);
            funcDecl->returnType = typeParser.parseType();
            if (!funcDecl->returnType)
            {
                reportError("Invalid function return type");
                return nullptr;
            }

            // 解析函数名
            Token tok = getCurrentToken();
            if (tok.kind != TokenKind::Identifier)
            {
                reportError("Expected function name");
                return nullptr;
            }
            funcDecl->name = std::string(tok.text);
            consumeToken(); // 消费函数名

            // 解析模板参数（如果有）
            if (getCurrentToken().kind == TokenKind::Less)
            {
                TemplateParser templateParser(lexer, diagnostics);
                funcDecl->templateParams = templateParser.parseTemplateParameters();
                if (funcDecl->templateParams.empty())
                {
                    reportError("Failed to parse template parameters");
                    return nullptr;
                }
            }

            // 解析参数列表
            if (getCurrentToken().kind != TokenKind::LParen)
            {
                reportError("Expected '(' in function declaration");
                return nullptr;
            }
            consumeToken(); // 消费左括号

            // 解析参数
            while (getCurrentToken().kind != TokenKind::RParen)
            {
                if (getCurrentToken().kind == TokenKind::EndOfFile)
                {
                    reportError("Unexpected end of file in parameter list");
                    return nullptr;
                }

                auto type = typeParser.parseType();
                if (!type)
                {
                    reportError("Invalid parameter type");
                    return nullptr;
                }

                tok = getCurrentToken();
                if (tok.kind != TokenKind::Identifier)
                {
                    reportError("Expected parameter name");
                    return nullptr;
                }

                auto param = std::make_shared<VarDecl>();
                param->type = type;
                param->name = std::string(tok.text);
                consumeToken(); // 消费参数名

                funcDecl->params.push_back(param);

                if (getCurrentToken().kind == TokenKind::Comma)
                {
                    consumeToken(); // 消费逗号
                }
                else if (getCurrentToken().kind != TokenKind::RParen)
                {
                    reportError("Expected ',' or ')' in parameter list");
                    return nullptr;
                }
            }
            consumeToken(); // 消费右括号

            // 检查是否有const修饰
            if (getCurrentToken().kind == TokenKind::Keyword_const)
            {
                funcDecl->isConst = true;
                consumeToken();
            }

            // 检查是否有override
            if (getCurrentToken().kind == TokenKind::Keyword_override)
            {
                funcDecl->isOverride = true;
                consumeToken();
            }

            // 检查是否有final
            if (getCurrentToken().kind == TokenKind::Keyword_final)
            {
                funcDecl->isFinal = true;
                consumeToken();
            }

            // 检查是否有函数体
            if (getCurrentToken().kind == TokenKind::LBrace)
            {
                // TODO: 解析函数体
                // 这里需要调用StmtParser来解析函数体
                int braceCount = 1;
                consumeToken(); // 消费左花括号

                while (braceCount > 0 && getCurrentToken().kind != TokenKind::EndOfFile)
                {
                    if (getCurrentToken().kind == TokenKind::LBrace)
                    {
                        braceCount++;
                    }
                    else if (getCurrentToken().kind == TokenKind::RBrace)
                    {
                        braceCount--;
                    }
                    consumeToken();
                }

                if (braceCount > 0)
                {
                    reportError("Unmatched '{' in function body");
                    return nullptr;
                }
            }

            return funcDecl;
        }

    } // namespace frontend
} // namespace rp
