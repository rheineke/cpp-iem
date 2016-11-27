// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "iem/trader_message.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(TraderMessageTest, MessageType) {
  // Serialization
  EXPECT_EQ(to_string(MessageType::ORDER), "Order");
  EXPECT_EQ(to_string(MessageType::ORDER_RESOLUTION), "Order Resolution");

  // Deserialization
  EXPECT_EQ(message_type_from_string("Order"), MessageType::ORDER);
  EXPECT_EQ(message_type_from_string("Order Resolution"),
            MessageType::ORDER_RESOLUTION);
  EXPECT_THROW(message_type_from_string("foo"), std::invalid_argument);
}

TEST(TraderMessageTest, Action) {
  // Serialization
  EXPECT_EQ(to_string(Action::ASK_ENTERED), "Ask entered");
  EXPECT_EQ(to_string(Action::BID_ENTERED), "Bid entered");
  EXPECT_EQ(to_string(Action::FIXED_BUNDLE_PURCHASE_EXECUTED),
            "Fixed bundle purchase executed");

  // Deserialization
  EXPECT_EQ(action_from_string("Ask entered"), Action::ASK_ENTERED);
  EXPECT_EQ(action_from_string("Bid entered"), Action::BID_ENTERED);
  EXPECT_EQ(action_from_string("Fixed bundle purchase executed"),
            Action::FIXED_BUNDLE_PURCHASE_EXECUTED);
}

TEST(SessionTest, DISABLED_TradeMessageDateFromString) {
  const boost::gregorian::date d(2016, boost::date_time::Nov, 25);
  const boost::posix_time::time_duration t(15, 20, 33);
  const boost::posix_time::ptime expected_dt(d, t);
  const auto dt = date_from_string("Nov 25, 2016 03:20:33");
  EXPECT_EQ(dt, expected_dt);
}

}  // namespace iem
