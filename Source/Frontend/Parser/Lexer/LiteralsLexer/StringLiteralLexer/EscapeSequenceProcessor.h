
#ifndef ESCAPESEQUENCEPROCESSOR_H
#define ESCAPESEQUENCEPROCESSOR_H

#include <string>

namespace rp {
    namespace frontend {

        class EscapeSequenceProcessor {
          public:
            static std::string processEscapeSequence(const std::string& source, size_t& currentPos, std::string& error);
            static bool isValidEscapeSequence(char c);

          private:
            static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processOctalEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processUnicodeEscape(const std::string& source, size_t& currentPos, std::string& error);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // ESCAPESEQUENCEPROCESSOR_H
