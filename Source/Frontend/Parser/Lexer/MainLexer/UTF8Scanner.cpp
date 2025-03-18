
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
                try {
                    unicodeScanner = std::make_unique<unicode::UTF8Scanner>(std::string(source, sourceLength));
                } catch (const std::exception&) {
                    unicodeScanner.reset();
                }
            } else {
                unicodeScanner.reset();
            }
        }

        std::string UTF8Scanner::scanUTF8Sequence() {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return "";
            }

            try {
                unicodeScanner->setPosition(currentPos);
                std::string result = unicodeScanner->scanUTF8Sequence();

                if (!result.empty()) {
                    size_t bytesConsumed = result.length();
                    if (currentPos + bytesConsumed <= sourceLength) {
                        currentPos += bytesConsumed;

                        // 计算Unicode字符宽度
                        uint32_t codepoint;
                        std::string_view sv(result);
                        size_t dummy;
                        codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, dummy);
                        currentColumn += unicode::UnicodeProcessing::getCharWidth(codepoint);
                    }
                }

                return result;
            } catch (const std::exception&) {
                return "";
            }
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
            if (!source || currentPos >= sourceLength) {
                return false;
            }

            try {
                size_t bytesConsumed;
                return unicode::UnicodeProcessing::validateUtf8Sequence(
                    std::string(source + currentPos, sourceLength - currentPos), 0, bytesConsumed);
            } catch (const std::exception&) {
                return false;
            }
        }

        std::optional<uint32_t> UTF8Scanner::tryPeekCodepoint() const {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return std::nullopt;
            }

            try {
                unicodeScanner->setPosition(currentPos);
                uint32_t codepoint;
                if (unicodeScanner->tryPeekCodepoint(codepoint)) {
                    return codepoint;
                }
            } catch (const std::exception&) {
                // 捕获任何可能的异常并返回nullopt
            }
            return std::nullopt;
        }

        size_t UTF8Scanner::lookAheadUTF8(size_t n) const {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return currentPos;
            }

            try {
                unicodeScanner->setPosition(currentPos);
                return unicodeScanner->lookAhead(n);
            } catch (const std::exception&) {
                return currentPos;
            }
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
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                if (currentPos < sourceLength) {
                    currentPos++;
                    currentColumn++;
                }
                return;
            }

            try {
                unicodeScanner->setPosition(currentPos);
                unicodeScanner->skipInvalidUTF8();

                // 更新位置
                size_t newPos = unicodeScanner->position();
                if (newPos > currentPos && newPos <= sourceLength) {
                    currentColumn += (newPos - currentPos);
                    currentPos = newPos;
                } else {
                    // 如果新位置无效，只前进一个字符
                    currentPos++;
                    currentColumn++;
                }
            } catch (const std::exception&) {
                // 发生异常时，只前进一个字符
                currentPos++;
                currentColumn++;
            }
        }

        uint32_t UTF8Scanner::decodeUTF8Sequence(char first) {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return 0;
            }

            try {
                unicodeScanner->setPosition(currentPos);
                uint32_t codepoint = unicodeScanner->decodeUTF8Sequence(first);

                if (codepoint != 0) {
                    size_t bytesConsumed = getUTF8SequenceLength(first);
                    if (currentPos + bytesConsumed <= sourceLength) {
                        currentPos += bytesConsumed;
                        currentColumn += unicode::UnicodeProcessing::getCharWidth(codepoint);
                    }
                }

                return codepoint;
            } catch (const std::exception&) {
                return 0;
            }
        }

        std::pair<uint32_t, size_t> UTF8Scanner::getNextCodepoint() {
            if (!source || currentPos >= sourceLength) {
                return {0, 0};
            }

            try {
                size_t bytesConsumed;
                std::string_view sv(source + currentPos, sourceLength - currentPos);
                uint32_t codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);

                if (codepoint != 0 && bytesConsumed > 0 && currentPos + bytesConsumed <= sourceLength) {
                    return {codepoint, bytesConsumed};
                }
            } catch (const std::exception&) {
                // 捕获任何可能的异常
            }
            return {0, 0};
        }

        std::string UTF8Scanner::collectUTF8Until(uint32_t targetCodepoint) {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return "";
            }

            try {
                unicodeScanner->setPosition(currentPos);
                std::string result = unicodeScanner->collectUntil(targetCodepoint);

                if (!result.empty() && currentPos + result.length() <= sourceLength) {
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
            } catch (const std::exception&) {
                return "";
            }
        }

        bool UTF8Scanner::skipUTF8Until(uint32_t targetCodepoint) {
            if (!unicodeScanner || !source || currentPos >= sourceLength) {
                return false;
            }

            try {
                unicodeScanner->setPosition(currentPos);
                bool result = unicodeScanner->skipUntil(targetCodepoint);

                if (result) {
                    size_t newPos = unicodeScanner->position();
                    if (newPos > currentPos && newPos <= sourceLength) {
                        // 重新计算列位置
                        while (currentPos < newPos) {
                            size_t bytesConsumed;
                            std::string_view sv(source + currentPos, newPos - currentPos);
                            uint32_t cp = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);
                            currentColumn += unicode::UnicodeProcessing::getCharWidth(cp);
                            currentPos += bytesConsumed;
                        }
                    }
                }

                return result;
            } catch (const std::exception&) {
                return false;
            }
        }

    }  // namespace frontend
}  // namespace rp
