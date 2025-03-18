#include "CodeGen.h"

namespace rp
{
    namespace backend
    {

        class CodeGen::CodeGenImpl
        {
        public:
            CodeGenImpl() : optimizationLevel(0) {}
            ~CodeGenImpl() {}

            bool generate(const std::string &outputPath)
            {
                // TODO: 实现代码生成
                return true;
            }

            void setOptimizationLevel(int level)
            {
                optimizationLevel = level;
            }

        private:
            int optimizationLevel;
        };

        CodeGen::CodeGen() : impl(std::make_unique<CodeGenImpl>()) {}
        CodeGen::~CodeGen() = default;

        bool CodeGen::generate(const std::string &outputPath)
        {
            return impl->generate(outputPath);
        }

        void CodeGen::setOptimizationLevel(int level)
        {
            impl->setOptimizationLevel(level);
        }

    } // namespace backend
} // namespace rp
