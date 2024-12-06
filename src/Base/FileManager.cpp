#include <Base/FileManager.h>
using namespace rp;
FileID SourceManager::createFileID(FileEntry SourceFile) {
    return this->createFileIDImpl(SourceFile);
}

FileID SourceManager::createFileIDImpl(FileEntry SourceFile) {
    this->FileEntryTable.emplace_back(SourceFile);
    FileID FID = FileID::get(FileEntryTable.size() - 1);
    return FID;
}