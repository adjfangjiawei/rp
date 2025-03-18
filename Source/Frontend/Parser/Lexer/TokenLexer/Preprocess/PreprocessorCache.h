#pragma once

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/TokenLexer/Serializer/TokenSerializer.h"

namespace rp {
    namespace frontend {

        // 缓存配置
        struct CacheConfig {
            bool useCompression = true;               // 是否使用压缩
            bool useBinaryFormat = true;              // 是否使用二进制格式
            size_t compressionLevel = 6;              // 压缩级别(0-9)
            size_t maxCacheSize = 100 * 1024 * 1024;  // 最大缓存大小（字节）
            size_t maxEntrySize = 10 * 1024 * 1024;   // 单个缓存项最大大小
        };

        // 文件缓存项
        struct FileCacheEntry {
            std::vector<uint8_t> serializedTokens;         // 序列化（可能压缩）的tokens
            std::filesystem::file_time_type lastModified;  // 最后修改时间
            std::vector<std::string> dependencies;         // 依赖文件列表
            size_t originalSize;                           // 压缩前大小
            bool isCompressed;                             // 是否已压缩
        };

        // 宏缓存项
        struct MacroCacheEntry {
            std::vector<uint8_t> serializedTokens;  // 序列化的展开结果
            std::vector<std::string> dependencies;  // 依赖的宏列表
            bool isFunction;                        // 是否是函数式宏
            size_t originalSize;                    // 压缩前大小
            bool isCompressed;                      // 是否已压缩
        };

        class PreprocessorCache {
          public:
            explicit PreprocessorCache(const CacheConfig& config = CacheConfig());
            ~PreprocessorCache();

            // 设置配置
            void setConfig(const CacheConfig& config);
            const CacheConfig& getConfig() const;

            // 文件缓存操作
            bool hasFileCache(const std::string& filename) const;
            bool getFileCache(const std::string& filename, std::vector<Token>& tokens) const;
            bool addFileCache(const std::string& filename,
                              const std::vector<Token>& tokens,
                              const std::vector<std::string>& dependencies = {});
            void invalidateFileCache(const std::string& filename);
            void clearFileCache();

            // 宏缓存操作
            bool hasMacroCache(const std::string& macroName, const std::vector<Token>& args = {}) const;
            bool getMacroCache(const std::string& macroName,
                               std::vector<Token>& tokens,
                               const std::vector<Token>& args = {}) const;
            bool addMacroCache(const std::string& macroName,
                               const std::vector<Token>& tokens,
                               bool isFunction = false,
                               const std::vector<Token>& args = {});
            void invalidateMacroCache(const std::string& macroName);
            void clearMacroCache();

            // 缓存统计
            struct CacheStats {
                size_t totalFilesCached;
                size_t totalMacrosCached;
                size_t cacheHits;
                size_t cacheMisses;
                size_t totalCacheSize;
                size_t compressionRatio;  // 原始大小/压缩大小 * 100
            };
            CacheStats getStatistics() const;

          private:
            // 配置
            CacheConfig config;

            // 缓存存储
            std::unordered_map<std::string, FileCacheEntry> fileCache;
            std::unordered_map<std::string, std::unordered_map<std::string, MacroCacheEntry>> macroCache;

            // 缓存统计
            mutable size_t cacheHits;
            mutable size_t cacheMisses;
            size_t totalCacheSize;

            // 压缩/解压缩
            std::vector<uint8_t> compressData(const std::vector<uint8_t>& data) const;
            std::vector<uint8_t> decompressData(const std::vector<uint8_t>& compressedData, size_t originalSize) const;

            // 辅助函数
            std::string createMacroCacheKey(const std::string& macroName, const std::vector<Token>& args) const;
            bool isCacheValid(const FileCacheEntry& entry) const;
            bool shouldCache(const std::string& filename, size_t dataSize) const;
            void updateCacheSize(size_t newEntrySize);
            void evictCacheEntries(size_t requiredSpace);

            // 序列化辅助函数
            std::vector<uint8_t> serializeTokens(const std::vector<Token>& tokens) const;
            bool deserializeTokens(const std::vector<uint8_t>& data, std::vector<Token>& tokens) const;
        };

    }  // namespace frontend
}  // namespace rp
