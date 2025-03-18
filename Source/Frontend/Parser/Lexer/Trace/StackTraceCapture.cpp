#include "StackTraceCapture.h"

#include <cxxabi.h>
#include <execinfo.h>

#include <cstdlib>
#include <sstream>

namespace rp {
    namespace frontend {
        namespace memory {
            namespace stacktrace {
                std::string demangleSymbol(const std::string& symbol) {
                    size_t nameStart = symbol.find('(');
                    size_t nameEnd = symbol.find('+', nameStart);

                    if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                        std::string mangledName = symbol.substr(nameStart + 1, nameEnd - nameStart - 1);
                        int status;
                        char* demangledName = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);

                        if (status == 0 && demangledName) {
                            std::string result = demangledName;
                            free(demangledName);
                            return result;
                        }
                    }
                    return symbol;
                }

                std::string captureStackTrace() {
                    const int MAX_FRAMES = 32;
                    void* frames[MAX_FRAMES];
                    int numFrames = backtrace(frames, MAX_FRAMES);
                    char** symbols = backtrace_symbols(frames, numFrames);

                    std::stringstream ss;
                    // Skip first frame (this function)
                    for (int i = 1; i < numFrames; ++i) {
                        std::string symbol(symbols[i]);
                        std::string demangled = demangleSymbol(symbol);
                        ss << "    " << i << ": " << demangled << "\n";
                    }

                    free(symbols);
                    return ss.str();
                }
            }  // namespace stacktrace
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
