#include <random>
#include <string>

#include "Frontend/Parser/Lexer/TokenLexer/Serializer/TokenSerializer.h"
#include "PreprocessorTest.h"

namespace rp {
    namespace frontend {
        namespace test {

            class TokenSerializerTest : public ::testing::Test {
              protected:
                // 生成测试用的token序列
                std::vector<Token> generateTestTokens(size_t count) {
                    std::vector<Token> tokens;
                    tokens.reserve(count);

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> tokenKindDist(0, 100);  // 假设TokenKind的范围
                    std::uniform_int_distribution<> lineDist(1, 1000);
                    std::uniform_int_distribution<> colDist(1, 100);

                    for (size_t i = 0; i < count; ++i) {
                        Token token(static_cast<TokenKind>(tokenKindDist(gen)));
                        token.setText(std::string("token_" + std::to_string(i)));
                        token.line = lineDist(gen);
                        token.column = colDist(gen);
                        token.filename = "test.cpp";
                        tokens.push_back(std::move(token));
                    }

                    return tokens;
                }

                // 比较两个token是否相等
                bool compareTokens(const Token& a, const Token& b) {
                    return a.kind == b.kind && a.text == b.text && a.line == b.line && a.column == b.column &&
                           a.filename == b.filename;
                }
            };

            // 测试文本序列化
            TEST_F(TokenSerializerTest, TextSerialization) {
                auto originalTokens = generateTestTokens(100);

                // 序列化
                std::string serialized = TokenSerializer::serialize(originalTokens);
                EXPECT_FALSE(serialized.empty());

                // 反序列化
                std::vector<Token> deserializedTokens;
                EXPECT_TRUE(TokenSerializer::deserialize(serialized, deserializedTokens));

                // 验证结果
                ASSERT_EQ(originalTokens.size(), deserializedTokens.size());
                for (size_t i = 0; i < originalTokens.size(); ++i) {
                    EXPECT_TRUE(compareTokens(originalTokens[i], deserializedTokens[i]))
                        << "Tokens differ at index " << i;
                }
            }

            // 测试二进制序列化
            TEST_F(TokenSerializerTest, BinarySerialization) {
                auto originalTokens = generateTestTokens(100);

                // 序列化
                auto serialized = TokenSerializer::serializeBinary(originalTokens);
                EXPECT_FALSE(serialized.empty());

                // 反序列化
                std::vector<Token> deserializedTokens;
                EXPECT_TRUE(TokenSerializer::deserializeBinary(serialized, deserializedTokens));

                // 验证结果
                ASSERT_EQ(originalTokens.size(), deserializedTokens.size());
                for (size_t i = 0; i < originalTokens.size(); ++i) {
                    EXPECT_TRUE(compareTokens(originalTokens[i], deserializedTokens[i]))
                        << "Tokens differ at index " << i;
                }
            }

            // 测试特殊字符处理
            TEST_F(TokenSerializerTest, SpecialCharacters) {
                std::vector<Token> tokens;

                // 创建带有特殊字符的Token
                Token t1(TokenKind::StringLiteral);
                t1.setText(std::string_view("Hello\nWorld"));
                tokens.push_back(std::move(t1));

                Token t2(TokenKind::StringLiteral);
                t2.setText(std::string_view("Tab\there"));
                tokens.push_back(std::move(t2));

                Token t3(TokenKind::StringLiteral);
                t3.setText(std::string_view("Quote\"inside"));
                tokens.push_back(std::move(t3));

                Token t4(TokenKind::StringLiteral);
                t4.setText(std::string_view("Unicode\u2022bullet"));
                tokens.push_back(std::move(t4));

                // 文本序列化
                std::string serialized = TokenSerializer::serialize(tokens);
                std::vector<Token> deserializedTokens;
                EXPECT_TRUE(TokenSerializer::deserialize(serialized, deserializedTokens));

                ASSERT_EQ(tokens.size(), deserializedTokens.size());
                for (size_t i = 0; i < tokens.size(); ++i) {
                    EXPECT_TRUE(compareTokens(tokens[i], deserializedTokens[i])) << "Tokens differ at index " << i;
                }
            }

            // 测试大量数据
            TEST_F(TokenSerializerTest, LargeDataSet) {
                auto originalTokens = generateTestTokens(10000);

                // 测试文本序列化性能
                auto startText = std::chrono::high_resolution_clock::now();
                std::string textSerialized = TokenSerializer::serialize(originalTokens);
                std::vector<Token> textDeserialized;
                EXPECT_TRUE(TokenSerializer::deserialize(textSerialized, textDeserialized));
                auto endText = std::chrono::high_resolution_clock::now();
                auto textTime = std::chrono::duration_cast<std::chrono::milliseconds>(endText - startText);

                // 测试二进制序列化性能
                auto startBinary = std::chrono::high_resolution_clock::now();
                auto binarySerialized = TokenSerializer::serializeBinary(originalTokens);
                std::vector<Token> binaryDeserialized;
                EXPECT_TRUE(TokenSerializer::deserializeBinary(binarySerialized, binaryDeserialized));
                auto endBinary = std::chrono::high_resolution_clock::now();
                auto binaryTime = std::chrono::duration_cast<std::chrono::milliseconds>(endBinary - startBinary);

                // 验证结果
                EXPECT_EQ(originalTokens.size(), textDeserialized.size());
                EXPECT_EQ(originalTokens.size(), binaryDeserialized.size());

                // 比较性能
                EXPECT_LT(binaryTime, textTime) << "Binary serialization should be faster";
            }

            // 测试错误处理
            TEST_F(TokenSerializerTest, ErrorHandling) {
                // 测试空数据
                std::vector<Token> emptyTokens;
                EXPECT_NO_THROW({
                    std::string serialized = TokenSerializer::serialize(emptyTokens);
                    std::vector<Token> deserialized;
                    EXPECT_TRUE(TokenSerializer::deserialize(serialized, deserialized));
                    EXPECT_TRUE(deserialized.empty());
                });

                // 测试损坏的数据
                std::string corruptedData = "invalid data format";
                std::vector<Token> tokens;
                EXPECT_FALSE(TokenSerializer::deserialize(corruptedData, tokens));

                // 测试不完整的二进制数据
                std::vector<uint8_t> incompleteData = {0x01, 0x02, 0x03};
                EXPECT_FALSE(TokenSerializer::deserializeBinary(incompleteData, tokens));
            }

            // 测试边界情况
            TEST_F(TokenSerializerTest, EdgeCases) {
                std::vector<Token> tokens;

                // 空token
                Token emptyToken(TokenKind::EndOfFile);
                emptyToken.line = 0;
                emptyToken.column = 0;
                tokens.push_back(std::move(emptyToken));

                // 大数值token
                Token largeToken(TokenKind::NumberLiteral);
                largeToken.setText(std::string_view("9999999999999999"));
                largeToken.line = 999999;
                largeToken.column = 999999;
                largeToken.filename = "very/long/path/to/file.cpp";
                tokens.push_back(std::move(largeToken));

                // 测试文本序列化
                std::string serialized = TokenSerializer::serialize(tokens);
                std::vector<Token> deserializedTokens;
                EXPECT_TRUE(TokenSerializer::deserialize(serialized, deserializedTokens));
                ASSERT_EQ(tokens.size(), deserializedTokens.size());

                // 测试二进制序列化
                auto binarySerialized = TokenSerializer::serializeBinary(tokens);
                std::vector<Token> binaryDeserialized;
                EXPECT_TRUE(TokenSerializer::deserializeBinary(binarySerialized, binaryDeserialized));
                ASSERT_EQ(tokens.size(), binaryDeserialized.size());
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
