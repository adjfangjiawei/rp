
#include "Scanner.h"

namespace rp {
    namespace frontend {

        Scanner::Scanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : identifierScanner(diagEngine), operatorScanner(diagEngine) {}

        void Scanner::setSource(const char* src, size_t length, const std::string& filename) {
            source = src;
            sourceLength = length;
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
            // 获取两个扫描器的当前位置
            size_t idPos = identifierScanner.getCurrentPos();
            size_t opPos = operatorScanner.getCurrentPos();

            // 使用最大的位置来确保前进
            size_t newPos = std::max(idPos, opPos);

            // 重新计算行和列
            size_t newLine = getCurrentLine();
            size_t newColumn = getCurrentColumn();
            for (size_t i = std::min(idPos, opPos); i < newPos && i < sourceLength; ++i) {
                if (source[i] == '\n') {
                    newLine++;
                    newColumn = 1;
                } else {
                    newColumn++;
                }
            }

            // 同步两个扫描器的位置
            identifierScanner.setPosition(newPos, newLine, newColumn);
            operatorScanner.setPosition(newPos, newLine, newColumn);
        }

    }  // namespace frontend
}  // namespace rp
