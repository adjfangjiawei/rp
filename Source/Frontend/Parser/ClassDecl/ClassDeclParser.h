
#pragma once
#include "../Parser.h"
#include "../AST/AST.h"
#include "../Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>
#include <vector>

namespace rp
{
    namespace frontend
    {

        class ClassDeclParser
        {
        public:
            ClassDeclParser(Lexer &lexer, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), diagnostics(diag) {}

            // 解析类声明
            std::shared_ptr<Decl> parseClassDecl();

            // 解析类定义
            std::shared_ptr<Decl> parseClassDef();

            // 解析继承列表
            std::vector<std::shared_ptr<Type>> parseBaseClasses();

            // 解析类成员
            std::vector<std::shared_ptr<Decl>> parseMembers();

            // 解析虚函数
            std::shared_ptr<Decl> parseVirtualFunction();

        private:
            Lexer &lexer;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 解析访问说明符
            void parseAccessSpecifier();

            // 解析成员函数
            std::shared_ptr<Decl> parseMemberFunction();

            // 辅助函数
            Token getCurrentToken() { return lexer.getCurrentToken(); }
            Token consumeToken() { return lexer.consumeToken(); }
            void reportError(const std::string &message)
            {
                diagnostics->report(getCurrentToken().line, getCurrentToken().column,
                                    message, DiagnosticLevel::Error);
            }

            // 判断是否是访问说明符
            bool isAccessSpecifier(TokenKind kind)
            {
                return kind == TokenKind::Keyword_public ||
                       kind == TokenKind::Keyword_protected ||
                       kind == TokenKind::Keyword_private;
            }

            // 判断是否是函数声明的开始
            bool isFunctionDeclaration(Token tok)
            {
                return tok.kind == TokenKind::Identifier ||
                       tok.kind == TokenKind::Keyword_virtual ||
                       tok.kind == TokenKind::Keyword_static ||
                       tok.kind == TokenKind::Keyword_const;
            }
        };

    } // namespace frontend
} // namespace rp
