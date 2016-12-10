// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/side.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(SideTest, OtherSide) {
  EXPECT_EQ(otherSide(Side::BUY), Side::SELL);
  EXPECT_EQ(otherSide(Side::SELL), Side::BUY);
}

}  // namespace iem
