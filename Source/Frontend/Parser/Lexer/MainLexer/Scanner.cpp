
#include "Scanner.h"

namespace rp {
    namespace frontend {

        Scanner::Scanner(DiagnosticEngine* diagEngine) : identifierScanner(diagEngine), operatorScanner(diagEngine) {}

        void Scanner::setSource(const char* src, size_t length, const std::string& filename) {
            identifierScanner.setSource(src, length, filename);
            operatorScanner.setSource(src, length, filename);
        }

        Token Scanner::scanIdentifier() {
            Token result = identifierScanner.scanIdentifier();
            syncScannerPositions();
            return result;
        }

        Token Scanner::scanOperatorOrPunctuation() {
            Token result = operatorScanner.scanOperatorOrPunctuation();
            syncScannerPositions();
            return result;
        }

        void Scanner::syncScannerPositions() {
            // 同步两个扫描器的位置
            size_t pos = identifierScanner.getCurrentPos();
            size_t line = identifierScanner.getCurrentLine();
            size_t column = identifierScanner.getCurrentColumn();

            operatorScanner.setPosition(pos, line, column);
        }

    }  // namespace frontend
}  // namespace rp
