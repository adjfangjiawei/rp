# 词法分析器示例程序

本目录包含了一系列用于测试和展示词法分析器功能的示例程序。这些示例涵盖了词法分析器的所有主要功能，从基本的token识别到错误处理和性能测试。

## 目录结构

```
Example/Frontend/Lexer/
├── CMakeLists.txt                  # 构建配置文件
├── lexer_example.cpp              # 基本词法分析示例
├── number_literal_example.cpp     # 数字字面量测试
├── string_literal_example.cpp     # 字符串字面量测试
├── identifier_keyword_example.cpp # 标识符和关键字测试
├── error_recovery_example.cpp     # 错误处理和恢复测试
├── comprehensive_example.cpp      # 综合功能测试
└── README.md                      # 本文档
```

## 构建说明

所有示例程序都使用CMake构建系统。构建步骤如下：

```bash
# 在项目根目录下创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .
```

构建完成后，可执行文件将位于 `build/example/bin` 目录下。

## 示例程序说明

### 1. 基本词法分析示例 (lexer_example)
- 演示基本的词法分析功能
- 包含token获取、预览和回退功能
- 展示位置信息和错误上下文获取

### 2. 数字字面量示例 (number_literal_example)
- 测试各种数字字面量的识别
- 包括整数、浮点数、不同进制的数字
- 测试数字字面量的后缀处理

### 3. 字符串字面量示例 (string_literal_example)
- 测试字符串和字符字面量
- 包含转义序列处理
- 支持Unicode字符串
- 测试原始字符串(Raw string)

### 4. 标识符和关键字示例 (identifier_keyword_example)
- 测试标识符的识别
- 验证所有C++关键字
- 包含Unicode标识符支持
- 测试特殊标识符规则

### 5. 错误处理示例 (error_recovery_example)
- 测试各种词法错误的处理
- 验证错误恢复机制
- 展示错误报告功能
- 测试多重错误的处理

### 6. 综合测试示例 (comprehensive_example)
- 测试所有运算符和标点符号
- 验证复杂的嵌套结构
- 包含注释处理测试
- 提供性能测试功能

## 使用方法

每个示例程序都可以独立运行：

```bash
# 运行基本词法分析示例
./example/bin/lexer_example

# 运行数字字面量测试
./example/bin/number_literal_example

# 运行字符串字面量测试
./example/bin/string_literal_example

# 运行标识符和关键字测试
./example/bin/identifier_keyword_example

# 运行错误处理测试
./example/bin/error_recovery_example

# 运行综合测试
./example/bin/comprehensive_example
```

## 注意事项

1. 所有示例程序都会输出详细的测试信息，包括：
   - Token的类型和内容
   - 位置信息（行号和列号）
   - 错误信息（如果有）
   - 上下文信息

2. 性能测试（在comprehensive_example中）会处理大量代码，可能需要一些时间完成。

3. 错误处理测试会产生预期的错误信息，这些是测试的一部分，不表示程序出现问题。

## 扩展和修改

这些示例程序可以作为模板来创建新的测试用例。要添加新的测试，只需：

1. 创建新的源文件
2. 在CMakeLists.txt中添加新的可执行目标
3. 链接必要的库
4. 设置输出目录

## 调试建议

1. 使用详细输出模式来查看更多信息
2. 检查错误上下文来定位问题
3. 使用小型测试用例来隔离特定问题
4. 注意观察位置信息的正确性
