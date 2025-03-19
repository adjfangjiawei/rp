#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

#include <cctype>
#include <sstream>

namespace rp {
    namespace frontend {

        bool SuffixProcessor::processSuffix(const std::string &input,
                                            size_t &pos,
                                            NumberValue &value,
                                            std::string &error) {
            SuffixState state = SuffixState::Start;
            SuffixState prevState;  // 添加前一个状态的变量
            const size_t startPos = pos;

            while (pos < input.length()) {
                char c = std::tolower(input[pos]);
                prevState = state;  // 保存当前状态

                switch (state) {
                    case SuffixState::Start:
                        if (c == 'u') {
                            if (value.isUnsigned) {
                                error = "Duplicate 'u' suffix";
                                return false;
                            }
                            value.isUnsigned = true;
                            state = SuffixState::AfterU;
                            pos++;
                        } else if (c == 'l') {
                            if (value.isLong || value.isLongLong) {
                                error = "Duplicate 'l' suffix";
                                return false;
                            }
                            state = SuffixState::AfterL;
                            pos++;
                        } else if (c == 'f') {
                            if (value.isFloat || value.isDouble) {
                                error = "Duplicate float suffix";
                                return false;
                            }
                            value.isFloat = true;
                            state = SuffixState::AfterF;
                            pos++;
                        } else if (c == 'd') {
                            if (value.isFloat || value.isDouble) {
                                error = "Duplicate float suffix";
                                return false;
                            }
                            value.isDouble = true;
                            state = SuffixState::AfterD;
                            pos++;
                        } else {
                            // 不是后缀字符，结束处理
                            break;
                        }
                        break;

                    case SuffixState::AfterU:
                        if (c == 'l') {
                            if (value.isLong || value.isLongLong) {
                                error = "Duplicate 'l' suffix after 'u'";
                                return false;
                            }
                            state = SuffixState::AfterL;
                            pos++;
                        } else {
                            // 其他字符不合法
                            if (std::isalpha(c)) {
                                error = "Invalid suffix combination with 'u'";
                                return false;
                            }
                            break;
                        }
                        break;

                    case SuffixState::AfterL:
                        if (c == 'l') {
                            if (value.isLongLong) {
                                error = "Duplicate 'll' suffix";
                                return false;
                            }
                            value.isLongLong = true;
                            value.isLong = false;  // 转换为long long
                            state = SuffixState::AfterLL;
                            pos++;
                        } else if (c == 'u') {
                            if (value.isUnsigned) {
                                error = "Duplicate 'u' suffix";
                                return false;
                            }
                            value.isUnsigned = true;
                            state = SuffixState::AfterU;
                            pos++;
                        } else {
                            value.isLong = true;
                            if (std::isalpha(c)) {
                                error = "Invalid suffix combination with 'l'";
                                return false;
                            }
                            break;
                        }
                        break;

                    case SuffixState::AfterLL:
                        if (c == 'u') {
                            if (value.isUnsigned) {
                                error = "Duplicate 'u' suffix";
                                return false;
                            }
                            value.isUnsigned = true;
                            state = SuffixState::AfterU;
                            pos++;
                        } else {
                            if (std::isalpha(c)) {
                                error = "Invalid suffix combination with 'll'";
                                return false;
                            }
                            break;
                        }
                        break;

                    case SuffixState::AfterF:
                        if (std::isalpha(c)) {
                            error = "Invalid suffix after 'f'";
                            return false;
                        }
                        break;

                    case SuffixState::AfterD:
                        if (std::isalpha(c)) {
                            error = "Invalid suffix after 'd'";
                            return false;
                        }
                        break;
                }

                if (state == prevState) {
                    // 如果状态没有改变，说明遇到了非后缀字符
                    break;
                }
            }

            // 验证后缀组合的合法性
            if (!validateSuffixCombination(value, error)) {
                pos = startPos;  // 恢复位置
                return false;
            }

            return true;
        }

        bool SuffixProcessor::validateSuffixCombination(const NumberValue &value, std::string &error) {
            // 检查浮点数和整数后缀的混用
            if ((value.isFloat || value.isDouble) && (value.isUnsigned || value.isLong || value.isLongLong)) {
                error = "Cannot combine floating point and integer suffixes";
                return false;
            }

            // 检查float和double的混用
            if (value.isFloat && value.isDouble) {
                error = "Cannot combine 'f' and 'd' suffixes";
                return false;
            }

            // 检查long和long long的混用
            if (value.isLong && value.isLongLong) {
                error = "Cannot combine 'l' and 'll' suffixes";
                return false;
            }

            // 根据数字类型检查后缀的合法性
            if (value.kind == NumberKind::FloatingPoint) {
                if (value.isUnsigned || value.isLong || value.isLongLong) {
                    error = "Invalid integer suffix on floating point literal";
                    return false;
                }
            } else {
                if (value.isFloat || value.isDouble) {
                    error = "Invalid floating point suffix on integer literal";
                    return false;
                }
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
