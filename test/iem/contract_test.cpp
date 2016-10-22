// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "boost/filesystem.hpp"

#include "iem/contract.hpp"

#include "gtest/gtest.h"

// TODO(rheineke): Ensure market constructor fails correctly on bad market name
TEST(ContractTest, BadMarketNameException) {
  std::cout << "Current path: " << boost::filesystem::current_path() << std::endl;
  iem::Market m("FedPolicyB", "conf/markets.json");
  EXPECT_TRUE(m.value() == 51);
  EXPECT_ANY_THROW(iem::Market("FedPolicyC"));
  // EXPECT_THROW(iem::Market("FedPolicyC"), std::invalid_argument);
}

// TODO(rheineke): Test throw "string" with struct implementing what()
// TODO(rheineke): Test contract and contract bundle in similar ways
