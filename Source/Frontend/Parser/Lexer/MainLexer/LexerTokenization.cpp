
#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {

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
                currentColumn = charLexer.currentColumn;
                Token token = charLexer.scan();
                currentPos = charLexer.currentPos;
                currentLine = charLexer.currentLine;
                currentColumn = charLexer.currentColumn;
                return token;
            }

            // 字符串字面量处理
            if (c == '"' || (c == 'R' && currentPos + 1 < sourceLength && source[currentPos + 1] == '"')) {
                // 检查是否是原始字符串字面量
                bool isRawString = (c == 'R');
                if (isRawString) {
                    currentPos++;  // 跳过'R'
                }

                StringLiteralLexer stringLexer(diagnostics.get());
                stringLexer.setSource(source, sourceLength, filename);
                stringLexer.currentPos = currentPos;
                stringLexer.currentLine = currentLine;
                stringLexer.currentColumn = currentColumn;

                Token token = stringLexer.scan();

                // 更新位置信息
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

    }  // namespace frontend
}  // namespace rp
