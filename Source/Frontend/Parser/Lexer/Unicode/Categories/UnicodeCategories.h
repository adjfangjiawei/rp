#pragma once
#include <cstdint>
#include <string_view>

namespace rp::frontend::unicode {

    // Unicode代理项范围常量
    constexpr uint32_t SURROGATE_MIN = 0xD800;
    constexpr uint32_t SURROGATE_MAX = 0xDFFF;

    class UnicodeCategories {
      public:
        enum class Category {
            Letter_Uppercase,
            Letter_Lowercase,
            Letter_Titlecase,
            Letter_Modifier,
            Letter_Other,
            Number_Decimal,
            Number_Letter,
            Number_Other,
            Punctuation_Connector,
            Punctuation_Dash,
            Punctuation_Open,
            Punctuation_Close,
            Punctuation_Quote,
            Punctuation_Other,
            Symbol_Math,
            Symbol_Currency,
            Symbol_Modifier,
            Symbol_Other,
            Separator_Space,
            Separator_Line,
            Separator_Paragraph,
            Mark_NonSpacing,
            Mark_SpacingCombining,
            Mark_Enclosing,
            Other_Control,
            Other_Format,
            Other_Surrogate,
            Other_PrivateUse,
            Other_NotAssigned
        };

        // Unicode属性检查
        static bool isIdentifierStart(uint32_t codepoint);
        static bool isIdentifierContinue(uint32_t codepoint);
        static bool isWhitespace(uint32_t codepoint);
        static bool isDigit(uint32_t codepoint);
        static bool isHexDigit(uint32_t codepoint);
        static bool isAlpha(uint32_t codepoint);
        static bool isAlnum(uint32_t codepoint);

        // Unicode字符分类
        static Category getCategory(uint32_t codepoint);
        static bool isInCategory(uint32_t codepoint, Category category);

      private:
        // Unicode范围结构体定义
        struct UnicodeRange {
            uint32_t start;
            uint32_t end;
            Category category;
        };

        static bool binarySearchUnicodeData(uint32_t codepoint, Category &category);
        static constexpr UnicodeRange unicodeRanges[] = {
            {0x0000, 0x001F, UnicodeCategories::Category::Other_Control},        // C0控制字符
            {0x0020, 0x0020, UnicodeCategories::Category::Separator_Space},      // 空格
            {0x0021, 0x002F, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
            {0x0030, 0x0039, UnicodeCategories::Category::Number_Decimal},       // ASCII数字
            {0x003A, 0x0040, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
            {0x0041, 0x005A, UnicodeCategories::Category::Letter_Uppercase},     // ASCII大写字母
            {0x005B, 0x0060, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
            {0x0061, 0x007A, UnicodeCategories::Category::Letter_Lowercase},     // ASCII小写字母
            {0x007B, 0x007E, UnicodeCategories::Category::Punctuation_Other},    // ASCII标点
            {0x007F, 0x009F, UnicodeCategories::Category::Other_Control},        // C1控制字符
            {0x00A0, 0x00A0, UnicodeCategories::Category::Separator_Space},      // 不间断空格
            {0x00A1, 0x00BF, UnicodeCategories::Category::Punctuation_Other},    // Latin-1标点
            {0x00C0, 0x00D6, UnicodeCategories::Category::Letter_Uppercase},     // Latin-1大写字母
            {0x00D7, 0x00D7, UnicodeCategories::Category::Symbol_Math},          // 乘号
            {0x00D8, 0x00DE, UnicodeCategories::Category::Letter_Uppercase},     // Latin-1大写字母
            {0x00DF, 0x00F6, UnicodeCategories::Category::Letter_Lowercase},     // Latin-1小写字母
            {0x00F7, 0x00F7, UnicodeCategories::Category::Symbol_Math},          // 除号
            {0x00F8, 0x00FF, UnicodeCategories::Category::Letter_Lowercase},     // Latin-1小写字母
            {0x0100, 0x017F, UnicodeCategories::Category::Letter_Other},         // 扩展Latin-A
            {0x0180, 0x024F, UnicodeCategories::Category::Letter_Other},         // 扩展Latin-B
            {0x0250, 0x02AF, UnicodeCategories::Category::Letter_Other},         // 国际音标扩展
            {0x02B0, 0x02FF, UnicodeCategories::Category::Letter_Modifier},      // 声调修饰符
            {0x0300, 0x036F, UnicodeCategories::Category::Mark_NonSpacing},      // 组合变音标记
            {0x0370, 0x03FF, UnicodeCategories::Category::Letter_Other},         // 希腊和科普特文
            {0x0400, 0x04FF, UnicodeCategories::Category::Letter_Other},         // 西里尔字母
            {0x0500, 0x052F, UnicodeCategories::Category::Letter_Other},         // 西里尔补充
            {0x0530, 0x058F, UnicodeCategories::Category::Letter_Other},         // 亚美尼亚文
            {0x0590, 0x05FF, UnicodeCategories::Category::Letter_Other},         // 希伯来文
            {0x0600, 0x06FF, UnicodeCategories::Category::Letter_Other},         // 阿拉伯文
            {0x0900, 0x097F, UnicodeCategories::Category::Letter_Other},         // 天城文
            {0x0980, 0x09FF, UnicodeCategories::Category::Letter_Other},         // 孟加拉文
            {0x0A00, 0x0A7F, UnicodeCategories::Category::Letter_Other},         // 古木基文
            {0x0A80, 0x0AFF, UnicodeCategories::Category::Letter_Other},         // 古吉拉特文
            {0x0B00, 0x0B7F, UnicodeCategories::Category::Letter_Other},         // 奥里亚文
            {0x0B80, 0x0BFF, UnicodeCategories::Category::Letter_Other},         // 泰米尔文
            {0x0C00, 0x0C7F, UnicodeCategories::Category::Letter_Other},         // 泰卢固文
            {0x0C80, 0x0CFF, UnicodeCategories::Category::Letter_Other},         // 卡纳达文
            {0x0D00, 0x0D7F, UnicodeCategories::Category::Letter_Other},         // 马拉雅拉姆文
            {0x0D80, 0x0DFF, UnicodeCategories::Category::Letter_Other},         // 僧伽罗文
            {0x0E00, 0x0E7F, UnicodeCategories::Category::Letter_Other},         // 泰文
            {0x0E80, 0x0EFF, UnicodeCategories::Category::Letter_Other},         // 老挝文
            {0x0F00, 0x0FFF, UnicodeCategories::Category::Letter_Other},         // 藏文
            {0x1000, 0x109F, UnicodeCategories::Category::Letter_Other},         // 缅甸文
            {0x10A0, 0x10FF, UnicodeCategories::Category::Letter_Other},         // 格鲁吉亚文
            {0x1100, 0x11FF, UnicodeCategories::Category::Letter_Other},         // 谚文字母
            {0x1680, 0x169F, UnicodeCategories::Category::Letter_Other},         // 欧甘字母
            {0x16A0, 0x16FF, UnicodeCategories::Category::Letter_Other},         // 卢恩字母
            {0x1700, 0x171F, UnicodeCategories::Category::Letter_Other},         // 他加禄字母
            {0x1720, 0x173F, UnicodeCategories::Category::Letter_Other},         // 哈努诺文
            {0x1740, 0x175F, UnicodeCategories::Category::Letter_Other},         // 布希德文
            {0x1760, 0x177F, UnicodeCategories::Category::Letter_Other},         // 塔格巴努亚文
            {0x1780, 0x17FF, UnicodeCategories::Category::Letter_Other},         // 高棉文
            {0x1800, 0x18AF, UnicodeCategories::Category::Letter_Other},         // 蒙古文
            {0x1E00, 0x1EFF, UnicodeCategories::Category::Letter_Other},         // 拉丁文扩展附加
            {0x1F00, 0x1FFF, UnicodeCategories::Category::Letter_Other},         // 希腊文扩展
            {0x2000, 0x200A, UnicodeCategories::Category::Separator_Space},      // 各种空格
            {0x2010, 0x2027, UnicodeCategories::Category::Punctuation_Dash},     // 各种破折号
            {0x2028, 0x2028, UnicodeCategories::Category::Separator_Line},       // 行分隔符
            {0x2029, 0x2029, UnicodeCategories::Category::Separator_Paragraph},  // 段落分隔符
            {0x202A, 0x202E, UnicodeCategories::Category::Other_Format},         // 双向文本控制
            {0x2030, 0x205E, UnicodeCategories::Category::Punctuation_Other},    // 常用标点
            {0x2060, 0x2064, UnicodeCategories::Category::Other_Format},         // 不可见格式控制
            {0x206A, 0x206F, UnicodeCategories::Category::Other_Format},         // 一般格式控制
            {0x2070, 0x209F, UnicodeCategories::Category::Number_Other},         // 上标和下标
            {0x20A0, 0x20CF, UnicodeCategories::Category::Symbol_Currency},      // 货币符号
            {0x20D0, 0x20FF, UnicodeCategories::Category::Mark_NonSpacing},      // 组合变音标记for符号
            {0x2100, 0x214F, UnicodeCategories::Category::Symbol_Other},         // 字母符号
            {0x2150, 0x218F, UnicodeCategories::Category::Number_Other},         // 数字形式
            {0x2190, 0x21FF, UnicodeCategories::Category::Symbol_Math},          // 箭头
            {0x2200, 0x22FF, UnicodeCategories::Category::Symbol_Math},          // 数学运算符
            {0x2300, 0x23FF, UnicodeCategories::Category::Symbol_Other},         // 各种技术符号
            {0x2400, 0x243F, UnicodeCategories::Category::Symbol_Other},         // 控制图片
            {0x2440, 0x245F, UnicodeCategories::Category::Symbol_Other},         // OCR
            {0x2460, 0x24FF, UnicodeCategories::Category::Number_Other},         // 带圈数字
            {0x2500, 0x257F, UnicodeCategories::Category::Symbol_Other},         // 制表符
            {0x2580, 0x259F, UnicodeCategories::Category::Symbol_Other},         // 方块元素
            {0x25A0, 0x25FF, UnicodeCategories::Category::Symbol_Other},         // 几何图形
            {0x2600, 0x26FF, UnicodeCategories::Category::Symbol_Other},         // 杂项符号
            {0x2700, 0x27BF, UnicodeCategories::Category::Symbol_Other},         // 装饰符号
            {0x27C0, 0x27EF, UnicodeCategories::Category::Symbol_Math},          // 杂项数学符号-A
            {0x27F0, 0x27FF, UnicodeCategories::Category::Symbol_Math},          // 补充箭头-A
            {0x2800, 0x28FF, UnicodeCategories::Category::Symbol_Other},         // 盲文图案
            {0x2900, 0x297F, UnicodeCategories::Category::Symbol_Math},          // 补充箭头-B
            {0x2980, 0x29FF, UnicodeCategories::Category::Symbol_Math},          // 杂项数学符号-B
            {0x2A00, 0x2AFF, UnicodeCategories::Category::Symbol_Math},          // 补充数学运算符
            {0x2B00, 0x2BFF, UnicodeCategories::Category::Symbol_Other},         // 杂项符号和箭头
            {0x2E00, 0x2E7F, UnicodeCategories::Category::Punctuation_Other},    // 补充标点
            {0x2E80, 0x2EFF, UnicodeCategories::Category::Symbol_Other},         // CJK部首补充
            {0x2F00, 0x2FDF, UnicodeCategories::Category::Symbol_Other},         // 康熙部首
            {0x2FF0, 0x2FFF, UnicodeCategories::Category::Symbol_Other},         // 表意文字描述符
            {0x3000, 0x303F, UnicodeCategories::Category::Punctuation_Other},    // CJK符号和标点
            {0x3040, 0x309F, UnicodeCategories::Category::Letter_Other},         // 平假名
            {0x30A0, 0x30FF, UnicodeCategories::Category::Letter_Other},         // 片假名
            {0x3100, 0x312F, UnicodeCategories::Category::Letter_Other},         // 注音符号
            {0x3130, 0x318F, UnicodeCategories::Category::Letter_Other},         // 谚文兼容字母
            {0x3190, 0x319F, UnicodeCategories::Category::Symbol_Other},         // 象形字注释标志
            {0x31A0, 0x31BF, UnicodeCategories::Category::Letter_Other},         // 注音符号扩展
            {0x31F0, 0x31FF, UnicodeCategories::Category::Letter_Other},         // 片假名语音扩展
            {0x3200, 0x32FF, UnicodeCategories::Category::Symbol_Other},         // 带圈字符
            {0x3300, 0x33FF, UnicodeCategories::Category::Symbol_Other},         // CJK兼容
            {0x3400, 0x4DBF, UnicodeCategories::Category::Letter_Other},         // CJK统一表意文字扩展A
            {0x4DC0, 0x4DFF, UnicodeCategories::Category::Symbol_Other},         // 易经六十四卦符号
            {0x4E00, 0x9FFF, UnicodeCategories::Category::Letter_Other},         // CJK统一表意文字
            {0xA000, 0xA48F, UnicodeCategories::Category::Letter_Other},         // 彝文音节
            {0xA490, 0xA4CF, UnicodeCategories::Category::Symbol_Other},         // 彝文部首
            {0xAC00, 0xD7AF, UnicodeCategories::Category::Letter_Other},         // 朝鲜文音节
            {0xD800, 0xDBFF, UnicodeCategories::Category::Other_Surrogate},      // 高位代理项
            {0xDC00, 0xDFFF, UnicodeCategories::Category::Other_Surrogate},      // 低位代理项
            {0xE000, 0xF8FF, UnicodeCategories::Category::Other_PrivateUse},     // 私用区
            {0xF900, 0xFAFF, UnicodeCategories::Category::Letter_Other},         // CJK兼容表意文字
            {0xFB00, 0xFB4F, UnicodeCategories::Category::Letter_Other},         // 字母表示形式
            {0xFB50, 0xFDFF, UnicodeCategories::Category::Letter_Other},         // 阿拉伯表示形式A
            {0xFE00, 0xFE0F, UnicodeCategories::Category::Mark_NonSpacing},      // 变体选择符
            {0xFE10, 0xFE1F, UnicodeCategories::Category::Punctuation_Other},    // 竖排形式
            {0xFE20, 0xFE2F, UnicodeCategories::Category::Mark_NonSpacing},      // 组合用半符号
            {0xFE30, 0xFE4F, UnicodeCategories::Category::Punctuation_Other},    // CJK兼容形式
            {0xFE50, 0xFE6F, UnicodeCategories::Category::Punctuation_Other},    // 小型变体形式
            {0xFE70, 0xFEFF, UnicodeCategories::Category::Letter_Other},         // 阿拉伯表示形式B
            {0xFF00, 0xFFEF, UnicodeCategories::Category::Symbol_Other},         // 半角及全角形式
            {0xFFF0, 0xFFFF, UnicodeCategories::Category::Other_NotAssigned}     // 特殊
        };
    };

}  // namespace rp::frontend::unicode
