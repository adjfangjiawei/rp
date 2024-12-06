#include <sys/types.h>

#include <vector>
namespace rp {
    class FileID {
        int ID = 0;
        friend class SourceManager;

        static FileID get(int V) {
            FileID F;
            F.ID = V;
            return F;
        }
    };

    class FileEntry {
        off_t Size;

      public:
        off_t getSize() const { return Size; }
    };

    class SourceManager {
        FileID MainFileID;
        std::vector<FileEntry> FileEntryTable;

      public:
        FileID getMainFileID() const { return MainFileID; }

        FileID createFileID(FileEntry SourceFile);
        FileID createFileIDImpl(FileEntry SourceFile);
    };

}  // namespace rp