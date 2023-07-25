#include <assert.h>
#include <exception>
#include <iostream>
#include <string>

namespace mylib {
namespace errc {

enum my_error {
  isLegalVariableName = 1,

};

inline const char *error_message(int c) {
  static const char *err_msg[] = {
      "hdfgsg",
  };

  assert(c < sizeof(err_msg) / sizeof(err_msg[0]));
  return err_msg[c];
}

class my_error_category : public std::error_category {
public:
  my_error_category() {}

  std::string message(int c) const { return error_message(c); }

  const char *name() const noexcept { return "My Error Category"; }

  const static error_category &get() {
    const static my_error_category category_const;
    return category_const;
  }
};

inline std::error_code make_error_code(my_error e) {
  return std::error_code(static_cast<int>(e), my_error_category::get());
}

} // end namespace errc
} // end namespace mylib

namespace std {

template <>
struct is_error_code_enum<mylib::errc::my_error> : std::true_type {};

} // end namespace std
