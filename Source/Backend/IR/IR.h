#pragma once
#include <memory>
#include <string>
#include <vector>

namespace rp
{
    namespace backend
    {

        class IR
        {
        public:
            IR();
            ~IR();

            // 生成中间表示
            bool generate();

            // 序列化IR
            std::string serialize() const;

            // 反序列化IR
            bool deserialize(const std::string &data);

        private:
            class IRImpl;
            std::unique_ptr<IRImpl> impl;
        };

    } // namespace backend
} // namespace rp
