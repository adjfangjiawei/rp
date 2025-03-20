
#include "UTF8Scanner.h"

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        UTF8Scanner::UTF8Scanner(std::shared_ptr<DiagnosticEngine> diagEngine)
            : BaseScanner(std::move(diagEngine)), unicodeScanner(nullptr) {}

        void UTF8Scanner::setSource(const char* src, size_t length, const std::string& filename) {
            BaseScanner::setSource(src, length, filename);
            initUnicodeScanner();
        }

        void UTF8Scanner::initUnicodeScanner() {
            if (source && sourceLength > 0) {
                unicodeScanner = std::make_unique<unicode::UTF8Scanner>(std::string(source, sourceLength));
            } else {
                unicodeScanner.reset();
            }
        }

        std::string UTF8Scanner::scanUTF8Sequence() {
            if (!unicodeScanner || currentPos >= sourceLength) {
                return "";
            }

            unicodeScanner->setPosition(currentPos);
            std::string result = unicodeScanner->scanUTF8Sequence();

            if (!result.empty()) {
                size_t bytesConsumed = result.length();
                currentPos += bytesConsumed;

                // 计算Unicode字符宽度
                uint32_t codepoint;
                std::string_view sv(result);
                size_t dummy;
                codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, dummy);
                currentColumn += unicode::UnicodeProcessing::getCharWidth(codepoint);
            }

            return result;
        }

        bool UTF8Scanner::isValidUTF8Continuation(char c) const {
            return unicode::UnicodeCore::isUtf8ContinuationByte(static_cast<unsigned char>(c));
        }

        bool UTF8Scanner::isValidUTF8FirstByte(char c) const {
            return unicode::UnicodeCore::isValidUtf8FirstByte(static_cast<unsigned char>(c));
        }

        size_t UTF8Scanner::getUTF8SequenceLength(char firstByte) const {
            return unicode::UnicodeCore::getUtf8SequenceLength(static_cast<unsigned char>(firstByte));
        }

        bool UTF8Scanner::isUTF8Char() const {
            if (currentPos >= sourceLength) {
                return false;
            }

            size_t bytesConsumed;
            return unicode::UnicodeProcessing::validateUtf8Sequence(
                std::string(source + currentPos, sourceLength - currentPos), 0, bytesConsumed);
        }

        std::optional<uint32_t> UTF8Scanner::tryPeekCodepoint() const {
            if (!unicodeScanner || currentPos >= sourceLength) {
                return std::nullopt;
            }

            unicodeScanner->setPosition(currentPos);
            uint32_t codepoint;
            if (unicodeScanner->tryPeekCodepoint(codepoint)) {
                return codepoint;
            }
            return std::nullopt;
        }

        size_t UTF8Scanner::lookAheadUTF8(size_t n) const {
            if (!unicodeScanner) {
                return currentPos;
            }

            unicodeScanner->setPosition(currentPos);
            return unicodeScanner->lookAhead(n);
        }

        void UTF8Scanner::reportInvalidUTF8(const std::string& detail) {
            std::string message = "Invalid UTF-8 sequence";
            if (!detail.empty()) {
                message += ": " + detail;
            }

            SourceLocation loc{filename, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn)};
            diagnostics->report(DiagnosticLevel::Error, loc, message);
        }

        void UTF8Scanner::skipInvalidUTF8() {
            if (!unicodeScanner) {
                currentPos++;
                currentColumn++;
                return;
            }

            unicodeScanner->setPosition(currentPos);
            unicodeScanner->skipInvalidUTF8();

            // 更新位置
            size_t newPos = unicodeScanner->position();
            currentColumn += (newPos - currentPos);
            currentPos = newPos;
        }

        uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
            if (!unicodeScanner) {
                return 0;
            }

            unicodeScanner->setPosition(currentPos);
            uint32_t codepoint = unicodeScanner->decodeUTF8Sequence(first);

            if (codepoint != 0) {
                size_t bytesConsumed = getUTF8SequenceLength(first);
                currentPos += bytesConsumed;
                currentColumn += unicode::UnicodeProcessing::getCharWidth(codepoint);
            }

            return codepoint;
        }

        std::pair<uint32_t, size_t> UTF8Scanner::getNextCodepoint() {
            if (currentPos >= sourceLength) {
                return {0, 0};
            }

            size_t bytesConsumed;
            std::string_view sv(source + currentPos, sourceLength - currentPos);
            uint32_t codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);

            if (codepoint != 0) {
                return {codepoint, bytesConsumed};
            }
            return {0, 0};
        }

        std::string UTF8Scanner::collectUTF8Until(uint32_t targetCodepoint) {
            if (!unicodeScanner) {
                return "";
            }

            unicodeScanner->setPosition(currentPos);
            std::string result = unicodeScanner->collectUntil(targetCodepoint);

            if (!result.empty()) {
                // 更新位置，考虑每个Unicode字符的宽度
                std::string_view sv(result);
                size_t pos = 0;
                while (pos < result.length()) {
                    size_t bytesConsumed;
                    uint32_t cp = unicode::UnicodeEncoding::utf8ToCodePoint(sv.substr(pos), bytesConsumed);
                    currentColumn += unicode::UnicodeProcessing::getCharWidth(cp);
                    pos += bytesConsumed;
                }
                currentPos += result.length();
            }

            return result;
        }

        bool UTF8Scanner::skipUTF8Until(uint32_t targetCodepoint) {
            if (!unicodeScanner) {
                return false;
            }

            unicodeScanner->setPosition(currentPos);
            bool result = unicodeScanner->skipUntil(targetCodepoint);

            if (result) {
                size_t newPos = unicodeScanner->position();
                // 重新计算列位置
                while (currentPos < newPos) {
                    size_t bytesConsumed;
                    std::string_view sv(source + currentPos, newPos - currentPos);
                    uint32_t cp = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);
                    currentColumn += unicode::UnicodeProcessing::getCharWidth(cp);
                    currentPos += bytesConsumed;
                }
            }

            return result;
        }

    }  // namespace frontend
}  // namespace rp
