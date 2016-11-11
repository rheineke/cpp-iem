// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

#include "gtest/gtest.h"

#include "iem/contract.hpp"

namespace iem {

boost::posix_time::ptime _test_ptime() {
  const auto dt = boost::gregorian::date(2016, boost::date_time::Jan, 10);
  const auto t = boost::posix_time::time_duration(20, 01, 00);
  return boost::posix_time::ptime(dt, t);
}

TEST(PriceTimeLimitTest, ValidConstructorArguments) {
  using boost::date_time::not_a_date_time;
  const auto nan_px = nanPrice();
  EXPECT_NO_THROW(PriceTimeLimit(nan_px, not_a_date_time));  // IOC
  const Price px(999);
  EXPECT_NO_THROW(PriceTimeLimit(px, not_a_date_time));  // GTC limit
  EXPECT_NO_THROW(PriceTimeLimit(px, _test_ptime()));  // Non-GTC limit
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
  const auto& tod = ptime.time_of_day();
  EXPECT_EQ(boost::posix_time::to_iso_extended_string(ptime),
            "2016-01-10T20:01:00");
  EXPECT_EQ(boost::gregorian::to_iso_extended_string(d), "2016-01-10");
  EXPECT_EQ(to_string(ptime), "2016-01-10 08:01 PM");
  EXPECT_EQ(to_string(boost::posix_time::not_a_date_time), "No expiration");
}

TEST(PriceTimeLimitTest, ExpiryDate) {
  const auto& markets_dict = read_markets_json();

  for (const auto& mkt_name : markets_dict.getMemberNames()) {
    const auto& bundle_value = markets_dict[mkt_name]["bundle"];
    const auto& assets_value = bundle_value["assets"];
    if (assets_value.isNull()) {
      for (const auto& bundle_name : bundle_value.getMemberNames()) {
        const auto& assets_value = bundle_value[bundle_name]["assets"];
        for (const auto &asset_name : assets_value.getMemberNames()) {
          EXPECT_NO_THROW(read_expiration_ptime(mkt_name, asset_name));
        }
      }
    } else {
      for (const auto &asset_name : assets_value.getMemberNames()) {
        EXPECT_NO_THROW(read_expiration_ptime(mkt_name, asset_name));
      }
    }
  }
}

}  // namespace iem
