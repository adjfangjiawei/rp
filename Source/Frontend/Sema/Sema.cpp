#include "Sema.h"

namespace rp
{
    namespace frontend
    {

        class Sema::SemaImpl
        {
        public:
            SemaImpl() {}
            ~SemaImpl() {}

            bool analyze()
            {
                // TODO: 实现语义分析
                return true;
            }

            bool checkTypes()
            {
                // TODO: 实现类型检查
                return true;
            }

            bool instantiateTemplates()
            {
                // TODO: 实现模板实例化
                return true;
            }
        };

        Sema::Sema() : impl(std::make_unique<SemaImpl>()) {}
        Sema::~Sema() = default;

        bool Sema::analyze()
        {
            return impl->analyze();
        }

        bool Sema::checkTypes()
        {
            return impl->checkTypes();
        }

        bool Sema::instantiateTemplates()
        {
            return impl->instantiateTemplates();
        }

    } // namespace frontend
} // namespace rp
