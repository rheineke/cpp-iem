// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

#include <string>
#include <tuple>
#include <vector>

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

TEST(EventStrategyTest, BuysTheWinnerAndSellsTheOthers) {
  const std::vector<std::tuple<Event, std::string, std::string, std::string>>
      cases{{Event::UP, "FRup1116", "FRsame1116", "FRdown1116"},
            {Event::SAME, "FRsame1116", "FRup1116", "FRdown1116"},
            {Event::DOWN, "FRdown1116", "FRup1116", "FRsame1116"}};
  for (const auto& [event, winner, other1, other2] : cases) {
    const auto os = event_orders(event, 3);
    ASSERT_EQ(os.size(), 4u);
    const auto asset = [&os](std::size_t i) {
      return dynamic_cast<const Single&>(*os[i]).contract().asset_id();
    };
    EXPECT_EQ(asset(0), Contract("FedPolicyB", winner).asset_id()) << winner;
    EXPECT_EQ(os[0]->side(), Side::BUY);
    EXPECT_EQ(asset(2), Contract("FedPolicyB", other1).asset_id());
    EXPECT_EQ(asset(3), Contract("FedPolicyB", other2).asset_id());
    EXPECT_EQ(os[2]->side(), Side::SELL);
    EXPECT_EQ(os[3]->side(), Side::SELL);
  }
}

}  // namespace iem
