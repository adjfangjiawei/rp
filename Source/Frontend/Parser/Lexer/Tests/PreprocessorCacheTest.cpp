#include <chrono>
#include <fstream>
#include <random>

#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/PreprocessorCache.h"
#include "PreprocessorTest.h"

namespace rp {
    namespace frontend {
        namespace test {

            class PreprocessorCacheTest : public ::testing::Test {
              protected:
                void SetUp() override {
                    CacheConfig config;
                    config.useCompression = true;
                    config.useBinaryFormat = true;
                    config.compressionLevel = 6;
                    config.maxCacheSize = 1024 * 1024;  // 1MB
                    config.maxEntrySize = 100 * 1024;   // 100KB

                    cache = std::make_unique<PreprocessorCache>(config);
                }

                // 生成测试用的token序列
                std::vector<Token> generateTokens(size_t count, size_t textSize = 10) {
                    std::vector<Token> tokens;
                    tokens.reserve(count);

                    static const std::vector<TokenKind> kinds = {TokenKind::Identifier,
                                                                 TokenKind::NumberLiteral,
                                                                 TokenKind::StringLiteral,
                                                                 TokenKind::Plus,
                                                                 TokenKind::Minus,
                                                                 TokenKind::Star};

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> kindDist(0, kinds.size() - 1);
                    std::uniform_int_distribution<> lineDist(1, 1000);
                    std::uniform_int_distribution<> colDist(1, 100);

                    for (size_t i = 0; i < count; ++i) {
                        Token token;
                        token.kind = kinds[kindDist(gen)];

                        // 生成随机文本
                        std::string text(textSize, 'a');
                        for (char& c : text) {
                            c = 'a' + (gen() % 26);
                        }
                        token.text = text;

                        token.line = lineDist(gen);
                        token.column = colDist(gen);
                        token.filename = "test.cpp";
                        tokens.push_back(token);
                    }

                    return tokens;
                }

                // 创建临时文件
                std::string createTempFile(const std::string& content) {
                    static int counter = 0;
                    std::string filename = "test_" + std::to_string(counter++) + ".cpp";
                    std::ofstream file(filename);
                    file << content;
                    file.close();
                    return filename;
                }

                void removeTempFile(const std::string& filename) { std::remove(filename.c_str()); }

                std::unique_ptr<PreprocessorCache> cache;
            };

            // 基本缓存功能测试
            TEST_F(PreprocessorCacheTest, BasicCaching) {
                std::string filename = createTempFile("test content");
                auto tokens = generateTokens(100);

                // 添加到缓存
                EXPECT_TRUE(cache->addFileCache(filename, tokens));

                // 从缓存获取
                std::vector<Token> cachedTokens;
                EXPECT_TRUE(cache->getFileCache(filename, cachedTokens));

                // 验证内容
                ASSERT_EQ(tokens.size(), cachedTokens.size());
                for (size_t i = 0; i < tokens.size(); ++i) {
                    EXPECT_EQ(tokens[i].kind, cachedTokens[i].kind);
                    EXPECT_EQ(tokens[i].text, cachedTokens[i].text);
                }

                removeTempFile(filename);
            }

            // 压缩测试
            TEST_F(PreprocessorCacheTest, Compression) {
                // 生成大量token以触发压缩
                auto tokens = generateTokens(1000, 100);  // 1000个token，每个100字符
                std::string filename = createTempFile("large content");

                // 添加到缓存
                EXPECT_TRUE(cache->addFileCache(filename, tokens));

                // 获取统计信息
                auto stats = cache->getStatistics();

                // 验证压缩比
                EXPECT_GT(stats.compressionRatio, 100);  // 压缩比应该大于1:1

                removeTempFile(filename);
            }

            // 缓存淘汰测试
            TEST_F(PreprocessorCacheTest, CacheEviction) {
                std::vector<std::string> files;
                const int fileCount = 20;

                // 创建多个文件并缓存
                for (int i = 0; i < fileCount; ++i) {
                    std::string filename = createTempFile("content " + std::to_string(i));
                    files.push_back(filename);
                    auto tokens = generateTokens(1000, 50);  // 生成足够大的数据
                    EXPECT_TRUE(cache->addFileCache(filename, tokens));
                }

                // 验证旧的缓存项被淘汰
                auto stats = cache->getStatistics();
                EXPECT_LT(stats.totalFilesCached, fileCount);

                // 清理文件
                for (const auto& file : files) {
                    removeTempFile(file);
                }
            }

            // 宏缓存测试
            TEST_F(PreprocessorCacheTest, MacroCaching) {
                auto tokens = generateTokens(10);
                std::vector<Token> args = generateTokens(2);

                // 添加宏缓存
                EXPECT_TRUE(cache->addMacroCache("TEST_MACRO", tokens, true, args));

                // 获取宏缓存
                std::vector<Token> cachedTokens;
                EXPECT_TRUE(cache->getMacroCache("TEST_MACRO", cachedTokens, args));

                // 验证内容
                ASSERT_EQ(tokens.size(), cachedTokens.size());
            }

            // 性能测试
            TEST_F(PreprocessorCacheTest, Performance) {
                const int iterations = 1000;
                auto tokens = generateTokens(100);
                std::string filename = createTempFile("perf test content");

                // 测量写入性能
                auto writeStart = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < iterations; ++i) {
                    cache->addFileCache(filename, tokens);
                }
                auto writeEnd = std::chrono::high_resolution_clock::now();
                auto writeTime = std::chrono::duration_cast<std::chrono::milliseconds>(writeEnd - writeStart);

                // 测量读取性能
                auto readStart = std::chrono::high_resolution_clock::now();
                std::vector<Token> cachedTokens;
                for (int i = 0; i < iterations; ++i) {
                    cache->getFileCache(filename, cachedTokens);
                }
                auto readEnd = std::chrono::high_resolution_clock::now();
                auto readTime = std::chrono::duration_cast<std::chrono::milliseconds>(readEnd - readStart);

                // 输出性能统计
                std::cout << "Cache Performance:\n"
                          << "Write time: " << writeTime.count() << "ms\n"
                          << "Read time: " << readTime.count() << "ms\n"
                          << "Average write: " << (writeTime.count() / iterations) << "ms\n"
                          << "Average read: " << (readTime.count() / iterations) << "ms\n";

                removeTempFile(filename);
            }

            // 压力测试
            TEST_F(PreprocessorCacheTest, StressTest) {
                const int fileCount = 100;
                const int operationsPerFile = 100;
                std::vector<std::string> files;

                // 创建多个文件
                for (int i = 0; i < fileCount; ++i) {
                    files.push_back(createTempFile("stress test " + std::to_string(i)));
                }

                // 随机操作
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> fileDist(0, fileCount - 1);
                std::uniform_int_distribution<> opDist(0, 2);  // 0=read, 1=write, 2=invalidate

                for (int i = 0; i < fileCount * operationsPerFile; ++i) {
                    int fileIndex = fileDist(gen);
                    int operation = opDist(gen);

                    switch (operation) {
                        case 0:
                            {  // read
                                std::vector<Token> tokens;
                                cache->getFileCache(files[fileIndex], tokens);
                                break;
                            }
                        case 1:
                            {  // write
                                auto tokens = generateTokens(50);
                                cache->addFileCache(files[fileIndex], tokens);
                                break;
                            }
                        case 2:  // invalidate
                            cache->invalidateFileCache(files[fileIndex]);
                            break;
                    }
                }

                // 验证缓存状态
                auto stats = cache->getStatistics();
                EXPECT_GT(stats.cacheHits + stats.cacheMisses, 0);

                // 清理文件
                for (const auto& file : files) {
                    removeTempFile(file);
                }
            }

            // 错误处理测试
            TEST_F(PreprocessorCacheTest, ErrorHandling) {
                // 测试不存在的文件
                std::vector<Token> tokens;
                EXPECT_FALSE(cache->getFileCache("nonexistent.cpp", tokens));

                // 测试无效的缓存项
                std::string filename = createTempFile("");
                auto originalTokens = generateTokens(10);
                cache->addFileCache(filename, originalTokens);
                removeTempFile(filename);  // 删除文件使缓存失效
                EXPECT_FALSE(cache->getFileCache(filename, tokens));
            }

            // 配置测试
            TEST_F(PreprocessorCacheTest, Configuration) {
                CacheConfig config;
                config.useCompression = false;
                config.useBinaryFormat = false;
                cache->setConfig(config);

                auto tokens = generateTokens(100);
                std::string filename = createTempFile("config test");

                EXPECT_TRUE(cache->addFileCache(filename, tokens));

                std::vector<Token> cachedTokens;
                EXPECT_TRUE(cache->getFileCache(filename, cachedTokens));

                removeTempFile(filename);
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
