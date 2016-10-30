// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(EventStrategyTest, AssertNoThrow) {
  EXPECT_NO_THROW(event_orders(Event::SAME, 1));
}

}  // namespace iem