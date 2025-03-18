#include "InitializerParser.h"

namespace rp
{
    namespace frontend
    {

        std::shared_ptr<Expr> InitializerParser::parseInitializer()
        {
            switch (lexer.currentToken().kind)
            {
            case TokenKind::LBrace:
                return parseInitializerList();

            case TokenKind::LParen:
                return parseConstructorCall();

            case TokenKind::Equal:
                return parseAssignmentInit();

            default:
                reportError("Expected initializer");
                return nullptr;
            }
        }

        std::shared_ptr<InitListExpr> InitializerParser::parseInitializerList()
        {
            if (lexer.currentToken().kind != TokenKind::LBrace)
            {
                reportError("Expected '{' at start of initializer list");
                return nullptr;
            }
            lexer.nextToken(); // 消费'{'

            auto initList = std::make_shared<InitListExpr>();

            // 解析初始化器列表
            if (lexer.currentToken().kind != TokenKind::RBrace)
            {
                do
                {
                    // 处理嵌套的初始化器列表
                    std::shared_ptr<Expr> element;
                    if (lexer.currentToken().kind == TokenKind::LBrace)
                    {
                        element = parseInitializerList();
                    }
                    else
                    {
                        element = exprParser.parseExpression();
                    }

                    if (!element)
                        return nullptr;
                    initList->inits.push_back(element);

                    if (lexer.currentToken().kind != TokenKind::Comma)
                        break;
                    lexer.nextToken(); // 消费','

                    // 允许尾随逗号
                    if (lexer.currentToken().kind == TokenKind::RBrace)
                        break;
                } while (true);
            }

            if (lexer.currentToken().kind != TokenKind::RBrace)
            {
                reportError("Expected '}' at end of initializer list");
                return nullptr;
            }
            lexer.nextToken(); // 消费'}'

            return initList;
        }

        std::shared_ptr<Expr> InitializerParser::parseConstructorCall()
        {
            if (lexer.currentToken().kind != TokenKind::LParen)
            {
                reportError("Expected '(' in constructor call");
                return nullptr;
            }
            lexer.nextToken(); // 消费'('

            auto callExpr = std::make_shared<CallExpr>();

            // 解析构造函数参数
            if (lexer.currentToken().kind != TokenKind::RParen)
            {
                do
                {
                    auto arg = exprParser.parseExpression();
                    if (!arg)
                        return nullptr;
                    callExpr->args.push_back(arg);

                    if (lexer.currentToken().kind != TokenKind::Comma)
                        break;
                    lexer.nextToken(); // 消费','
                } while (true);
            }

            if (lexer.currentToken().kind != TokenKind::RParen)
            {
                reportError("Expected ')' in constructor call");
                return nullptr;
            }
            lexer.nextToken(); // 消费')'

            return callExpr;
        }

        std::shared_ptr<Expr> InitializerParser::parseAssignmentInit()
        {
            if (lexer.currentToken().kind != TokenKind::Equal)
            {
                reportError("Expected '=' in initializer");
                return nullptr;
            }
            lexer.nextToken(); // 消费'='

            return exprParser.parseExpression();
        }

        void InitializerParser::reportError(const std::string &message)
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
