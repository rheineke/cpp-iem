// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/order.hpp"

#include "gtest/gtest.h"

#include <typeindex>

namespace iem {

TEST(OrderTest, Constructor) {
  const auto contract_name = "FRup1116";
  const Contract c(contract_name);
  const auto exp_ptime(read_expiration_ptime(c.market().name(), contract_name));
  const PriceTimeLimit ptl(Price(1), exp_ptime);
  const Single o(c, Side::BUY, 1, ptl);
  EXPECT_EQ(typeid(o), typeid(Single));

  const ContractBundle cb("FedPolicyB", MonthYear(boost::date_time::Nov, 16));
  EXPECT_NO_THROW(Bundle(cb, Side::BUY, 1, Counterparty::EXCHANGE));
}

}  // namespace iem