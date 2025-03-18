#include "ExprParser.h"

namespace rp
{
    namespace frontend
    {

        // ... [保持之前的代码不变] ...

        std::shared_ptr<Expr> ExprParser::parseLambdaExpr()
        {
            auto lambda = std::make_shared<LambdaExpr>();

            // 解析捕获列表
            if (lexer.currentToken().kind != TokenKind::LBracket)
            {
                reportError("Expected '[' at start of lambda expression");
                return nullptr;
            }
            lexer.nextToken(); // 消费'['

            if (lexer.currentToken().kind != TokenKind::RBracket)
            {
                do
                {
                    // 解析捕获说明符
                    bool isByRef = false;
                    if (lexer.currentToken().kind == TokenKind::Amp)
                    {
                        isByRef = true;
                        lexer.nextToken(); // 消费'&'
                    }

                    if (lexer.currentToken().kind != TokenKind::Identifier)
                    {
                        reportError("Expected identifier in lambda capture");
                        return nullptr;
                    }

                    auto capture = std::make_shared<VarDecl>();
                    capture->name = lexer.currentToken().text;
                    capture->isRef = isByRef;
                    lambda->captures.push_back(capture);

                    lexer.nextToken(); // 消费标识符

                    if (lexer.currentToken().kind != TokenKind::Comma)
                        break;
                    lexer.nextToken(); // 消费','
                } while (true);
            }

            if (lexer.currentToken().kind != TokenKind::RBracket)
            {
                reportError("Expected ']' after lambda captures");
                return nullptr;
            }
            lexer.nextToken(); // 消费']'

            // 解析可选的mutable说明符
            if (lexer.currentToken().kind == TokenKind::Keyword_mutable)
            {
                lambda->isMutable = true;
                lexer.nextToken(); // 消费'mutable'
            }

            // 解析参数列表
            if (lexer.currentToken().kind == TokenKind::LParen)
            {
                lexer.nextToken(); // 消费'('

                if (lexer.currentToken().kind != TokenKind::RParen)
                {
                    do
                    {
                        auto param = std::make_shared<ParmVarDecl>();
                        // TODO: 解析参数类型和名称
                        lambda->params.push_back(param);

                        if (lexer.currentToken().kind != TokenKind::Comma)
                            break;
                        lexer.nextToken(); // 消费','
                    } while (true);
                }

                if (lexer.currentToken().kind != TokenKind::RParen)
                {
                    reportError("Expected ')' after lambda parameters");
                    return nullptr;
                }
                lexer.nextToken(); // 消费')'
            }

            // 解析可选的返回类型
            if (lexer.currentToken().kind == TokenKind::Arrow)
            {
                lexer.nextToken(); // 消费'->'
                // TODO: 解析返回类型
            }

            // 解析lambda体
            if (lexer.currentToken().kind != TokenKind::LBrace)
            {
                reportError("Expected '{' at start of lambda body");
                return nullptr;
            }

            // TODO: 解析lambda体
            lambda->body = nullptr; // 暂时设为nullptr

            return lambda;
        }

        std::shared_ptr<Expr> ExprParser::parseNewExpr()
        {
            lexer.nextToken(); // 消费'new'

            auto newExpr = std::make_shared<NewExpr>();

            // 解析可选的放置参数
            if (lexer.currentToken().kind == TokenKind::LParen)
            {
                // TODO: 解析放置参数
            }

            // 解析类型
            // TODO: 解析类型说明符

            // 解析可选的数组大小
            if (lexer.currentToken().kind == TokenKind::LSquare)
            {
                lexer.nextToken(); // 消费'['
                newExpr->isArray = true;
                newExpr->arraySize = parseExpression();
                if (!newExpr->arraySize)
                    return nullptr;

                if (lexer.currentToken().kind != TokenKind::RSquare)
                {
                    reportError("Expected ']' after array size");
                    return nullptr;
                }
                lexer.nextToken(); // 消费']'
            }

            // 解析可选的初始化器
            if (lexer.currentToken().kind == TokenKind::LParen ||
                lexer.currentToken().kind == TokenKind::LBrace)
            {
                newExpr->initializer = parseInitializer();
                if (!newExpr->initializer)
                    return nullptr;
            }

            return newExpr;
        }

        std::shared_ptr<Expr> ExprParser::parseDeleteExpr()
        {
            lexer.nextToken(); // 消费'delete'

            auto deleteExpr = std::make_shared<DeleteExpr>();

            // 检查是否是数组delete
            if (lexer.currentToken().kind == TokenKind::LSquare)
            {
                lexer.nextToken(); // 消费'['
                if (lexer.currentToken().kind != TokenKind::RSquare)
                {
                    reportError("Expected ']' after 'delete['");
                    return nullptr;
                }
                lexer.nextToken(); // 消费']'
                deleteExpr->isArray = true;
            }

            // 解析要删除的表达式
            deleteExpr->argument = parseUnaryExpr();
            if (!deleteExpr->argument)
                return nullptr;

            return deleteExpr;
        }

        std::shared_ptr<Expr> ExprParser::parseCastExpr()
        {
            auto castExpr = std::make_shared<CastExpr>();

            // 确定转换类型
            if (lexer.currentToken().kind == TokenKind::LParen)
            {
                castExpr->castKind = CastExpr::CastKind::CStyle;
                lexer.nextToken(); // 消费'('

                // TODO: 解析类型名

                if (lexer.currentToken().kind != TokenKind::RParen)
                {
                    reportError("Expected ')' after type in cast expression");
                    return nullptr;
                }
                lexer.nextToken(); // 消费')'
            }
            else
            {
                std::string castName = lexer.currentToken().text;
                if (castName == "static_cast")
                    castExpr->castKind = CastExpr::CastKind::Static;
                else if (castName == "dynamic_cast")
                    castExpr->castKind = CastExpr::CastKind::Dynamic;
                else if (castName == "const_cast")
                    castExpr->castKind = CastExpr::CastKind::Const;
                else if (castName == "reinterpret_cast")
                    castExpr->castKind = CastExpr::CastKind::Reinterpret;
                else
                {
                    reportError("Unknown cast operator");
                    return nullptr;
                }

                lexer.nextToken(); // 消费cast关键字

                if (lexer.currentToken().kind != TokenKind::Less)
                {
                    reportError("Expected '<' after cast operator");
                    return nullptr;
                }
                lexer.nextToken(); // 消费'<'

                // TODO: 解析类型名

                if (lexer.currentToken().kind != TokenKind::Greater)
                {
                    reportError("Expected '>' after type in cast expression");
                    return nullptr;
                }
                lexer.nextToken(); // 消费'>'

                if (lexer.currentToken().kind != TokenKind::LParen)
                {
                    reportError("Expected '(' after cast type");
                    return nullptr;
                }
                lexer.nextToken(); // 消费'('
            }

            // 解析要转换的表达式
            castExpr->operand = parseExpression();
            if (!castExpr->operand)
                return nullptr;

            if (lexer.currentToken().kind != TokenKind::RParen)
            {
                reportError("Expected ')' after cast expression");
                return nullptr;
            }
            lexer.nextToken(); // 消费')'

            return castExpr;
        }

        // ... [保持其他代码不变] ...

    } // namespace frontend
} // namespace rp
