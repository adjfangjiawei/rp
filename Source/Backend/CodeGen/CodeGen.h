#pragma once
#include <memory>
#include <string>

namespace rp
{
    namespace backend
    {

        class CodeGen
        {
        public:
            CodeGen();
            ~CodeGen();

            // 生成目标代码
            bool generate(const std::string &outputPath);

            // 设置优化级别
            void setOptimizationLevel(int level);

        private:
            class CodeGenImpl;
            std::unique_ptr<CodeGenImpl> impl;
        };

    } // namespace backend
} // namespace rp
