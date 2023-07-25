#include <pprint.hpp>
#include <sstream>
class sPrittyPrinter {
public:
  sPrittyPrinter(std::stringstream *stream) : stream(stream) {
    printer = new pprint::PrettyPrinter(*stream);
  }
  template <typename T> inline auto print(T value) {
    stream->str("");
    printer->print(value);
    return stream->str();
  }

  template <typename T> auto print(std::initializer_list<T> value) {
    stream->str("");
    printer->print(value);
    return stream->str();
  }

  template <typename T, typename... Targs> auto print(T value, Targs... Fargs) {
    stream->str("");
    printer->print(value, Fargs...);
    return stream->str();
  }

private:
  pprint::PrettyPrinter *printer;
  std::stringstream *stream;
};

auto getPPrint() {
  pprint::PrettyPrinter printer;
  printer.quotes(true);
  auto sstream = std::stringstream{};
  sPrittyPrinter sprinter(&sstream);
  return std::make_pair(printer, sprinter);
}