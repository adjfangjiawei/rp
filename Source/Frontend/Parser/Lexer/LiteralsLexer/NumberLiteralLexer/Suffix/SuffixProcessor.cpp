
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer/Suffix/SuffixProcessor.h"

#include <cctype>

namespace rp {
    namespace frontend {

        bool SuffixProcessor::processSuffix(const std::string &input,
                                            size_t &pos,
                                            NumberValue &value,
                                            std::string &error) {
            SuffixState state = SuffixState::Start;

            while (pos < input.length()) {
                char c = std::tolower(input[pos]);

                switch (state) {
                    case SuffixState::Start:
                        switch (c) {
                            case 'u':
                                state = SuffixState::AfterU;
                                value.isUnsigned = true;
                                break;
                            case 'l':
                                if (pos + 1 < input.length() && std::tolower(input[pos + 1]) == 'l') {
                                    state = SuffixState::AfterLL;
                                    value.isLongLong = true;
                                    pos++;
                                } else {
                                    state = SuffixState::AfterL;
                                    value.isLong = true;
                                }
                                break;
                            case 'f':
                                if (value.kind != NumberKind::FloatingPoint) {
                                    error = "Float suffix 'f' can only be used with floating point literals";
                                    return false;
                                }
                                state = SuffixState::AfterF;
                                value.isFloat = true;
                                break;
                            case 'd':
                                if (value.kind != NumberKind::FloatingPoint) {
                                    error = "Double suffix 'd' can only be used with floating point literals";
                                    return false;
                                }
                                state = SuffixState::AfterD;
                                value.isDouble = true;
                                break;
                            default:
                                if (std::isalpha(c)) {
                                    error = "Invalid suffix '" + std::string(1, c) + "'";
                                    return false;
                                }
                                return validateSuffixCombination(value, error);
                        }
                        break;

                    case SuffixState::AfterU:
                        if (c == 'l') {
                            if (pos + 1 < input.length() && std::tolower(input[pos + 1]) == 'l') {
                                state = SuffixState::AfterLL;
                                value.isLongLong = true;
                                pos++;
                            } else {
                                state = SuffixState::AfterL;
                                value.isLong = true;
                            }
                        } else {
                            if (std::isalpha(c)) {
                                error = "Invalid character '" + std::string(1, c) + "' after 'u' suffix";
                                return false;
                            }
                            return validateSuffixCombination(value, error);
                        }
                        break;

                    case SuffixState::AfterL:
                    case SuffixState::AfterLL:
                    case SuffixState::AfterF:
                    case SuffixState::AfterD:
                        if (std::isalpha(c)) {
                            error = "Unexpected character after number suffix";
                            return false;
                        }
                        return validateSuffixCombination(value, error);
                }
                pos++;
            }

            return validateSuffixCombination(value, error);
        }

        bool SuffixProcessor::validateSuffixCombination(const NumberValue &value, std::string &error) {
            if (value.isFloat && value.isDouble) {
                error = "Cannot combine 'f' and 'd' suffixes";
                return false;
            }

            if ((value.isFloat || value.isDouble) && (value.isUnsigned || value.isLong || value.isLongLong)) {
                error = "Cannot combine floating point and integer suffixes";
                return false;
            }

            if (value.isLong && value.isLongLong) {
                error = "Cannot combine 'l' and 'll' suffixes";
                return false;
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
