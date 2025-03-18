#pragma once

#include <chrono>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rp {
    namespace frontend {

        // 缓存项基类
        struct CacheEntryBase {
            virtual ~CacheEntryBase() = default;
            virtual size_t getSize() const = 0;
        };

        // 缓存策略类型
        enum class CacheStrategyType {
            LRU,      // 最近最少使用
            LFU,      // 最不经常使用
            ARC,      // 自适应替换缓存
            TwoQueue  // 双队列
        };

        // 缓存策略配置
        struct CacheStrategyConfig {
            CacheStrategyType type = CacheStrategyType::LRU;
            size_t maxSize = 100 * 1024 * 1024;  // 最大缓存大小
            size_t maxEntries = 10000;           // 最大缓存项数
            bool persistToFile = false;          // 是否持久化到文件
            std::string persistPath = "";        // 持久化文件路径
        };

        // 缓存策略接口
        class ICacheStrategy {
          public:
            virtual ~ICacheStrategy() = default;

            // 基本操作
            virtual bool add(const std::string& key, std::shared_ptr<CacheEntryBase> entry) = 0;
            virtual std::shared_ptr<CacheEntryBase> get(const std::string& key) = 0;
            virtual void remove(const std::string& key) = 0;
            virtual void clear() = 0;

            // 状态查询
            virtual bool contains(const std::string& key) const = 0;
            virtual size_t size() const = 0;
            virtual size_t count() const = 0;

            // 持久化
            virtual bool saveToFile(const std::string& filepath) = 0;
            virtual bool loadFromFile(const std::string& filepath) = 0;

            // 统计信息
            struct Stats {
                size_t hits = 0;
                size_t misses = 0;
                size_t evictions = 0;
                size_t totalSize = 0;
                size_t entryCount = 0;
            };
            virtual Stats getStats() const = 0;
        };

        // LRU缓存策略实现
        class LRUStrategy : public ICacheStrategy {
          public:
            explicit LRUStrategy(const CacheStrategyConfig& config);
            ~LRUStrategy() override;

            bool add(const std::string& key, std::shared_ptr<CacheEntryBase> entry) override;
            std::shared_ptr<CacheEntryBase> get(const std::string& key) override;
            void remove(const std::string& key) override;
            void clear() override;
            bool contains(const std::string& key) const override;
            size_t size() const override;
            size_t count() const override;
            bool saveToFile(const std::string& filepath) override;
            bool loadFromFile(const std::string& filepath) override;
            Stats getStats() const override;

          private:
            struct LRUEntry {
                std::string key;
                std::shared_ptr<CacheEntryBase> value;
                std::chrono::steady_clock::time_point lastAccess;
            };

            using LRUList = std::list<LRUEntry>;
            using LRUMap = std::unordered_map<std::string, LRUList::iterator>;

            CacheStrategyConfig config;
            LRUList entries;
            LRUMap lookup;
            Stats stats;
            size_t currentSize;

            void evict(size_t requiredSpace);
            void updateStats(bool hit);
        };

        // 双队列缓存策略实现
        class TwoQueueStrategy : public ICacheStrategy {
          public:
            explicit TwoQueueStrategy(const CacheStrategyConfig& config);
            ~TwoQueueStrategy() override;

            bool add(const std::string& key, std::shared_ptr<CacheEntryBase> entry) override;
            std::shared_ptr<CacheEntryBase> get(const std::string& key) override;
            void remove(const std::string& key) override;
            void clear() override;
            bool contains(const std::string& key) const override;
            size_t size() const override;
            size_t count() const override;
            bool saveToFile(const std::string& filepath) override;
            bool loadFromFile(const std::string& filepath) override;
            Stats getStats() const override;

          private:
            struct QueueEntry {
                std::string key;
                std::shared_ptr<CacheEntryBase> value;
                std::chrono::steady_clock::time_point timestamp;
            };

            using Queue = std::list<QueueEntry>;
            using QueueMap = std::unordered_map<std::string, Queue::iterator>;

            CacheStrategyConfig config;
            Queue am;     // 主队列
            Queue a1in;   // 最近访问队列
            Queue a1out;  // 历史队列
            QueueMap amMap;
            QueueMap a1inMap;
            QueueMap a1outMap;
            Stats stats;
            size_t currentSize;

            void evict(size_t requiredSpace);
            void moveToAm(const std::string& key);
            void updateStats(bool hit);
        };

        // 缓存策略工厂
        class CacheStrategyFactory {
          public:
            static std::unique_ptr<ICacheStrategy> create(const CacheStrategyConfig& config) {
                switch (config.type) {
                    case CacheStrategyType::LRU:
                        return std::make_unique<LRUStrategy>(config);
                    case CacheStrategyType::TwoQueue:
                        return std::make_unique<TwoQueueStrategy>(config);
                    default:
                        return std::make_unique<LRUStrategy>(config);
                }
            }
        };

    }  // namespace frontend
}  // namespace rp
