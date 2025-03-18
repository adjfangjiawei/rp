#pragma once

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/ConstantEvaluator.h"
#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/MacroExpander.h"
#include "Frontend/Parser/Lexer/TokenLexer/Preprocess/PreprocessorCache.h"
#include "Frontend/Parser/Lexer/TokenLexer/Serializer/TokenSerializer.h"
namespace rp {
    namespace frontend {

        class PreprocessorHandler {
          public:
            // 构造函数
            explicit PreprocessorHandler(DiagnosticEngine* diagEngine);

            // 预处理器配置
            struct Config {
                bool enableCache = true;                  // 启用缓存
                bool enableIncrementalCache = true;       // 启用增量缓存
                size_t maxCacheSize = 100 * 1024 * 1024;  // 最大缓存大小（字节）
                size_t maxMacroExpansionDepth = 1024;     // 最大宏展开深度
                bool verboseOutput = false;               // 详细输出模式
            };

            // 设置配置
            void setConfig(const Config& config);
            const Config& getConfig() const;

            // 处理预处理指令
            bool handleDirective(const std::string& directive, const std::vector<Token>& tokens);

            // 检查是否在条件编译块内
            bool isInActiveBlock() const;

            // 宏定义相关
            bool defineMacro(const std::string& name, const std::vector<Token>& replacement);
            bool undefMacro(const std::string& name);
            bool isMacroDefined(const std::string& name) const;
            std::vector<Token> expandMacro(const std::string& name, const std::vector<Token>& args = {});

            // 文件包含处理
            std::string resolveInclude(const std::string& filename, bool isSystemHeader);
            void addIncludePath(const std::string& path);

            // 条件编译
            void pushCondition(bool condition);
            void popCondition();
            bool evaluateCondition(const std::vector<Token>& tokens);

            // 缓存统计
            void getCacheStatistics(size_t& totalFilesCached,
                                    size_t& totalMacrosCached,
                                    size_t& cacheHits,
                                    size_t& cacheMisses) const;
            void clearCache();

            // Token处理方法
            bool isPreprocessorDirective(const Token& token) const;
            Token handlePreprocessorDirective();

          private:
            // 宏定义结构
            struct Macro {
                std::string name;
                std::vector<std::string> params;  // 函数式宏的参数
                std::vector<Token> replacement;   // 替换列表
                bool isFunctionLike;              // 是否是函数式宏
                bool isVariadic;                  // 是否是可变参数宏
            };

            // 条件编译状态
            struct ConditionState {
                bool isActive;  // 当前分支是否激活
                bool hasElse;   // 是否已经遇到#else
                bool wasTrue;   // 是否有任何分支为真
            };

            // 成员变量
            DiagnosticEngine* diagnostics;
            std::unordered_map<std::string, Macro> macros;
            std::vector<std::string> includePaths;
            std::stack<ConditionState> conditionStack;
            std::vector<std::string> includeStack;  // 用于检测循环包含

            // 新增：配置和组件
            Config config;
            std::unique_ptr<PreprocessorCache> cache;
            std::unique_ptr<MacroExpander> macroExpander;
            std::unique_ptr<ConstantEvaluator> constantEvaluator;

            // 辅助函数
            bool handleInclude(const std::vector<Token>& tokens);
            bool handleDefine(const std::vector<Token>& tokens);
            bool handleUndef(const std::vector<Token>& tokens);
            bool handleIfdef(const std::vector<Token>& tokens);
            bool handleIfndef(const std::vector<Token>& tokens);
            bool handleIf(const std::vector<Token>& tokens);
            bool handleElif(const std::vector<Token>& tokens);
            bool handleElse(const std::vector<Token>& tokens);
            bool handleEndif(const std::vector<Token>& tokens);
            bool handlePragma(const std::vector<Token>& tokens);
            bool handleError(const std::vector<Token>& tokens);
            bool handleWarning(const std::vector<Token>& tokens);

            // 宏展开辅助函数
            std::vector<Token> expandMacroInternal(const Macro& macro, const std::vector<std::vector<Token>>& args);
            bool collectMacroArguments(const std::vector<Token>& tokens,
                                       size_t& pos,
                                       std::vector<std::vector<Token>>& args);

            // 条件编译辅助函数
            bool evaluateConstantExpression(const std::vector<Token>& tokens);

            // 错误处理
            void reportError(const std::string& message, const Token& token);
            void reportWarning(const std::string& message, const Token& token);

            // 缓存相关
            bool tryGetFromCache(const std::string& filename, std::vector<Token>& tokens);
            void addToCache(const std::string& filename, const std::vector<Token>& tokens);
            void updateCacheDependencies(const std::string& filename, const std::vector<std::string>& deps);
        };

    }  // namespace frontend
}  // namespace rp
