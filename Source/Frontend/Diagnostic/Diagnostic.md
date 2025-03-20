# Diagnostic 模块文档

## 主模块
使用#include "Frontend/Diagnostic/Diagnostic.h"就能使用模块所有内容

包含头文件:
```cpp
#include "Frontend/Diagnostic/Diagnostic.h"
```

命名空间:
```cpp
rp::frontend
```

## 枚举类型

### DiagnosticLevel
```cpp
enum class DiagnosticLevel {
    Note,                          // 提示信息
    Warning,                       // 警告
    Error,                         // 错误
    Fatal                         // 致命错误
};
```

## 基础结构

### SourceLocation 结构体
```cpp
struct SourceLocation {
    std::string filename;          // 文件名
    unsigned line;                 // 行号
    unsigned column;               // 列号
};
```

## 诊断消息类

### DiagnosticMessage
```cpp
class DiagnosticMessage
```

构造函数:
```cpp
DiagnosticMessage(DiagnosticLevel level,
                 const SourceLocation &loc,
                 const std::string &msg)
```

公共方法:
```cpp
DiagnosticLevel getLevel() const                      // 获取诊断级别
const SourceLocation& getLocation() const             // 获取源码位置
const std::string& getMessage() const                 // 获取诊断消息
const std::vector<std::string>& getFixes() const      // 获取修复建议
void addFix(const std::string& fix)                   // 添加修复建议
std::string format() const                            // 格式化诊断信息
```

## 诊断引擎类

### DiagnosticEngine
```cpp
class DiagnosticEngine
```

构造函数:
```cpp
DiagnosticEngine()                                    // 默认构造函数
```

公共方法:
```cpp
void report(DiagnosticLevel level,                    // 报告诊断信息
           const SourceLocation &loc,
           const std::string &message)
void addFix(const std::string& fix)                   // 添加修复建议
const std::vector<std::shared_ptr<DiagnosticMessage>>& // 获取所有诊断信息
getDiagnostics() const
bool hasErrors() const                                // 检查是否有错误
unsigned getErrorCount() const                        // 获取错误数量
unsigned getWarningCount() const                      // 获取警告数量
void clear()                                         // 清除所有诊断信息
```
