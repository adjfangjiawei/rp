#pragma once
#include "AST/Type.h"
#include "Lexer/Lexer.h"
#include "../Diagnostic/Diagnostic.h"
#include <memory>

namespace rp
{
    namespace frontend
    {

        class TypeParser
        {
        public:
            TypeParser(Lexer &lexer, std::shared_ptr<DiagnosticEngine> diag)
                : lexer(lexer), diagnostics(diag) {}

            // 解析类型说明符
            std::shared_ptr<Type> parseType();

        private:
            Lexer &lexer;
            std::shared_ptr<DiagnosticEngine> diagnostics;

            // 解析各种类型
            std::shared_ptr<Type> parseBasicType();
            std::shared_ptr<Type> parsePointerType();
            std::shared_ptr<Type> parseReferenceType();
            std::shared_ptr<Type> parseArrayType();
            std::shared_ptr<Type> parseFunctionType();
            std::shared_ptr<Type> parseQualifiedType();
            std::shared_ptr<Type> parseTemplateType();

            // 解析类型限定符
            bool parseTypeQualifiers(bool &isConst);

            // 错误处理
            void reportError(const std::string &message);
        };

    } // namespace frontend
} // namespace rp
