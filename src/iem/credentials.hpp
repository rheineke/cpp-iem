// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_CREDENTIALS_HPP_
#define CPPIEM_IEM_CREDENTIALS_HPP_

#include <string>

namespace iem {

struct Credentials {
  std::string username;
  std::string password;
};

// Reads the IEM login from the IEM_USERNAME and IEM_PASSWORD environment
// variables, which a secret store fills in (see README). Throws
// std::runtime_error naming each one that is unset or empty.
Credentials credentials_from_env();

}  // namespace iem

#endif  // CPPIEM_IEM_CREDENTIALS_HPP_
