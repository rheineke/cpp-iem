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
  EXPECT_EQ(s.cookie(), "");
}

TEST(SessionTest, PriceParser) {
  auto px = _parse_price("");
  EXPECT_EQ(px, nanPrice());
}

TEST(SessionTest, PositionParser) {
  auto qty = _parse_quantity("");
  EXPECT_EQ(qty, 0);
}

TEST(SessionTest, LimitOrderRequest) {
  const iem::Contract c("FedPolicyB", "FRsame1216");
  const iem::Price px(1);
  const iem::PriceTimeLimit ptl(px, boost::posix_time::not_a_date_time);
  const iem::Single o(c, iem::Side::BUY, 1, ptl);
  const auto request = iem::_order_request(o);
  // TODO(rheineke): Unit test aspects of this
  std::cout << body(request) << std::endl;
}

}  // namespace iem
