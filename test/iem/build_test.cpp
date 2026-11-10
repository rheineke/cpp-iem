// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
// Checks the language standard that CMakeLists.txt selects.
#include "gtest/gtest.h"

TEST(BuildTest, CompilesAsCpp20) { EXPECT_GE(__cplusplus, 202002L); }
