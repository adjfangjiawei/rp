
#pragma once
#include "../AST/AST.h"
#include "../AST/TemplateDecl.h"
#include "../Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>
#include <vector>

namespace rp
{
    namespace frontend
    {

        class TemplateParser
        {
        public:
            TemplateParser(Lexer &lexer, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), diagnostics(diag) {}

            // 解析函数模板
            std::shared_ptr<Decl> parseFunctionTemplate();

            // 解析类模板
            std::shared_ptr<Decl> parseClassTemplate();

            // 解析变参模板
            std::shared_ptr<Decl> parseVariadicTemplate();

            // 解析常量模板
            std::shared_ptr<Decl> parseConstantTemplate();

            // 解析模板参数列表
            std::vector<std::shared_ptr<TemplateParam>> parseTemplateParameters();

        private:
            Lexer &lexer;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 解析模板实参列表
            std::vector<std::shared_ptr<Expr>> parseTemplateArguments();

            // 解析模板特化
            std::shared_ptr<Decl> parseTemplateSpecialization();

            // 解析单个模板参数
            std::shared_ptr<TemplateParam> parseTemplateParameter();

            // 辅助函数
            Token getCurrentToken() { return lexer.getCurrentToken(); }
            Token consumeToken() { return lexer.consumeToken(); }
            void reportError(const std::string &message)
            {
                diagnostics->report(getCurrentToken().line, getCurrentToken().column,
                                    message, DiagnosticLevel::Error);
            }

            // 检查当前token是否是模板参数关键字
            bool isTemplateParameterKeyword(TokenKind kind)
            {
                return kind == TokenKind::Keyword_typename ||
                       kind == TokenKind::Keyword_class ||
                       kind == TokenKind::Keyword_template;
            }
        };

    } // namespace frontend
} // namespace rp
