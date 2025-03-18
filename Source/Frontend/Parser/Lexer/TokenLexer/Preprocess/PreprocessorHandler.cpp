#include "PreprocessorHandler.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace rp {
    namespace frontend {

        PreprocessorHandler::PreprocessorHandler(DiagnosticEngine* diagEngine)
            : diagnostics(diagEngine),
              cache(std::make_unique<PreprocessorCache>(CacheConfig{})),
              macroExpander(std::make_unique<MacroExpander>(diagEngine)),
              constantEvaluator(std::make_unique<ConstantEvaluator>(diagEngine)) {
            // 预定义宏
            Token cppToken(TokenKind::NumberLiteral);
            cppToken.setText(std::string_view("202002"));
            std::vector<Token> tokens;
            tokens.push_back(std::move(cppToken));
            defineMacro("__cplusplus", std::move(tokens));
            defineMacro("__FILE__", {});  // 动态替换
            defineMacro("__LINE__", {});  // 动态替换
            defineMacro("__DATE__", {});  // 动态替换
            defineMacro("__TIME__", {});  // 动态替换
        }

        void PreprocessorHandler::setConfig(const Config& config) { this->config = config; }

        const PreprocessorHandler::Config& PreprocessorHandler::getConfig() const { return config; }

        bool PreprocessorHandler::tryGetFromCache(const std::string& filename, std::vector<Token>& tokens) {
            if (!config.enableCache) {
                return false;
            }

            // 检查文件缓存
            std::vector<Token> cachedTokens;
            if (cache->getFileCache(filename, cachedTokens)) {
                tokens = std::move(cachedTokens);
                if (config.verboseOutput) {
                    reportWarning("Cache hit for file: " + filename, Token(TokenKind::Invalid));
                }
                return true;
            }

            // 读取文件内容
            std::ifstream file(filename);
            if (!file) {
                reportError("Cannot open file: " + filename, Token(TokenKind::Invalid));
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            // 处理文件内容
            // TODO: 实现文件内容到tokens的转换

            // 添加到缓存
            if (config.enableCache) {
                std::vector<Token> parsedTokens;
                // TODO: 将content解析为tokens
                // 这里应该调用词法分析器将content解析为tokens

                std::vector<std::string> deps = {filename};
                cache->addFileCache(filename, parsedTokens, deps);
            }

            return true;
        }

        void PreprocessorHandler::addToCache(const std::string& filename, const std::vector<Token>& tokens) {
            if (!config.enableCache) {
                return;
            }

            std::vector<std::string> deps = {filename};
            cache->addFileCache(filename, tokens, deps);
        }

        void PreprocessorHandler::updateCacheDependencies(const std::string& filename,
                                                          const std::vector<std::string>& deps) {
            if (!config.enableCache || !config.enableIncrementalCache) {
                return;
            }

            std::vector<Token> tokens;
            if (cache->getFileCache(filename, tokens)) {
                cache->addFileCache(filename, tokens, deps);
            }
        }

        void PreprocessorHandler::getCacheStatistics(size_t& totalFilesCached,
                                                     size_t& totalMacrosCached,
                                                     size_t& cacheHits,
                                                     size_t& cacheMisses) const {
            auto stats = cache->getStatistics();
            totalFilesCached = stats.totalFilesCached;
            totalMacrosCached = stats.totalMacrosCached;
            cacheHits = stats.cacheHits;
            cacheMisses = stats.cacheMisses;
        }

        void PreprocessorHandler::clearCache() {
            cache->clearFileCache();
            cache->clearMacroCache();
        }

        std::vector<Token> PreprocessorHandler::expandMacro(const std::string& name, const std::vector<Token>& args) {
            // 检查宏缓存
            if (config.enableCache) {
                std::vector<Token> cachedTokens;
                if (cache->getMacroCache(name, cachedTokens, args)) {
                    // 使用缓存的展开结果
                    if (config.verboseOutput) {
                        reportWarning("Macro cache hit: " + name, Token());
                    }
                    return cachedTokens;
                }
            }

            // 展开宏
            auto it = macros.find(name);
            if (it == macros.end()) {
                return {};
            }

            const Macro& macro = it->second;
            std::vector<std::vector<Token>> processedArgs;

            if (macro.isFunctionLike) {
                if (args.size() != macro.params.size() && !macro.isVariadic) {
                    Token errorToken =
                        args.empty() ? Token(TokenKind::Invalid) : Token(args[0].kind, args[0].line, args[0].column);

                    reportError("Macro '" + name + "' requires " + std::to_string(macro.params.size()) +
                                    " arguments, but " + std::to_string(args.size()) + " given",
                                errorToken);
                    return {};
                }

                // 处理参数
                processedArgs.reserve(args.size());
                for (const auto& arg : args) {
                    std::vector<Token> expandedArg;
                    expandedArg.push_back(arg);
                    // 递归展开参数中的宏
                    // TODO: 处理参数中的宏展开
                    processedArgs.push_back(std::move(expandedArg));
                }
            }

            // 展开宏
            std::vector<Token> expanded = expandMacroInternal(macro, processedArgs);

            // 添加到缓存
            if (config.enableCache) {
                cache->addMacroCache(name, expanded, macro.isFunctionLike, args);
            }

            return expanded;
        }

        std::vector<Token> PreprocessorHandler::expandMacroInternal(const Macro& macro,
                                                                    const std::vector<std::vector<Token>>& args) {
            // 创建参数映射
            std::unordered_map<std::string, std::vector<Token>> paramMap;
            for (size_t i = 0; i < macro.params.size() && i < args.size(); ++i) {
                paramMap[macro.params[i]] = args[i];
            }

            // 处理可变参数
            if (macro.isVariadic && args.size() >= macro.params.size()) {
                auto variadicArgs = std::move(*macroExpander)
                                        .handleVariadicArgs(args,
                                                            macro.params.size() - 1);  // 减去__VA_ARGS__参数
                paramMap["__VA_ARGS__"] = std::move(variadicArgs);
            }

            // 展开宏体
            auto result = std::move(*macroExpander).expand(macro.replacement, paramMap);
            return result;
        }

        bool PreprocessorHandler::evaluateConstantExpression(const std::vector<Token>& tokens) {
            int64_t result;
            if (!constantEvaluator->evaluate(tokens, result)) {
                return false;
            }
            return result != 0;
        }

        void PreprocessorHandler::reportError(const std::string& message, const Token& token) {
            SourceLocation loc{token.filename, token.line, token.column};
            diagnostics->report(DiagnosticLevel::Error, loc, message);
        }

        void PreprocessorHandler::reportWarning(const std::string& message, const Token& token) {
            SourceLocation loc{token.filename, token.line, token.column};
            diagnostics->report(DiagnosticLevel::Warning, loc, message);
        }

        bool PreprocessorHandler::isPreprocessorDirective(const Token& token) const {
            // 检查token是否以'#'开始且在行首
            return token.kind == TokenKind::Hash && token.column == 1;
        }

        Token PreprocessorHandler::handlePreprocessorDirective() {
            // TODO: 实现预处理指令的处理逻辑
            // 这里应该读取并处理整个预处理指令
            // 返回处理后的token或者继续获取下一个token
            return Token(TokenKind::Invalid);
        }

    }  // namespace frontend
}  // namespace rp
