// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include <string>

#include "iem/contract.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(ContractTest, MarketMembers) {
  const auto& markets_dict = read_markets_json();
  for (const auto& name : markets_dict.getMemberNames()) {
    const Market m(name);
    EXPECT_EQ(m.name(), name);
    EXPECT_EQ(m.value(), markets_dict[name]["id"].asInt());
  }
}

TEST(ContractTest, BadMarketNameException) {
  EXPECT_ANY_THROW(iem::Market("FedPolicyC"));
  // TODO(rheineke): Test throw "string" with struct implementing what()
}

const MonthYear expiration_monthyear(const std::string& expiration_str) {
  const auto mon_int = std::stoi(expiration_str.substr(0, 2));
  const auto mon = boost::date_time::months_of_year(mon_int);
  const auto yr = std::stoi(expiration_str.substr(2, 4));
  return MonthYear(mon, yr);
}

TEST(ContractTest, ContractBundle) {
  const auto markets_dict = read_markets_json();

  for (const auto& name : markets_dict.getMemberNames()) {
    const auto bundles_value = markets_dict[name]["bundle"];

    // Expected market object
    const auto expected_market = Market(name);

    const auto bundle_id_value = bundles_value["bundle_id"];
    if (bundle_id_value.isNull()) {
      // Market has at least one expiration; iterate through all
      for (const auto& bundle_exp : bundles_value.getMemberNames()) {
        const auto exp_monthyear = expiration_monthyear(bundle_exp);
        const ContractBundle cb(name, exp_monthyear);
        EXPECT_EQ(cb.market().value(), expected_market.value());
        EXPECT_EQ(cb.expiration(), exp_monthyear);
        EXPECT_EQ(cb.bundle_id(), bundles_value[bundle_exp]["bundle_id"].asInt());
      }
    } else {
      // Market has one expiration associated with it
      const auto expected_bundle_id = bundle_id_value.asInt();
      const MonthYear null_exp(boost::date_time::months_of_year::NotAMonth, 0);
      const ContractBundle cb(name, null_exp);
      EXPECT_EQ(cb.market().value(), expected_market.value());
      EXPECT_EQ(cb.expiration(), null_exp);
      EXPECT_EQ(cb.bundle_id(), expected_bundle_id);
    }
  }
}

TEST(ContractTest, Contract) {
  const auto markets_dict = read_markets_json();

  for (const auto& mkt_name : markets_dict.getMemberNames()) {
    const auto& mkt_dict = markets_dict[mkt_name];
    const auto& assets_value = mkt_dict["assets"];
    for (const auto& contract_name : assets_value.getMemberNames()) {
      Contract c(mkt_name, contract_name);
      EXPECT_EQ(c.market().value(), mkt_dict["id"].asInt());
      EXPECT_EQ(c.asset_id(), assets_value[contract_name]["id"].asInt());
    }
  }
}

}  // namespace iem
