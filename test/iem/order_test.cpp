// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/order.hpp"

#include "gtest/gtest.h"

#include <typeindex>

namespace iem {

TEST(OrderTest, Constructor) {
  const auto mkt_name("FedPolicyB");
  const auto contract_name("FRup1116");
  const Contract c(mkt_name, contract_name);
  const auto exp_ptime(read_expiration_ptime(mkt_name, contract_name));
  const PriceTimeLimit ptl(Price(1), exp_ptime);
  Single o(c, Side::BUY, 1, ptl);
  EXPECT_EQ(typeid(o), typeid(Single));

  // OrderId to string
  o.set_id(42);
  const auto oid_str = std::to_string(o.id());
  EXPECT_EQ(oid_str, "42");

  const ContractBundle cb(mkt_name, MonthYear(boost::date_time::Nov, 16));
  EXPECT_NO_THROW(Bundle(cb, Side::BUY, 1, Counterparty::EXCHANGE));
}

}  // namespace iem