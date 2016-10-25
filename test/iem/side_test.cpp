// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/side.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(SideTest, OtherSide) {
EXPECT_EQ(otherSide(Side::buy), Side::sell);
EXPECT_EQ(otherSide(Side::sell), Side::buy);
}

}  // namespace iem
