// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(EventStrategyTest, AssertNoThrow) {
  const auto os = event_orders(Event::SAME, 1);
  EXPECT_EQ(os.size(), 4);
}

TEST(EventStrategyTest, Orders) {
  const auto os = event_orders(Event::SAME, 2);
  ASSERT_EQ(os.size(), 4);
  // Buy the in-the-money contract and a bundle from the exchange, then sell
  // the two out-of-the-money contracts
  EXPECT_EQ(os[0]->side(), Side::BUY);
  EXPECT_EQ(os[0]->price_time_limit().price(), Price(999));
  EXPECT_EQ(os[1]->side(), Side::BUY);
  EXPECT_EQ(os[1]->counterparty(), Counterparty::EXCHANGE);
  EXPECT_EQ(os[2]->side(), Side::SELL);
  EXPECT_EQ(os[3]->side(), Side::SELL);
  EXPECT_EQ(os[3]->price_time_limit().price(), Price(1));
  for (const auto& o : os) {
    EXPECT_EQ(o->quantity(), 2);
  }
}

}  // namespace iem
