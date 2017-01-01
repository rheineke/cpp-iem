// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/config.hpp"

#include "gtest/gtest.h"

#include "boost/date_time/gregorian/gregorian.hpp"

namespace iem {

TEST(ConfigTest, ReadJson) {
  EXPECT_NO_THROW(read_markets_json());
}

TEST(ConfigTest, MarketValue) {
  const auto& root = read_markets_json();
  for (const auto& market_name : root.getMemberNames()) {
    EXPECT_FALSE(market_value(root, market_name).isNull());
  }
}

TEST(ConfigTest, BundleValue) {
  const auto& root = read_markets_json();
  for (const auto& market_name : root.getMemberNames()) {
    // Non-const return value will insert a null value for "bundle"
    const auto& bundle_val = root[market_name]["bundle"];
    if (bundle_val["bundle_id"].isNull()) {  // Market has multiple bundles
      for (const auto& bundle_name : bundle_val.getMemberNames()) {
        EXPECT_FALSE(bundle_value(root, market_name, bundle_name).isNull());
      }
    } else {  // Market has one bundle
      EXPECT_FALSE(bundle_value(root, market_name, "").isNull());
    }
  }
}

TEST(ConfigTest, AssetValue) {
  const auto& root = read_markets_json();
  for (const auto& market_name : root.getMemberNames()) {
    // Non-const return value will insert a null value for "bundle"
    const auto& bundles_val = root[market_name]["bundle"];
    if (bundles_val["bundle_id"].isNull()) {  // Market has multiple bundles
      for (const auto& bundle_val : bundles_val) {
        const auto& assets_val = bundle_val["assets"];
        for (const auto& asset_name : assets_val.getMemberNames()) {
          EXPECT_FALSE(asset_value(root, market_name, asset_name).isNull());
        }
      }
    } else {  // Market has one bundle
      const auto& assets_val = bundles_val["assets"];
      for (const auto& asset_name : assets_val.getMemberNames()) {
        EXPECT_FALSE(asset_value(root, market_name, asset_name).isNull());
      }
    }
  }
}

TEST(ConfigTest, ValueTest) {
  // TODO(rheineke): Ensure that each attribute is unique (bundle_id, etc)
}

TEST(ConfigTest, DateStringToPtime) {
  // from_simple_string will return not_a_date_time date with empty string
  const auto nad = from_simple_string("");
  EXPECT_EQ(nad, boost::gregorian::date(boost::posix_time::not_a_date_time));

  // from_simple_string should match boost::gregorian when a correctly formatted
  // date time is provided
  const auto s("2016-12-31");
  EXPECT_EQ(from_simple_string(s), boost::gregorian::from_simple_string(s));
}

}  // namespace iem
