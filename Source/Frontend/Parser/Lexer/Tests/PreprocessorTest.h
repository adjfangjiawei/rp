#pragma once

#include <gtest/gtest.h>

#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/ConstantEvaluator.h"
#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/MacroExpander.h"
#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/PreprocessorHandler.h"

namespace rp {
    namespace frontend {
        namespace test {

            class PreprocessorTest : public ::testing::Test {
              protected:
                void SetUp() override {
                    diagnostics = std::make_shared<DiagnosticEngine>();
                    preprocessor = std::make_unique<PreprocessorHandler>(diagnostics.get());
                    macroExpander = std::make_unique<MacroExpander>(diagnostics.get());
                    constantEvaluator = std::make_unique<ConstantEvaluator>(diagnostics.get());
                }

                std::shared_ptr<DiagnosticEngine> diagnostics;
                std::unique_ptr<PreprocessorHandler> preprocessor;
                std::unique_ptr<MacroExpander> macroExpander;
                std::unique_ptr<ConstantEvaluator> constantEvaluator;
            };

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
