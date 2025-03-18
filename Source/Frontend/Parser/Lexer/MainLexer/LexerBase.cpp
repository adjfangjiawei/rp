
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

        Lexer::Lexer()
            : source(nullptr),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = std::make_shared<DiagnosticEngine>();
            scanner = std::make_unique<Scanner>(diagnostics.get());
            KeywordManager::initialize();
        }

        Lexer::Lexer(DiagnosticEngine* diagEngine)
            : source(nullptr),
              currentPos(0),
              currentLine(1),
              currentColumn(1),
              sourceLength(0),
              tokenStart(0),
              tokenLine(1),
              tokenColumn(1) {
            diagnostics = std::shared_ptr<DiagnosticEngine>(diagEngine);
            scanner = std::make_unique<Scanner>(diagEngine);
            KeywordManager::initialize();
        }

        void Lexer::setSource(const char* src, size_t length, const std::string& filename) {
            source = src;
            sourceLength = length;
            this->filename = filename;
            currentPos = 0;
            currentLine = 1;
            currentColumn = 1;
            tokenStart = 0;
            tokenLine = 1;
            tokenColumn = 1;

            scanner->setSource(src, length, filename);
        }

    }  // namespace frontend
}  // namespace rp
