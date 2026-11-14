// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

#include "gtest/gtest.h"
#include "iem/contract.hpp"

namespace iem {

boost::posix_time::ptime _test_ptime() {
  const boost::gregorian::date dt(2016, boost::date_time::Jan, 10);
  const boost::posix_time::time_duration t(20, 01, 00);
  return boost::posix_time::ptime(dt, t);
}

TEST(PriceTimeLimitTest, ValidConstructorArguments) {
  using boost::date_time::not_a_date_time;
  const auto nan_px = nanPrice();
  EXPECT_NO_THROW(PriceTimeLimit(nan_px, not_a_date_time));  // IOC
  const Price px(999);
  EXPECT_NO_THROW(PriceTimeLimit(px, not_a_date_time));  // GTC limit
  EXPECT_NO_THROW(PriceTimeLimit(px, _test_ptime()));    // Non-GTC limit
  EXPECT_THROW(PriceTimeLimit(nan_px, _test_ptime()), std::invalid_argument);
}

TEST(PriceTimeLimitTest, IOC) {
  EXPECT_TRUE(PriceTimeLimit().ioc());
  const Price px(999);
  EXPECT_FALSE(PriceTimeLimit(px, _test_ptime()).ioc());
  EXPECT_FALSE(PriceTimeLimit(px, boost::date_time::not_a_date_time).ioc());
}

TEST(PriceTimeLimitTest, SessionStringFormat) {
  const auto ptime = _test_ptime();
  const auto& d = ptime.date();
  EXPECT_EQ(boost::posix_time::to_iso_extended_string(ptime),
            "2016-01-10T20:01:00");
  EXPECT_EQ(boost::gregorian::to_iso_extended_string(d), "2016-01-10");
  EXPECT_EQ(to_string(ptime), "2016-01-10 08:01 PM");
  EXPECT_EQ(to_string(boost::posix_time::not_a_date_time), "No expiration");
}

TEST(PriceTimeLimitTest, ExpiryDate) {
  const auto& markets_dict = read_markets_json();

  for (const auto& market : markets_dict.items()) {
    const auto& mkt_name = market.key();
    const auto& bundle_value = market.value().at("bundle");
    if (!bundle_value.contains("assets")) {
      for (const auto& bundle : bundle_value) {
        for (const auto& asset : bundle.at("assets").items()) {
          EXPECT_NO_THROW(read_expiration_ptime(mkt_name, asset.key()));
        }
      }
    } else {
      for (const auto& asset : bundle_value.at("assets").items()) {
        EXPECT_NO_THROW(read_expiration_ptime(mkt_name, asset.key()));
      }
    }
  }
}

TEST(PriceTimeLimitTest, ExpiryDateValue) {
  // FedPolicyB's 0616 bundle expires 2016-06-15, at the end of the day
  EXPECT_EQ(boost::posix_time::to_iso_extended_string(
                read_expiration_ptime("FedPolicyB", "FRup0616")),
            "2016-06-15T23:59:59");
}

}  // namespace iem
