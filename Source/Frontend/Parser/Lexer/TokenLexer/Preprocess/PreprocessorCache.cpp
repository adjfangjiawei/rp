#include "PreprocessorCache.h"

#include <zlib.h>

#include <algorithm>
#include <filesystem>

namespace rp {
    namespace frontend {

        PreprocessorCache::PreprocessorCache(const CacheConfig& config)
            : config(config), cacheHits(0), cacheMisses(0), totalCacheSize(0) {}

        PreprocessorCache::~PreprocessorCache() = default;

        void PreprocessorCache::setConfig(const CacheConfig& newConfig) { config = newConfig; }

        const CacheConfig& PreprocessorCache::getConfig() const { return config; }

        bool PreprocessorCache::hasFileCache(const std::string& filename) const {
            if (auto it = fileCache.find(filename); it != fileCache.end()) {
                if (isCacheValid(it->second)) {
                    ++cacheHits;
                    return true;
                }
                ++cacheMisses;
                return false;
            }
            ++cacheMisses;
            return false;
        }

        bool PreprocessorCache::getFileCache(const std::string& filename, std::vector<Token>& tokens) const {
            auto it = fileCache.find(filename);
            if (it != fileCache.end() && isCacheValid(it->second)) {
                const auto& entry = it->second;
                std::vector<uint8_t> data;

                if (entry.isCompressed) {
                    data = decompressData(entry.serializedTokens, entry.originalSize);
                } else {
                    data = entry.serializedTokens;
                }

                if (deserializeTokens(data, tokens)) {
                    ++cacheHits;
                    return true;
                }
            }
            ++cacheMisses;
            return false;
        }

        bool PreprocessorCache::addFileCache(const std::string& filename,
                                             const std::vector<Token>& tokens,
                                             const std::vector<std::string>& dependencies) {
            // 序列化tokens
            std::vector<uint8_t> serialized = serializeTokens(tokens);
            size_t originalSize = serialized.size();

            // 检查是否应该缓存
            if (!shouldCache(filename, originalSize)) {
                return false;
            }

            // 压缩数据
            std::vector<uint8_t> data;
            bool isCompressed = false;
            if (config.useCompression && originalSize > 1024) {  // 只压缩大于1KB的数据
                data = compressData(serialized);
                isCompressed = true;
            } else {
                data = std::move(serialized);
            }

            // 确保有足够的缓存空间
            updateCacheSize(data.size());

            // 创建缓存项
            FileCacheEntry entry;
            entry.serializedTokens = std::move(data);
            entry.lastModified = std::filesystem::last_write_time(filename);
            entry.dependencies = dependencies;
            entry.originalSize = originalSize;
            entry.isCompressed = isCompressed;

            fileCache[filename] = std::move(entry);
            return true;
        }

        void PreprocessorCache::invalidateFileCache(const std::string& filename) {
            if (auto it = fileCache.find(filename); it != fileCache.end()) {
                totalCacheSize -= it->second.serializedTokens.size();
                fileCache.erase(filename);
            }
        }

        void PreprocessorCache::clearFileCache() {
            for (const auto& entry : fileCache) {
                totalCacheSize -= entry.second.serializedTokens.size();
            }
            fileCache.clear();
        }

        bool PreprocessorCache::hasMacroCache(const std::string& macroName, const std::vector<Token>& args) const {
            std::string key = createMacroCacheKey(macroName, args);
            auto it = macroCache.find(macroName);
            if (it != macroCache.end()) {
                if (it->second.find(key) != it->second.end()) {
                    ++cacheHits;
                    return true;
                }
            }
            ++cacheMisses;
            return false;
        }

        bool PreprocessorCache::getMacroCache(const std::string& macroName,
                                              std::vector<Token>& tokens,
                                              const std::vector<Token>& args) const {
            std::string key = createMacroCacheKey(macroName, args);
            auto it = macroCache.find(macroName);
            if (it != macroCache.end()) {
                auto entryIt = it->second.find(key);
                if (entryIt != it->second.end()) {
                    const auto& entry = entryIt->second;
                    std::vector<uint8_t> data;

                    if (entry.isCompressed) {
                        data = decompressData(entry.serializedTokens, entry.originalSize);
                    } else {
                        data = entry.serializedTokens;
                    }

                    if (deserializeTokens(data, tokens)) {
                        ++cacheHits;
                        return true;
                    }
                }
            }
            ++cacheMisses;
            return false;
        }

        bool PreprocessorCache::addMacroCache(const std::string& macroName,
                                              const std::vector<Token>& tokens,
                                              bool isFunction,
                                              const std::vector<Token>& args) {
            // 序列化tokens
            std::vector<uint8_t> serialized = serializeTokens(tokens);
            size_t originalSize = serialized.size();

            // 压缩数据
            std::vector<uint8_t> data;
            bool isCompressed = false;
            if (config.useCompression && originalSize > 1024) {
                data = compressData(serialized);
                isCompressed = true;
            } else {
                data = std::move(serialized);
            }

            // 确保有足够的缓存空间
            updateCacheSize(data.size());

            // 创建缓存项
            MacroCacheEntry entry;
            entry.serializedTokens = std::move(data);
            entry.isFunction = isFunction;
            entry.originalSize = originalSize;
            entry.isCompressed = isCompressed;

            std::string key = createMacroCacheKey(macroName, args);
            macroCache[macroName][key] = std::move(entry);
            return true;
        }

        void PreprocessorCache::invalidateMacroCache(const std::string& macroName) {
            if (auto it = macroCache.find(macroName); it != macroCache.end()) {
                for (const auto& entry : it->second) {
                    totalCacheSize -= entry.second.serializedTokens.size();
                }
                macroCache.erase(it);
            }
        }

        void PreprocessorCache::clearMacroCache() {
            for (const auto& macro : macroCache) {
                for (const auto& entry : macro.second) {
                    totalCacheSize -= entry.second.serializedTokens.size();
                }
            }
            macroCache.clear();
        }

        PreprocessorCache::CacheStats PreprocessorCache::getStatistics() const {
            CacheStats stats;
            stats.totalFilesCached = fileCache.size();

            stats.totalMacrosCached = 0;
            for (const auto& macro : macroCache) {
                stats.totalMacrosCached += macro.second.size();
            }

            stats.cacheHits = cacheHits;
            stats.cacheMisses = cacheMisses;
            stats.totalCacheSize = totalCacheSize;

            // 计算压缩比
            size_t totalOriginalSize = 0;
            size_t totalCompressedSize = 0;

            for (const auto& file : fileCache) {
                totalOriginalSize += file.second.originalSize;
                totalCompressedSize += file.second.serializedTokens.size();
            }

            for (const auto& macro : macroCache) {
                for (const auto& entry : macro.second) {
                    totalOriginalSize += entry.second.originalSize;
                    totalCompressedSize += entry.second.serializedTokens.size();
                }
            }

            stats.compressionRatio = totalCompressedSize > 0 ? (totalOriginalSize * 100) / totalCompressedSize : 100;

            return stats;
        }

        std::vector<uint8_t> PreprocessorCache::compressData(const std::vector<uint8_t>& data) const {
            if (data.empty()) {
                return {};
            }

            std::vector<uint8_t> compressedData;
            compressedData.resize(data.size() + 128);  // 预留一些空间

            z_stream stream = {};
            stream.next_in = const_cast<Bytef*>(data.data());
            stream.avail_in = static_cast<uInt>(data.size());
            stream.next_out = compressedData.data();
            stream.avail_out = static_cast<uInt>(compressedData.size());

            deflateInit(&stream, config.compressionLevel);
            deflate(&stream, Z_FINISH);
            deflateEnd(&stream);

            compressedData.resize(stream.total_out);
            return compressedData;
        }

        std::vector<uint8_t> PreprocessorCache::decompressData(const std::vector<uint8_t>& compressedData,
                                                               size_t originalSize) const {
            if (compressedData.empty()) {
                return {};
            }

            std::vector<uint8_t> decompressedData;
            decompressedData.resize(originalSize);

            z_stream stream = {};
            stream.next_in = const_cast<Bytef*>(compressedData.data());
            stream.avail_in = static_cast<uInt>(compressedData.size());
            stream.next_out = decompressedData.data();
            stream.avail_out = static_cast<uInt>(decompressedData.size());

            inflateInit(&stream);
            inflate(&stream, Z_FINISH);
            inflateEnd(&stream);

            return decompressedData;
        }

        std::string PreprocessorCache::createMacroCacheKey(const std::string& macroName,
                                                           const std::vector<Token>& args) const {
            std::stringstream ss;
            ss << macroName;

            if (!args.empty()) {
                ss << "(";
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) {
                        ss << ",";
                    }
                    ss << args[i].text;
                }
                ss << ")";
            }

            return ss.str();
        }

        bool PreprocessorCache::isCacheValid(const FileCacheEntry& entry) const {
            try {
                std::filesystem::path path(entry.dependencies.front());
                auto lastModified = std::filesystem::last_write_time(path);
                if (lastModified != entry.lastModified) {
                    return false;
                }

                for (size_t i = 1; i < entry.dependencies.size(); ++i) {
                    std::filesystem::path depPath(entry.dependencies[i]);
                    auto depLastModified = std::filesystem::last_write_time(depPath);
                    if (depLastModified != entry.lastModified) {
                        return false;
                    }
                }
            } catch (const std::filesystem::filesystem_error&) {
                return false;
            }

            return true;
        }

        bool PreprocessorCache::shouldCache(const std::string& filename, size_t dataSize) const {
            if (dataSize > config.maxEntrySize) {
                return false;
            }

            try {
                std::filesystem::path path(filename);
                std::string ext = path.extension().string();
                static const std::unordered_set<std::string> CACHED_EXTENSIONS = {
                    ".h", ".hpp", ".hxx", ".hh", ".c", ".cpp", ".cxx", ".cc"};

                return CACHED_EXTENSIONS.find(ext) != CACHED_EXTENSIONS.end();
            } catch (const std::filesystem::filesystem_error&) {
                return false;
            }
        }

        void PreprocessorCache::updateCacheSize(size_t newEntrySize) {
            if (totalCacheSize + newEntrySize > config.maxCacheSize) {
                evictCacheEntries(newEntrySize);
            }
            totalCacheSize += newEntrySize;
        }

        void PreprocessorCache::evictCacheEntries(size_t requiredSpace) {
            // 简单的LRU策略：移除最旧的缓存项直到有足够空间
            while (totalCacheSize + requiredSpace > config.maxCacheSize &&
                   (!fileCache.empty() || !macroCache.empty())) {
                if (!fileCache.empty()) {
                    auto it = fileCache.begin();
                    totalCacheSize -= it->second.serializedTokens.size();
                    fileCache.erase(it);
                }

                if (!macroCache.empty() && totalCacheSize + requiredSpace > config.maxCacheSize) {
                    auto it = macroCache.begin();
                    for (const auto& entry : it->second) {
                        totalCacheSize -= entry.second.serializedTokens.size();
                    }
                    macroCache.erase(it);
                }
            }
        }

        std::vector<uint8_t> PreprocessorCache::serializeTokens(const std::vector<Token>& tokens) const {
            return config.useBinaryFormat ? TokenSerializer::serializeBinary(tokens)
                                          : std::vector<uint8_t>(TokenSerializer::serialize(tokens).begin(),
                                                                 TokenSerializer::serialize(tokens).end());
        }

        bool PreprocessorCache::deserializeTokens(const std::vector<uint8_t>& data, std::vector<Token>& tokens) const {
            if (config.useBinaryFormat) {
                return TokenSerializer::deserializeBinary(data, tokens);
            } else {
                std::string textData(data.begin(), data.end());
                return TokenSerializer::deserialize(textData, tokens);
            }
        }

    }  // namespace frontend
}  // namespace rp
