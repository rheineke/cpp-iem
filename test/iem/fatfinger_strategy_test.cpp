// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/fatfinger_strategy.hpp"

#include <set>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

namespace iem {

TEST(FatfingerStrategyTest, BuyAndSellEveryAssetInBundle) {
  const Market mkt("FedPolicyB");
  const auto orders = fatfinger_orders(mkt, "0616", 2);

  // FedPolicyB's 0616 bundle holds FRup0616, FRsame0616 and FRdown0616
  ASSERT_EQ(orders.size(), 6U);
  std::set<AssetId> assets;
  for (std::size_t i = 0; i < orders.size(); ++i) {
    const auto* order = dynamic_cast<const Single*>(orders[i].get());
    ASSERT_NE(order, nullptr);
    EXPECT_EQ(order->quantity(), 2);
    EXPECT_FALSE(order->price_time_limit().ioc());
    // Bid at the lowest price, offer at the highest
    if (i % 2 == 0) {
      EXPECT_EQ(order->side(), Side::BUY);
      EXPECT_EQ(order->price_time_limit().price(), Price(1));
    } else {
      EXPECT_EQ(order->side(), Side::SELL);
      EXPECT_EQ(order->price_time_limit().price(), Price(999));
    }
    assets.insert(order->contract().asset_id());
  }
  EXPECT_EQ(assets, (std::set<AssetId>{3042, 3043, 3044}));
}

TEST(FatfingerStrategyTest, UnknownExpiryThrows) {
  EXPECT_THROW(fatfinger_orders(Market("FedPolicyB"), "0199", 1),
               std::invalid_argument);
}

}  // namespace iem
