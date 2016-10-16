// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include "gtest/gtest.h"

TEST(SessionTest, PriceParser) {
  auto px = iem::_parse_price("");
  EXPECT_EQ(px, iem::nanPrice());
}

TEST(SessionTest, PositionParser) {
  auto qty = iem::_parse_quantity("");
  EXPECT_EQ(qty, 0);
}
