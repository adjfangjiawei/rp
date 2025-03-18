# 预处理器模块

## 概述

预处理器模块提供了C++源代码的预处理功能，包括宏展开、条件编译、文件包含等功能。该模块由以下主要组件组成：

1. PreprocessorHandler：预处理器主类
2. MacroExpander：宏展开处理器
3. ConstantEvaluator：常量表达式求值器

## 功能特性

### 1. 宏处理
- 支持对象宏和函数式宏
- 支持预定义宏（__FILE__, __LINE__等）
- 支持字符串化操作符(#)
- 支持标记连接操作符(##)
- 支持可变参数宏

### 2. 条件编译
- 支持 #if, #ifdef, #ifndef
- 支持 #elif, #else, #endif
- 支持嵌套条件编译
- 支持常量表达式求值

### 3. 文件包含
- 支持 #include 指令
- 支持引号包含和尖括号包含
- 支持多级目录搜索
- 防止循环包含

### 4. 错误处理
- 详细的错误诊断
- 错误恢复机制
- 源代码位置追踪

## 使用示例

### 基本用法

```cpp
// 创建预处理器
DiagnosticEngine diagEngine;
PreprocessorHandler preprocessor(&diagEngine);

// 添加包含路径
preprocessor.addIncludePath("/usr/include");

// 定义宏
std::vector<Token> tokens = {
    Token(TokenKind::Identifier, "DEBUG"),
    Token(TokenKind::NumberLiteral, "1")
};
preprocessor.handleDefine(tokens);

// 条件编译
if (preprocessor.isInActiveBlock()) {
    // 处理当前代码块
}
```

### 宏展开示例

```cpp
// 创建宏展开器
MacroExpander expander(&diagEngine);

// 定义参数映射
std::unordered_map<std::string, std::vector<Token>> paramMap = {
    {"x", {Token(TokenKind::NumberLiteral, "42")}}
};

// 展开宏
std::vector<Token> macroTokens = {/* 宏定义tokens */};
auto expanded = expander.expand(macroTokens, paramMap);
```

### 常量表达式求值示例

```cpp
// 创建常量求值器
ConstantEvaluator evaluator(&diagEngine);

// 准备表达式tokens
std::vector<Token> expr = {
    Token(TokenKind::NumberLiteral, "1"),
    Token(TokenKind::Plus),
    Token(TokenKind::NumberLiteral, "2")
};

// 求值
int64_t result;
if (evaluator.evaluate(expr, result)) {
    std::cout << "Result: " << result << std::endl;
}
```

## 错误处理

预处理器使用DiagnosticEngine来报告错误和警告：

```cpp
void reportError(const std::string& message, const Token& token) {
    diagnostics->report(
        Diagnostic(
            DiagnosticSeverity::Error,
            message,
            token.filename,
            token.line,
            token.column
        )
    );
}
```

## 性能考虑

1. 宏展开
   - 使用缓存避免重复展开
   - 优化字符串操作
   - 减少内存分配

2. 常量表达式求值
   - 使用递归下降解析器
   - 优化运算符优先级处理
   - 避免不必要的token复制

3. 文件包含
   - 缓存已包含的文件
   - 优化文件路径查找
   - 使用内存映射文件（可选）

## 测试

项目包含完整的单元测试套件，覆盖：
- 宏定义和展开
- 条件编译
- 常量表达式求值
- 错误处理
- 边界情况

运行测试：
```bash
cd build
ctest -R TokenLexerTests
```

## 限制和注意事项

1. 宏展开
   - 不支持递归宏
   - 最大展开深度限制
   - 某些复杂的##操作可能需要特殊处理

2. 条件编译
   - 不支持某些非标准扩展
   - defined运算符限制

3. 文件包含
   - 文件路径限制
   - 包含深度限制

## 未来改进

1. 性能优化
   - 实现宏定义缓存
   - 优化文件读取
   - 改进内存管理

2. 功能增强
   - 支持更多预处理指令
   - 增加更多预定义宏
   - 改进错误恢复

3. 工具支持
   - 添加宏展开调试工具
   - 改进错误报告格式
   - 添加性能分析工具
