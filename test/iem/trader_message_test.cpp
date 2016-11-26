// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "iem/trader_message.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(TraderMessageTest, MessageType) {
  EXPECT_EQ(to_string(MessageType::ORDER), "order");
  EXPECT_EQ(to_string(MessageType::RESOLUTION), "resolution");
}

TEST(SessionTest, TradeMessageDateFromString) {
  const boost::gregorian::date d(2016, boost::date_time::Nov, 2);
  const boost::posix_time::time_duration t(20, 30, 54, 257000);
  const boost::posix_time::ptime expected_dt(d, t);
  const auto dt = date_from_string("2016-11-02 20:30:54.257");
  EXPECT_EQ(dt, expected_dt);
}

}  // namespace iem
