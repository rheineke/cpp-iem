// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(EventStrategyTest, AssertNoThrow) {
  const auto os = event_orders(Event::SAME, 1);
  EXPECT_EQ(os.size(), 4);
}

}  // namespace iem