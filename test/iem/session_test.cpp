// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(SessionTest, Session) {
  const auto usr = "foo";
  const auto pwd = "bar";
  const Session s(usr, pwd);
  EXPECT_EQ(s.username(), usr);
  EXPECT_EQ(s.password(), pwd);
}

TEST(SessionTest, PriceParser) {
  auto px = iem::_parse_price("");
  EXPECT_EQ(px, iem::nanPrice());
}

TEST(SessionTest, PositionParser) {
  auto qty = iem::_parse_quantity("");
  EXPECT_EQ(qty, 0);
}

}  // namespace iem
