// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/order.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>

#include "gtest/gtest.h"

namespace iem {

TEST(OrderTest, Constructor) {
  const auto mkt_name("FedPolicyB");
  const auto contract_name("FRup1116");
  const Contract c(mkt_name, contract_name);
  const auto exp_ptime(read_expiration_ptime(mkt_name, contract_name));
  const PriceTimeLimit ptl(Price(1), exp_ptime);
  Single o(c, Side::BUY, 1, ptl);
  EXPECT_EQ(typeid(o), typeid(Single));
  EXPECT_EQ(o.id(), 0);
  EXPECT_EQ(std::numeric_limits<OrderId>::lowest(), 0);

  // OrderId to string
  o.set_id(42);
  const auto oid_str = std::to_string(o.id());
  EXPECT_EQ(oid_str, "42");

  const ContractBundle cb(mkt_name, MonthYear(boost::date_time::Nov, 16));
  EXPECT_NO_THROW(Bundle(cb, Side::BUY, 1, Counterparty::EXCHANGE));
}

TEST(OrderTest, DestroyedThroughBase) {
  // Orders holds Single and Bundle as std::unique_ptr<Order>
  EXPECT_TRUE(std::has_virtual_destructor_v<Order>);
  // The virtual destructor keeps the defaulted moves
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<Single>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<Single>);
}

TEST(OrderTest, QuantityMustBeOneUpToTheMaximum) {
  const Contract c("FedPolicyB", "FRsame1216");
  const PriceTimeLimit ptl(Price(1), boost::posix_time::not_a_date_time);
  for (const Quantity bad : {0, -1, kMaxOrderQuantity + 1}) {
    EXPECT_THROW(Single(c, Side::BUY, bad, ptl), std::invalid_argument) << bad;
  }
  const Single big(c, Side::BUY, 300, ptl);  // was sent as 44
  EXPECT_EQ(big.quantity(), 300);
  std::ostringstream os;
  os << big;
  EXPECT_NE(os.str().find("quantity=300"), std::string::npos);
}

TEST(OrderTest, ValidIdMeansIemGaveTheOrderAnId) {
  const Contract c("FedPolicyB", "FRsame1216");
  Single o(c, Side::BUY, 1,
           PriceTimeLimit(Price(1), boost::posix_time::not_a_date_time));
  EXPECT_FALSE(valid_id(o));
  o.set_id(4700000);
  EXPECT_TRUE(valid_id(o));
}

}  // namespace iem
