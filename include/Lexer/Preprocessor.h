#include "Base/FileManager.h"
namespace rp {
    class Preprocessor {
        SourceManager SourceMgr;

      public:
        void EnterMainSourceFile();
    };
}  // namespace rp