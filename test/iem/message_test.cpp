// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "iem/message.hpp"

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
  EXPECT_EQ(to_string(Action::BUY), "Buy");
  EXPECT_EQ(to_string(Action::SELL), "Sell");
  EXPECT_EQ(to_string(Action::ASK_ENTERED), "Ask entered");
  EXPECT_EQ(to_string(Action::BID_ENTERED), "Bid entered");
  EXPECT_EQ(to_string(Action::FIXED_BUNDLE_PURCHASE_EXECUTED),
            "Fixed bundle purchase executed");
  EXPECT_EQ(to_string(Action::FIXED_BUNDLE_SALE_EXECUTED),
            "Fixed bundle sale executed");
  EXPECT_EQ(to_string(Action::MARKET_BUNDLE_PURCHASE_EXECUTED),
            "Market bundle purchase executed");
  EXPECT_EQ(to_string(Action::MARKET_BUNDLE_SALE_EXECUTED),
            "Market bundle sale executed");

  // Deserialization
  EXPECT_EQ(action_from_string("Buy"), Action::BUY);
  EXPECT_EQ(action_from_string("Sell"), Action::SELL);
  EXPECT_EQ(action_from_string("Ask entered"), Action::ASK_ENTERED);
  EXPECT_EQ(action_from_string("Bid entered"), Action::BID_ENTERED);
  EXPECT_EQ(action_from_string("Fixed bundle purchase executed"),
            Action::FIXED_BUNDLE_PURCHASE_EXECUTED);
  EXPECT_EQ(action_from_string("Buy Fixed Price Bundle"),
            Action::FIXED_BUNDLE_PURCHASE_EXECUTED);
  EXPECT_EQ(action_from_string("Sell Fixed Price Bundle"),
            Action::FIXED_BUNDLE_SALE_EXECUTED);
}

TEST(TraderMessageTest, TradeMessageDateFromString) {
  const boost::gregorian::date d(2016, boost::gregorian::Nov, 28);
  const boost::posix_time::time_duration t(19, 19, 30, 613000);
  const boost::posix_time::ptime expected_dt(d, t);
  const auto dt = date_from_string("2016-11-28 19:19:30.613");
  EXPECT_EQ(dt, expected_dt);
}

TEST(TraderMessageTest, Quantity) {
  EXPECT_NO_THROW(std::stoi("1.000000"));
}

}  // namespace iem
