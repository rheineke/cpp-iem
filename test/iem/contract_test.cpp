// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include "gtest/gtest.h"

// TODO: Ensure market constructor fails correctly on bad market name
TEST(ContractTest, BadMarketNameException) {
  iem::Market m("FedPolicyB", "/Users/rheineke/code/ciem/conf/markets.json");
  EXPECT_TRUE(m.value() == 51);
  // EXPECT_ANY_THROW(iem::Market("FedPolicyC"));
  // EXPECT_THROW(iem::Market("FedPolicyC"), std::invalid_argument);
}

// TODO: Test throw "string" with struct implementing what()
// TODO: Test contract and contract bundle in similar ways