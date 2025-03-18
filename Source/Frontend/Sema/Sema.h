#pragma once
#include <memory>
#include <string>
#include <vector>

namespace rp
{
    namespace frontend
    {

        class Sema
        {
        public:
            Sema();
            ~Sema();

            // 执行语义分析
            bool analyze();

            // 类型检查
            bool checkTypes();

            // 模板实例化
            bool instantiateTemplates();

        private:
            class SemaImpl;
            std::unique_ptr<SemaImpl> impl;
        };

    } // namespace frontend
} // namespace rp
