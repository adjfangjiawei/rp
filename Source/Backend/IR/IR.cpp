#include "IR.h"

namespace rp
{
    namespace backend
    {

        class IR::IRImpl
        {
        public:
            IRImpl() {}
            ~IRImpl() {}

            bool generate()
            {
                // TODO: 实现IR生成
                return true;
            }

            std::string serialize() const
            {
                // TODO: 实现IR序列化
                return "";
            }

            bool deserialize(const std::string &data)
            {
                // TODO: 实现IR反序列化
                return true;
            }
        };

        IR::IR() : impl(std::make_unique<IRImpl>()) {}
        IR::~IR() = default;

        bool IR::generate()
        {
            return impl->generate();
        }

        std::string IR::serialize() const
        {
            return impl->serialize();
        }

        bool IR::deserialize(const std::string &data)
        {
            return impl->deserialize(data);
        }

    } // namespace backend
} // namespace rp
