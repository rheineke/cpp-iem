// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/credentials.hpp"

#include <cstdlib>
#include <stdexcept>

namespace iem {

namespace {

std::string env(const char* name) {
  const char* value = std::getenv(name);
  return value ? value : "";
}

}  // namespace

Credentials credentials_from_env() {
  Credentials c{env("IEM_USERNAME"), env("IEM_PASSWORD")};
  std::string missing;
  if (c.username.empty()) {
    missing += " IEM_USERNAME";
  }
  if (c.password.empty()) {
    missing += " IEM_PASSWORD";
  }
  if (!missing.empty()) {
    throw std::runtime_error("Set the IEM login in the environment:" + missing);
  }
  return c;
}

}  // namespace iem
