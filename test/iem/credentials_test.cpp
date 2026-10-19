// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/credentials.hpp"

#include <cstdlib>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

namespace iem {

class CredentialsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    unsetenv("IEM_USERNAME");
    unsetenv("IEM_PASSWORD");
  }
  void TearDown() override { SetUp(); }
};

TEST_F(CredentialsTest, ReadsBothVariables) {
  setenv("IEM_USERNAME", "foo", 1);
  setenv("IEM_PASSWORD", "bar", 1);

  const auto c = credentials_from_env();

  EXPECT_EQ(c.username, "foo");
  EXPECT_EQ(c.password, "bar");
}

TEST_F(CredentialsTest, NamesWhatIsMissing) {
  setenv("IEM_USERNAME", "foo", 1);
  setenv("IEM_PASSWORD", "", 1);
  try {
    credentials_from_env();
    FAIL() << "expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    const std::string what = e.what();
    EXPECT_NE(what.find("IEM_PASSWORD"), std::string::npos);
    EXPECT_EQ(what.find("IEM_USERNAME"), std::string::npos);
  }
}

TEST_F(CredentialsTest, NothingSetThrows) {
  EXPECT_THROW(credentials_from_env(), std::runtime_error);
}

}  // namespace iem
