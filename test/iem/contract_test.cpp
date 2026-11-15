// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "gtest/gtest.h"

namespace iem {

TEST(ContractTest, MarketMembers) {
  const auto& markets_dict = read_markets_json();
  for (const auto& market : markets_dict.items()) {
    const Market m(market.key());
    EXPECT_EQ(m.name(), market.key());
    EXPECT_EQ(m.value(), market.value().at("id").get<int>());
  }
}

TEST(ContractTest, BadMarketNameException) {
  EXPECT_ANY_THROW(iem::Market("FedPolicyC"));
  EXPECT_THROW(iem::Market("FedPolicyC"), std::invalid_argument);
}

const MonthYear expiration_monthyear(const std::string& expiration_str) {
  const auto mon_int = std::stoi(expiration_str.substr(0, 2));
  const auto mon = boost::date_time::months_of_year(mon_int);
  const auto yr = std::stoi(expiration_str.substr(2, 4));
  return MonthYear(mon, yr);
}

TEST(ContractTest, ContractBundle) {
  const auto& markets_dict = read_markets_json();

  for (const auto& market : markets_dict.items()) {
    const auto& name = market.key();
    const auto& bundles_value = market.value().at("bundle");

    // Expected market object
    const auto expected_market = Market(name);

    if (!bundles_value.contains("bundle_id")) {
      // Market has at least one expiration; iterate through all
      for (const auto& bundle : bundles_value.items()) {
        const auto exp_monthyear = expiration_monthyear(bundle.key());
        const ContractBundle cb(name, exp_monthyear);
        EXPECT_EQ(cb.market().value(), expected_market.value());
        EXPECT_EQ(cb.expiration(), exp_monthyear);
        EXPECT_EQ(cb.bundle_id(), bundle.value().at("bundle_id").get<int>());
      }
    } else {
      // Market has one expiration associated with it
      const auto expected_bundle_id = bundles_value.at("bundle_id").get<int>();
      const MonthYear null_exp(boost::date_time::months_of_year::NotAMonth, 0);
      const ContractBundle cb(name, null_exp);
      EXPECT_EQ(cb.market().value(), expected_market.value());
      EXPECT_EQ(cb.expiration(), null_exp);
      EXPECT_EQ(cb.bundle_id(), expected_bundle_id);
    }
  }
}

TEST(ContractTest, Contract) {
  const auto& markets_dict = read_markets_json();

  int contracts = 0;
  for (const auto& market : markets_dict.items()) {
    const auto& bundles_value = market.value().at("bundle");
    // A market's bundle node is either one bundle or a map of them
    const auto bundles = bundles_value.contains("bundle_id")
                             ? nlohmann::json::array({bundles_value})
                             : bundles_value;
    for (const auto& bundle : bundles) {
      for (const auto& asset : bundle.at("assets").items()) {
        const Contract c(market.key(), asset.key());
        EXPECT_EQ(c.market().value(), market.value().at("id").get<int>());
        EXPECT_EQ(c.asset_id(), asset.value().at("id").get<int>());
        EXPECT_EQ(c.asset_to_market_id(), asset.value().at("order").get<int>());
        ++contracts;
      }
    }
  }
  EXPECT_GT(contracts, 0);
}

TEST(ContractTest, UnknownContractThrows) {
  EXPECT_THROW(Contract("FedPolicyB", "FRup0199"), std::invalid_argument);
  EXPECT_THROW(Contract("FedPolicyB", "FRmissing0616"), std::invalid_argument);
}

TEST(ContractTest, MarketIsExplicit) {
  EXPECT_FALSE((std::is_convertible_v<std::string, Market>));
  EXPECT_TRUE((std::is_constructible_v<Market, std::string>));
}

TEST(ContractTest, MarketNameIsNotCopied) {
  const Market m("FedPolicyB");
  EXPECT_TRUE((std::is_same_v<decltype(m.name()), const std::string&>));
  EXPECT_EQ(m.name(), "FedPolicyB");
}

TEST(ContractTest, BundleYearMayHaveFourDigits) {
  using boost::gregorian::Dec;
  const ContractBundle two("FedPolicyB", MonthYear(Dec, 16));
  const ContractBundle four("FedPolicyB", MonthYear(Dec, 2016));
  EXPECT_EQ(four.bundle_id(), two.bundle_id());
  for (const Year bad : {-1, 100, 1999, 2100}) {
    EXPECT_THROW(ContractBundle("FedPolicyB", MonthYear(Dec, bad)),
                 std::invalid_argument)
        << bad;
  }
}

TEST(ContractTest, MonthYearOutput) {
  std::ostringstream os;
  os << MonthYear(boost::date_time::Nov, 16);
  EXPECT_EQ(os.str(), "(11, 16)");
}

}  // namespace iem
