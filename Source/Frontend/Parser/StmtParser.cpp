#include "StmtParser.h"

namespace rp
{
    namespace frontend
    {

        // ... [保持之前的代码不变] ...

        std::shared_ptr<Stmt> StmtParser::parseBreakStmt()
        {
            if (!isInLoop() && !isInSwitch())
            {
                reportError("'break' statement not in loop or switch statement");
                return nullptr;
            }

            lexer.nextToken(); // 消费'break'

            auto breakStmt = std::make_shared<BreakStmt>();

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after 'break'");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return breakStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseContinueStmt()
        {
            if (!isInLoop())
            {
                reportError("'continue' statement not in loop statement");
                return nullptr;
            }

            lexer.nextToken(); // 消费'continue'

            auto continueStmt = std::make_shared<ContinueStmt>();

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after 'continue'");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return continueStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseReturnStmt()
        {
            lexer.nextToken(); // 消费'return'

            auto returnStmt = std::make_shared<ReturnStmt>();

            // 检查是否有返回值
            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                returnStmt->returnValue = exprParser.parseExpression();
                if (!returnStmt->returnValue)
                    return nullptr;
            }

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after return statement");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return returnStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseGotoStmt()
        {
            lexer.nextToken(); // 消费'goto'

            if (lexer.currentToken().kind != TokenKind::Identifier)
            {
                reportError("Expected identifier after 'goto'");
                return nullptr;
            }

            auto gotoStmt = std::make_shared<GotoStmt>();
            gotoStmt->label = lexer.currentToken().text;
            lexer.nextToken(); // 消费标识符

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after goto statement");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return gotoStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseLabelStmt()
        {
            std::string label = lexer.currentToken().text;
            lexer.nextToken(); // 消费标识符

            if (lexer.currentToken().kind != TokenKind::Colon)
            {
                reportError("Expected ':' after label");
                return nullptr;
            }
            lexer.nextToken(); // 消费':'

            auto labelStmt = std::make_shared<LabelStmt>();
            labelStmt->label = label;

            // 解析标签后的语句
            labelStmt->subStmt = parseStatement();
            if (!labelStmt->subStmt)
                return nullptr;

            return labelStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseDefaultStmt()
        {
            if (!isInSwitch())
            {
                reportError("'default' label not within switch statement");
                return nullptr;
            }

            lexer.nextToken(); // 消费'default'

            if (lexer.currentToken().kind != TokenKind::Colon)
            {
                reportError("Expected ':' after 'default'");
                return nullptr;
            }
            lexer.nextToken(); // 消费':'

            auto defaultStmt = std::make_shared<DefaultStmt>();
            defaultStmt->subStmt = parseStatement();
            if (!defaultStmt->subStmt)
                return nullptr;

            return defaultStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseExpressionStmt()
        {
            auto exprStmt = std::make_shared<ExprStmt>();

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                exprStmt->expression = exprParser.parseExpression();
                if (!exprStmt->expression)
                    return nullptr;
            }

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after expression");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return exprStmt;
        }

        std::shared_ptr<Stmt> StmtParser::parseDeclarationStmt()
        {
            auto declStmt = std::make_shared<DeclStmt>();

            // TODO: 解析声明说明符和声明符
            // 这里需要类型解析器的支持

            if (lexer.currentToken().kind != TokenKind::Semicolon)
            {
                reportError("Expected ';' after declaration");
                return nullptr;
            }
            lexer.nextToken(); // 消费';'

            return declStmt;
        }

        // ... [保持其他代码不变] ...

    } // namespace frontend
} // namespace rp
