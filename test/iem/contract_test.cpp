// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include "gtest/gtest.h"

TEST(ContractTest, BadMarketNameException) {
  iem::Market m("FedPolicyB");
  EXPECT_EQ(m.value(), 51);
  EXPECT_ANY_THROW(iem::Market("FedPolicyC"));
}

// TODO(rheineke): Test throw "string" with struct implementing what()
// TODO(rheineke): Test contract and contract bundle in similar ways
