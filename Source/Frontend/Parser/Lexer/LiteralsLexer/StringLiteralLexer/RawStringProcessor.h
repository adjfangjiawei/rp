#ifndef RAWSTRINGPROCESSOR_H
#define RAWSTRINGPROCESSOR_H

#include <string>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
namespace rp {
    namespace frontend {

        class RawStringProcessor {
          public:
            static Token processRawStringLiteral(const std::string& source,
                                                 size_t& currentPos,
                                                 const SourceLocation& startLoc);

          private:
            static bool isValidDelimiter(const std::string& delimiter);
        };

    }  // namespace frontend

}  // namespace rp
#endif  // RAWSTRINGPROCESSOR_H
