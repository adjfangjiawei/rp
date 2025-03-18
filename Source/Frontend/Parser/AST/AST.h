#pragma once
#include <memory>
#include <string>
#include <vector>

namespace rp
{
    namespace frontend
    {
        // 基础AST节点类型
        enum class ASTNodeKind
        {
            // 声明相关
            VarDecl,
            FuncDecl,
            ClassDecl,
            TemplateDecl,
            ConstructorDecl,
            DestructorDecl,
            MethodDecl,
            FieldDecl,
            EnumDecl,
            NamespaceDecl,
            UsingDecl,

            // 表达式相关
            BinaryExpr,
            UnaryExpr,
            LiteralExpr,
            CallExpr,
            MemberExpr,
            LambdaExpr,
            ThisExpr,
            CastExpr,
            CoroutineExpr,
            NewExpr,
            DeleteExpr,
            ArraySubscriptExpr,
            ConditionalExpr, // 三目运算符

            // 语句相关
            CompoundStmt,
            IfStmt,
            WhileStmt,
            ForStmt,
            ReturnStmt,
            BreakStmt,
            ContinueStmt,
            SwitchStmt,
            CaseStmt,
            CoAwaitStmt,
            CoYieldStmt,
            TryStmt,
            CatchStmt,

            // 类型相关
            BuiltinType,
            UserDefinedType,
            TemplateType,
            ArrayType,
            PointerType,
            ReferenceType,
            FunctionType,
            TupleType,    // 用于多返回值
            VariadicType, // 用于变参模板
            QualifiedType // const/volatile限定
        };

        // AST基类
        class ASTNode
        {
        public:
            virtual ~ASTNode() = default;
            virtual ASTNodeKind getKind() const = 0;

            // 源码位置信息
            struct Location
            {
                unsigned line;
                unsigned column;
                std::string filename;
            };

            Location location;

            // 添加属性系统支持
            std::vector<std::shared_ptr<class Attribute>> attributes;
        };

        // 声明节点基类
        class Decl : public ASTNode
        {
        public:
            std::string name;
            bool isDefinition = false;
            bool isExported = false;                                  // 是否导出
            unsigned accessLevel = 0;                                 // 访问级别(public/protected/private)
            std::vector<std::shared_ptr<class Attribute>> attributes; // 声明特有的属性
        };

        // 表达式节点基类
        class Expr : public ASTNode
        {
        public:
            // 表达式类型（在语义分析阶段填充）
            std::shared_ptr<class Type> type;
            bool isLValue = false;   // 是否为左值
            bool isConstant = false; // 是否为常量表达式
        };

        // 语句节点基类
        class Stmt : public ASTNode
        {
        public:
            virtual bool isTerminator() const { return false; }
            bool hasScope = false;                            // 是否引入新的作用域
            std::vector<std::shared_ptr<class Label>> labels; // 语句标签
        };

        // 类型节点基类
        class Type : public ASTNode
        {
        public:
            bool isConst = false;
            bool isVolatile = false;
            virtual std::string toString() const = 0;
            virtual bool isComplete() const = 0; // 是否为完整类型
            virtual size_t getSize() const = 0;  // 获取类型大小
        };

        // 模板参数基类
        class TemplateParam : public ASTNode
        {
        public:
            std::string name;
            bool isVariadic = false; // 是否为变参模板参数
            bool hasDefaultValue = false;
            std::shared_ptr<ASTNode> defaultValue;
        };

        // 协程状态基类
        class CoroutineState : public ASTNode
        {
        public:
            std::shared_ptr<Type> promiseType;
            std::vector<std::shared_ptr<VarDecl>> locals; // 协程局部变量
            std::shared_ptr<Type> returnType;             // 协程返回类型
        };

        // 属性节点
        class Attribute : public ASTNode
        {
        public:
            std::string name;
            std::vector<std::shared_ptr<Expr>> args;
        };

        // 标签节点
        class Label : public ASTNode
        {
        public:
            std::string name;
            bool isCase = false;             // 是否为case标签
            std::shared_ptr<Expr> caseValue; // case值
        };

    } // namespace frontend
} // namespace rp
