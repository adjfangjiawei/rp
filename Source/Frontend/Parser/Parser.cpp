#include "Parser.h"
#include <fstream>
#include <sstream>

namespace rp
{
    namespace frontend
    {

        class Parser::ParserImpl
        {
        public:
            ParserImpl()
                : diagnostics(std::make_shared<DiagnosticEngine>())
            {
                lexer.setDiagnosticEngine(diagnostics);
            }

            // 词法分析器
            Lexer lexer;

            // 当前token
            Token currentToken;

            // AST节点列表
            std::vector<std::shared_ptr<ASTNode>> astNodes;

            // 诊断引擎
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 解析方法
            bool parse(const std::string &source, const std::string &filename)
            {
                lexer.setSource(source, filename);
                return parseTranslationUnit();
            }

            // 获取下一个token
            void advance()
            {
                currentToken = lexer.nextToken();
            }

            // 检查当前token类型
            bool check(TokenKind kind) const
            {
                return currentToken.kind == kind;
            }

            // 匹配并消费token
            bool match(TokenKind kind)
            {
                if (check(kind))
                {
                    advance();
                    return true;
                }
                return false;
            }

            // 期望下一个token是指定类型
            bool expect(TokenKind kind)
            {
                if (check(kind))
                {
                    advance();
                    return true;
                }

                std::string message = "Expected token type ";
                message += std::to_string(static_cast<int>(kind));
                message += ", but got ";
                message += std::to_string(static_cast<int>(currentToken.kind));
                diagnostics->report(DiagnosticLevel::Error,
                                    SourceLocation{lexer.getCurrentLocation()},
                                    message);
                return false;
            }

            // 解析翻译单元（整个源文件）
            bool parseTranslationUnit()
            {
                advance(); // 获取第一个token

                while (!check(TokenKind::EndOfFile))
                {
                    if (auto decl = parseTopLevelDecl())
                    {
                        astNodes.push_back(decl);
                    }
                    else
                    {
                        // 错误恢复：跳过直到找到下一个可能的声明开始
                        recoverToNextDecl();
                    }
                }

                return !diagnostics->hasErrors();
            }

            // 解析顶层声明
            std::shared_ptr<Decl> parseTopLevelDecl()
            {
                switch (currentToken.kind)
                {
                case TokenKind::Keyword_class:
                case TokenKind::Keyword_struct:
                    return parseClassDecl();

                case TokenKind::Keyword_template:
                    return parseTemplateDecl();

                case TokenKind::Keyword_enum:
                    return parseEnumDecl();

                case TokenKind::Keyword_union:
                    return parseUnionDecl();

                default:
                    if (isTypeSpecifier())
                    {
                        return parseDeclaration();
                    }

                    diagnostics->report(DiagnosticLevel::Error,
                                        SourceLocation{lexer.getCurrentLocation()},
                                        "Expected declaration");
                    return nullptr;
                }
            }

            // 错误恢复
            void recoverToNextDecl()
            {
                while (!check(TokenKind::EndOfFile))
                {
                    switch (currentToken.kind)
                    {
                    case TokenKind::Keyword_class:
                    case TokenKind::Keyword_struct:
                    case TokenKind::Keyword_enum:
                    case TokenKind::Keyword_union:
                    case TokenKind::Keyword_template:
                        return;

                    default:
                        advance();
                        break;
                    }
                }
            }

            // 检查是否是类型说明符
            bool isTypeSpecifier() const
            {
                switch (currentToken.kind)
                {
                case TokenKind::Identifier: // 可能是类型名
                case TokenKind::Keyword_const:
                case TokenKind::Keyword_typename:
                    return true;
                default:
                    return false;
                }
            }

            // TODO: 实现其他解析方法
            std::shared_ptr<Decl> parseClassDecl();
            std::shared_ptr<Decl> parseTemplateDecl();
            std::shared_ptr<Decl> parseEnumDecl();
            std::shared_ptr<Decl> parseUnionDecl();
            std::shared_ptr<Decl> parseDeclaration();
        };

        Parser::Parser() : impl(std::make_unique<ParserImpl>()) {}

        Parser::~Parser() = default;

        bool Parser::parseFile(const std::string &filename)
        {
            std::ifstream file(filename);
            if (!file)
            {
                impl->diagnostics->report(DiagnosticLevel::Fatal,
                                          SourceLocation{filename, 0, 0},
                                          "Could not open source file");
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            return parseString(buffer.str());
        }

        bool Parser::parseString(const std::string &source)
        {
            return impl->parse(source, "<input>");
        }

        const std::vector<std::shared_ptr<ASTNode>> &Parser::getAST() const
        {
            return impl->astNodes;
        }

        std::shared_ptr<DiagnosticEngine> Parser::getDiagnostics() const
        {
            return impl->diagnostics;
        }

    } // namespace frontend
} // namespace rp
