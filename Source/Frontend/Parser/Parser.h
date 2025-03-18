#pragma once
#include "Lexer/Lexer.h"
#include "AST/AST.h"
#include <memory>
#include <vector>
#include <string>

namespace rp
{
    namespace frontend
    {

        class Parser
        {
        public:
            Parser();
            ~Parser();

            // 解析源文件
            bool parseFile(const std::string &filename);

            // 解析源代码字符串
            bool parseString(const std::string &source);

            // 获取解析结果
            const std::vector<std::shared_ptr<ASTNode>> &getAST() const;

            // 获取诊断引擎
            std::shared_ptr<DiagnosticEngine> getDiagnostics() const;

        private:
            class ParserImpl;
            std::unique_ptr<ParserImpl> impl;

            // 禁止拷贝和赋值
            Parser(const Parser &) = delete;
            Parser &operator=(const Parser &) = delete;
        };

    } // namespace frontend
} // namespace rp
