#pragma once

#include <string>
#include <vector>

#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        class TokenSerializer {
          public:
            // 将tokens序列化为字符串
            static std::string serialize(const std::vector<Token>& tokens);

            // 从字符串反序列化为tokens
            static bool deserialize(const std::string& data, std::vector<Token>& tokens);

            // 将tokens序列化为二进制格式
            static std::vector<uint8_t> serializeBinary(const std::vector<Token>& tokens);

            // 从二进制格式反序列化为tokens
            static bool deserializeBinary(const std::vector<uint8_t>& data, std::vector<Token>& tokens);

          private:
            // 序列化单个token
            static std::string serializeToken(const Token& token);

            // 反序列化单个token
            static bool deserializeToken(const std::string& data, Token& token);

            // 二进制序列化辅助函数
            static void writeBinary(std::vector<uint8_t>& buffer, const void* data, size_t size);
            static bool readBinary(const std::vector<uint8_t>& buffer, size_t& offset, void* data, size_t size);

            // 字符串编码/解码
            static std::string encodeString(std::string_view str);
            static std::string decodeString(const std::string& str);
        };

    }  // namespace frontend
}  // namespace rp
