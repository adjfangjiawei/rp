
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

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

        Token Lexer::nextToken() {
            // 跳过空白字符和注释
            skipWhitespaceAndComments();

            // 记录token的起始位置
            saveTokenStart();

            // 到达文件末尾
            if (currentPos >= sourceLength) {
                return createToken(TokenKind::EndOfFile);
            }

            char c = source[currentPos];

            // 更新扫描器的位置
            scanner->setPosition(currentPos, currentLine, currentColumn);

            // 标识符或关键字
            if (scanner->isIdentifierStart(c)) {
                Token token = scanner->scanIdentifier();
                updatePositionFromScanner();
                return token;
            }

            // 数字
            if (isdigit(c) || (c == '.' && currentPos + 1 < sourceLength && isdigit(source[currentPos + 1]))) {
                NumberLiteralLexer numberLexer(diagnostics.get());
                numberLexer.setSource(source, sourceLength, filename);
                numberLexer.currentPos = currentPos;
                numberLexer.currentLine = currentLine;
                numberLexer.currentColumn = currentColumn;
                Token token = numberLexer.scan();
                currentPos = numberLexer.currentPos;
                currentLine = numberLexer.currentLine;
                currentColumn = numberLexer.currentColumn;
                return token;
            }

            // 字符字面量
            if (c == '\'') {
                CharacterLiteralLexer charLexer(diagnostics.get());
                charLexer.setSource(source, sourceLength, filename);
                charLexer.currentPos = currentPos;
                charLexer.currentLine = currentLine;
                charLexer.currentColumn = currentColumn;
                Token token = charLexer.scan();
                currentPos = charLexer.currentPos;
                currentLine = charLexer.currentLine;
                currentColumn = charLexer.currentColumn;
                return token;
            }

            // 字符串字面量
            if (c == '"') {
                StringLiteralLexer stringLexer(diagnostics.get());
                stringLexer.setSource(source, sourceLength, filename);
                stringLexer.currentPos = currentPos;
                stringLexer.currentLine = currentLine;
                stringLexer.currentColumn = currentColumn;
                Token token = stringLexer.scan();
                currentPos = stringLexer.currentPos;
                currentLine = stringLexer.currentLine;
                currentColumn = stringLexer.currentColumn;
                return token;
            }

            // 运算符和标点符号
            Token token = scanner->scanOperatorOrPunctuation();
            updatePositionFromScanner();
            return token;
        }

        Token Lexer::peekToken() {
            // 保存当前状态
            size_t savedPos = currentPos;
            size_t savedLine = currentLine;
            size_t savedColumn = currentColumn;
            size_t savedTokenStart = tokenStart;
            size_t savedTokenLine = tokenLine;
            size_t savedTokenColumn = tokenColumn;

            // 获取下一个token
            Token token = nextToken();

            // 恢复状态
            currentPos = savedPos;
            currentLine = savedLine;
            currentColumn = savedColumn;
            tokenStart = savedTokenStart;
            tokenLine = savedTokenLine;
            tokenColumn = savedTokenColumn;

            return token;
        }

        void Lexer::skipWhitespaceAndComments() {
            while (currentPos < sourceLength) {
                char c = source[currentPos];

                // 跳过空白字符
                if (isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 跳过单行注释
                if (c == '/' && currentPos + 1 < sourceLength && source[currentPos + 1] == '/') {
                    currentPos += 2;
                    currentColumn += 2;
                    while (currentPos < sourceLength && source[currentPos] != '\n') {
                        currentPos++;
                        currentColumn++;
                    }
                    continue;
                }

                // 跳过多行注释
                if (c == '/' && currentPos + 1 < sourceLength && source[currentPos + 1] == '*') {
                    currentPos += 2;
                    currentColumn += 2;
                    while (currentPos + 1 < sourceLength) {
                        if (source[currentPos] == '*' && source[currentPos + 1] == '/') {
                            currentPos += 2;
                            currentColumn += 2;
                            break;
                        }
                        if (source[currentPos] == '\n') {
                            currentLine++;
                            currentColumn = 1;
                        } else {
                            currentColumn++;
                        }
                        currentPos++;
                    }
                    continue;
                }

                break;
            }
        }

        Token Lexer::createToken(TokenKind kind, const std::string& text, bool consumeToken) {
            Token token;
            token.kind = kind;
            token.text = text;
            token.filename = filename;
            token.line = tokenLine;
            token.column = tokenColumn;
            if (text.empty()) {
                token.text = std::string_view(source + tokenStart, currentPos - tokenStart);
            }
            if (!consumeToken) {
                restoreToTokenStart();
            }
            return token;
        }

        void Lexer::saveTokenStart() {
            tokenStart = currentPos;
            tokenLine = currentLine;
            tokenColumn = currentColumn;
        }

        void Lexer::restoreToTokenStart() {
            currentPos = tokenStart;
            currentLine = tokenLine;
            currentColumn = tokenColumn;
        }

        void Lexer::updatePositionFromScanner() {
            currentPos = scanner->getCurrentPos();
            currentLine = scanner->getCurrentLine();
            currentColumn = scanner->getCurrentColumn();
        }

    }  // namespace frontend
}  // namespace rp
