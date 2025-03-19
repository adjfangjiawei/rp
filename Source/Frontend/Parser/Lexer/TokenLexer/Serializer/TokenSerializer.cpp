#include "TokenSerializer.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace rp {
    namespace frontend {

        std::string TokenSerializer::serialize(const std::vector<Token>& tokens) {
            std::stringstream ss;
            ss << tokens.size() << "\n";  // 写入token数量

            for (const auto& token : tokens) {
                ss << serializeToken(token) << "\n";
            }

            return ss.str();
        }

        bool TokenSerializer::deserialize(const std::string& data, std::vector<Token>& tokens) {
            std::stringstream ss(data);
            size_t count;
            ss >> count;

            if (ss.fail()) {
                return false;
            }

            tokens.clear();
            tokens.reserve(count);

            std::string line;
            std::getline(ss, line);  // 读取剩余的换行符

            for (size_t i = 0; i < count; ++i) {
                std::getline(ss, line);
                if (ss.fail()) {
                    return false;
                }

                Token token;
                if (!deserializeToken(line, token)) {
                    return false;
                }
                tokens.push_back(std::move(token));
            }

            return true;
        }

        std::vector<uint8_t> TokenSerializer::serializeBinary(const std::vector<Token>& tokens) {
            std::vector<uint8_t> buffer;

            // 写入token数量
            size_t count = tokens.size();
            writeBinary(buffer, &count, sizeof(count));

            // 写入每个token
            for (const auto& token : tokens) {
                // 写入token类型
                uint32_t kind = static_cast<uint32_t>(token.kind);
                writeBinary(buffer, &kind, sizeof(kind));

                // 写入文本内容
                size_t textLength = token.text.length();
                writeBinary(buffer, &textLength, sizeof(textLength));
                std::string text_str{token.text};
                writeBinary(buffer, text_str.data(), textLength);

                // 写入位置信息
                writeBinary(buffer, &token.line, sizeof(token.line));
                writeBinary(buffer, &token.column, sizeof(token.column));

                // 写入文件名
                size_t filenameLength = token.filename.length();
                writeBinary(buffer, &filenameLength, sizeof(filenameLength));
                writeBinary(buffer, token.filename.data(), filenameLength);
            }

            return buffer;
        }

        bool TokenSerializer::deserializeBinary(const std::vector<uint8_t>& data, std::vector<Token>& tokens) {
            size_t offset = 0;
            size_t count;

            // 读取token数量
            if (!readBinary(data, offset, &count, sizeof(count))) {
                return false;
            }

            tokens.clear();
            tokens.reserve(count);

            // 读取每个token
            for (size_t i = 0; i < count; ++i) {
                Token token;

                // 读取token类型
                uint32_t kind;
                if (!readBinary(data, offset, &kind, sizeof(kind))) {
                    return false;
                }
                token.kind = static_cast<TokenKind>(kind);

                // 读取文本内容
                size_t textLength;
                if (!readBinary(data, offset, &textLength, sizeof(textLength))) {
                    return false;
                }
                std::string text(textLength, '\0');
                if (!readBinary(data, offset, &text[0], textLength)) {
                    return false;
                }
                token.text = std::move(text);

                // 读取位置信息
                if (!readBinary(data, offset, &token.line, sizeof(token.line)) ||
                    !readBinary(data, offset, &token.column, sizeof(token.column))) {
                    return false;
                }

                // 读取文件名
                size_t filenameLength;
                if (!readBinary(data, offset, &filenameLength, sizeof(filenameLength))) {
                    return false;
                }
                std::string filename(filenameLength, '\0');
                if (!readBinary(data, offset, &filename[0], filenameLength)) {
                    return false;
                }
                token.filename = std::move(filename);

                tokens.push_back(std::move(token));
            }

            return true;
        }

        std::string TokenSerializer::serializeToken(const Token& token) {
            std::stringstream ss;
            ss << static_cast<int>(token.kind) << " " << encodeString(token.text) << " " << token.line << " "
               << token.column << " " << encodeString(token.filename);
            return ss.str();
        }

        bool TokenSerializer::deserializeToken(const std::string& data, Token& token) {
            std::stringstream ss(data);
            int kind;
            std::string encodedText, encodedFilename;

            if (!(ss >> kind >> encodedText >> token.line >> token.column >> encodedFilename)) {
                return false;
            }

            token.kind = static_cast<TokenKind>(kind);
            token.text = decodeString(encodedText);
            token.filename = decodeString(encodedFilename);

            return true;
        }

        void TokenSerializer::writeBinary(std::vector<uint8_t>& buffer, const void* data, size_t size) {
            const uint8_t* bytes = static_cast<const uint8_t*>(data);
            buffer.insert(buffer.end(), bytes, bytes + size);
        }

        bool TokenSerializer::readBinary(const std::vector<uint8_t>& buffer, size_t& offset, void* data, size_t size) {
            if (offset + size > buffer.size()) {
                return false;
            }

            std::memcpy(data, buffer.data() + offset, size);
            offset += size;
            return true;
        }

        std::string TokenSerializer::encodeString(std::string_view str) {
            std::stringstream ss;
            ss << std::hex;
            for (unsigned char c : str) {
                ss << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            }
            return ss.str();
        }

        std::string TokenSerializer::decodeString(const std::string& str) {
            std::string result;
            result.reserve(str.length() / 2);

            for (size_t i = 0; i < str.length(); i += 2) {
                std::string byteString = str.substr(i, 2);
                char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
                result.push_back(byte);
            }

            return result;
        }

    }  // namespace frontend
}  // namespace rp
