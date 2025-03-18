#include "CacheStrategy.h"

#include <algorithm>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>
#include <iostream>

namespace rp {
    namespace frontend {

        // LRU策略实现
        LRUStrategy::LRUStrategy(const CacheStrategyConfig& config) : config(config), currentSize(0) {
            if (config.persistToFile && !config.persistPath.empty()) {
                loadFromFile(config.persistPath);
            }
        }

        LRUStrategy::~LRUStrategy() {
            if (config.persistToFile && !config.persistPath.empty()) {
                saveToFile(config.persistPath);
            }
        }

        bool LRUStrategy::add(const std::string& key, std::shared_ptr<CacheEntryBase> entry) {
            if (!entry) return false;

            size_t entrySize = entry->getSize();
            if (entrySize > config.maxSize) return false;

            // 如果已存在，先移除
            auto it = lookup.find(key);
            if (it != lookup.end()) {
                currentSize -= it->second->value->getSize();
                entries.erase(it->second);
                lookup.erase(it);
            }

            // 确保有足够空间
            evict(entrySize);

            // 添加新项
            entries.push_front({key, entry, std::chrono::steady_clock::now()});
            lookup[key] = entries.begin();
            currentSize += entrySize;

            updateStats(false);
            return true;
        }

        std::shared_ptr<CacheEntryBase> LRUStrategy::get(const std::string& key) {
            auto it = lookup.find(key);
            if (it == lookup.end()) {
                updateStats(false);
                return nullptr;
            }

            // 移动到队首
            entries.splice(entries.begin(), entries, it->second);
            it->second->lastAccess = std::chrono::steady_clock::now();

            updateStats(true);
            return it->second->value;
        }

        void LRUStrategy::remove(const std::string& key) {
            auto it = lookup.find(key);
            if (it != lookup.end()) {
                currentSize -= it->second->value->getSize();
                entries.erase(it->second);
                lookup.erase(it);
            }
        }

        void LRUStrategy::clear() {
            entries.clear();
            lookup.clear();
            currentSize = 0;
            stats = Stats();
        }

        bool LRUStrategy::contains(const std::string& key) const { return lookup.find(key) != lookup.end(); }

        size_t LRUStrategy::size() const { return currentSize; }

        size_t LRUStrategy::count() const { return entries.size(); }

        void LRUStrategy::evict(size_t requiredSpace) {
            while (!entries.empty() &&
                   (currentSize + requiredSpace > config.maxSize || entries.size() >= config.maxEntries)) {
                const auto& entry = entries.back();
                currentSize -= entry.value->getSize();
                lookup.erase(entry.key);
                entries.pop_back();
                stats.evictions++;
            }
        }

        void LRUStrategy::updateStats(bool hit) {
            if (hit) {
                stats.hits++;
            } else {
                stats.misses++;
            }
            stats.totalSize = currentSize;
            stats.entryCount = entries.size();
        }

        bool LRUStrategy::saveToFile(const std::string& filepath) {
            try {
                std::ofstream file(filepath, std::ios::binary);
                cereal::BinaryOutputArchive archive(file);

                // 保存基本信息
                size_t entryCount = entries.size();
                archive(entryCount);

                // 保存每个缓存项
                for (const auto& entry : entries) {
                    archive(entry.key);
                    // TODO: 实现缓存项的序列化
                }

                return true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to save cache: " << e.what() << std::endl;
                return false;
            }
        }

        bool LRUStrategy::loadFromFile(const std::string& filepath) {
            try {
                std::ifstream file(filepath, std::ios::binary);
                if (!file) return false;

                cereal::BinaryInputArchive archive(file);

                // 加载基本信息
                size_t entryCount;
                archive(entryCount);

                // 加载每个缓存项
                for (size_t i = 0; i < entryCount; ++i) {
                    std::string key;
                    archive(key);
                    // TODO: 实现缓存项的反序列化
                }

                return true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to load cache: " << e.what() << std::endl;
                return false;
            }
        }

        ICacheStrategy::Stats LRUStrategy::getStats() const { return stats; }

        // TwoQueue策略实现
        TwoQueueStrategy::TwoQueueStrategy(const CacheStrategyConfig& config) : config(config), currentSize(0) {
            if (config.persistToFile && !config.persistPath.empty()) {
                loadFromFile(config.persistPath);
            }
        }

        TwoQueueStrategy::~TwoQueueStrategy() {
            if (config.persistToFile && !config.persistPath.empty()) {
                saveToFile(config.persistPath);
            }
        }

        bool TwoQueueStrategy::add(const std::string& key, std::shared_ptr<CacheEntryBase> entry) {
            if (!entry) return false;

            size_t entrySize = entry->getSize();
            if (entrySize > config.maxSize) return false;

            // 如果已在任何队列中，先移除
            remove(key);

            // 确保有足够空间
            evict(entrySize);

            // 添加到A1in队列
            a1in.push_front({key, entry, std::chrono::steady_clock::now()});
            a1inMap[key] = a1in.begin();
            currentSize += entrySize;

            updateStats(false);
            return true;
        }

        std::shared_ptr<CacheEntryBase> TwoQueueStrategy::get(const std::string& key) {
            // 检查Am队列
            auto amIt = amMap.find(key);
            if (amIt != amMap.end()) {
                am.splice(am.begin(), am, amIt->second);
                amIt->second->timestamp = std::chrono::steady_clock::now();
                updateStats(true);
                return amIt->second->value;
            }

            // 检查A1in队列
            auto a1inIt = a1inMap.find(key);
            if (a1inIt != a1inMap.end()) {
                // 移动到Am队列
                moveToAm(key);
                updateStats(true);
                return a1inIt->second->value;
            }

            // 检查A1out队列
            auto a1outIt = a1outMap.find(key);
            if (a1outIt != a1outMap.end()) {
                // 移动到Am队列
                std::shared_ptr<CacheEntryBase> value = a1outIt->second->value;
                moveToAm(key);
                updateStats(true);
                return value;
            }

            updateStats(false);
            return nullptr;
        }

        void TwoQueueStrategy::moveToAm(const std::string& key) {
            auto a1inIt = a1inMap.find(key);
            if (a1inIt != a1inMap.end()) {
                am.push_front({key, a1inIt->second->value, std::chrono::steady_clock::now()});
                amMap[key] = am.begin();

                currentSize -= a1inIt->second->value->getSize();
                a1in.erase(a1inIt->second);
                a1inMap.erase(a1inIt);

                currentSize += am.front().value->getSize();
                return;
            }

            auto a1outIt = a1outMap.find(key);
            if (a1outIt != a1outMap.end()) {
                am.push_front({key, a1outIt->second->value, std::chrono::steady_clock::now()});
                amMap[key] = am.begin();

                a1out.erase(a1outIt->second);
                a1outMap.erase(a1outIt);
            }
        }

        void TwoQueueStrategy::remove(const std::string& key) {
            // 从Am队列移除
            auto amIt = amMap.find(key);
            if (amIt != amMap.end()) {
                currentSize -= amIt->second->value->getSize();
                am.erase(amIt->second);
                amMap.erase(amIt);
                return;
            }

            // 从A1in队列移除
            auto a1inIt = a1inMap.find(key);
            if (a1inIt != a1inMap.end()) {
                currentSize -= a1inIt->second->value->getSize();
                a1in.erase(a1inIt->second);
                a1inMap.erase(a1inIt);
                return;
            }

            // 从A1out队列移除
            auto a1outIt = a1outMap.find(key);
            if (a1outIt != a1outMap.end()) {
                a1out.erase(a1outIt->second);
                a1outMap.erase(a1outIt);
            }
        }

        void TwoQueueStrategy::clear() {
            am.clear();
            a1in.clear();
            a1out.clear();
            amMap.clear();
            a1inMap.clear();
            a1outMap.clear();
            currentSize = 0;
            stats = Stats();
        }

        bool TwoQueueStrategy::contains(const std::string& key) const {
            return amMap.find(key) != amMap.end() || a1inMap.find(key) != a1inMap.end() ||
                   a1outMap.find(key) != a1outMap.end();
        }

        size_t TwoQueueStrategy::size() const { return currentSize; }

        size_t TwoQueueStrategy::count() const { return am.size() + a1in.size() + a1out.size(); }

        void TwoQueueStrategy::evict(size_t requiredSpace) {
            // 首先从A1in队列淘汰
            while (!a1in.empty() && (currentSize + requiredSpace > config.maxSize || count() >= config.maxEntries)) {
                const auto& entry = a1in.back();

                // 移动到A1out队列
                a1out.push_front({entry.key, entry.value, entry.timestamp});
                a1outMap[entry.key] = a1out.begin();

                currentSize -= entry.value->getSize();
                a1in.pop_back();
                a1inMap.erase(entry.key);

                stats.evictions++;
            }

            // 如果还需要空间，从Am队列淘汰
            while (!am.empty() && (currentSize + requiredSpace > config.maxSize || count() >= config.maxEntries)) {
                const auto& entry = am.back();
                currentSize -= entry.value->getSize();
                amMap.erase(entry.key);
                am.pop_back();

                stats.evictions++;
            }

            // 控制A1out队列大小
            while (a1out.size() > config.maxEntries / 2) {
                const auto& entry = a1out.back();
                a1outMap.erase(entry.key);
                a1out.pop_back();
            }
        }

        void TwoQueueStrategy::updateStats(bool hit) {
            if (hit) {
                stats.hits++;
            } else {
                stats.misses++;
            }
            stats.totalSize = currentSize;
            stats.entryCount = count();
        }

        bool TwoQueueStrategy::saveToFile(const std::string& filepath) {
            try {
                std::ofstream file(filepath, std::ios::binary);
                cereal::BinaryOutputArchive archive(file);

                // 保存基本信息
                size_t amCount = am.size();
                size_t a1inCount = a1in.size();
                size_t a1outCount = a1out.size();
                archive(amCount, a1inCount, a1outCount);

                // 保存队列内容
                // TODO: 实现队列内容的序列化

                return true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to save cache: " << e.what() << std::endl;
                return false;
            }
        }

        bool TwoQueueStrategy::loadFromFile(const std::string& filepath) {
            try {
                std::ifstream file(filepath, std::ios::binary);
                if (!file) return false;

                cereal::BinaryInputArchive archive(file);

                // 加载基本信息
                size_t amCount, a1inCount, a1outCount;
                archive(amCount, a1inCount, a1outCount);

                // 加载队列内容
                // TODO: 实现队列内容的反序列化

                return true;
            } catch (const std::exception& e) {
                std::cerr << "Failed to load cache: " << e.what() << std::endl;
                return false;
            }
        }

        ICacheStrategy::Stats TwoQueueStrategy::getStats() const { return stats; }

    }  // namespace frontend
}  // namespace rp
