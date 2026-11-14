// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/config.hpp"

#include <stdexcept>
#include <string>
#include <utility>

#include "boost/date_time/gregorian/gregorian.hpp"
#include "gtest/gtest.h"

namespace iem {

namespace {

// One market with two bundles and one market with a single bundle
const nlohmann::json& test_markets() {
  static const auto root = nlohmann::json::parse(R"({
    "Multi": {"id": 1, "bundle": {
      "0616": {"bundle_id": 10, "assets": {"Up0616": {"id": 100}}},
      "0716": {"bundle_id": 11, "assets": {"Up0716": {"id": 110}}}}},
    "Single": {"id": 2, "bundle": {"bundle_id": 20, "assets": {"Yes": {"id": 200}}}}
  })");
  return root;
}

}  // namespace

TEST(ConfigTest, ReadJson) {
  EXPECT_NO_THROW(read_markets_json());
  EXPECT_TRUE(read_markets_json().is_object());
}

TEST(ConfigTest, ReadJsonReadsOnce) {
  // Later calls return the first document, whatever their filename
  const auto& root = read_markets_json();
  EXPECT_EQ(&read_markets_json("missing.json"), &root);
}

TEST(ConfigTest, MarketValue) {
  const auto& root = read_markets_json();
  for (const auto& market : root.items()) {
    EXPECT_EQ(&market_value(root, market.key()), &market.value());
  }
}

TEST(ConfigTest, UnknownMarketThrows) {
  EXPECT_THROW(market_value(test_markets(), "Missing"), std::invalid_argument);
  const nlohmann::json empty;
  EXPECT_THROW(market_value(empty, "Multi"), std::invalid_argument);
}

TEST(ConfigTest, BundleValue) {
  const auto& root = read_markets_json();
  for (const auto& market : root.items()) {
    const auto& bundles_val = market.value().at("bundle");
    if (bundles_val.contains("bundle_id")) {  // Market has one bundle
      EXPECT_EQ(&bundle_value(root, market.key(), ""), &bundles_val);
    } else {  // Market has multiple bundles
      for (const auto& bundle : bundles_val.items()) {
        EXPECT_EQ(&bundle_value(root, market.key(), bundle.key()),
                  &bundle.value());
      }
    }
  }
}

TEST(ConfigTest, BundleValueByExpiry) {
  const auto& root = test_markets();
  EXPECT_EQ(bundle_value(root, "Multi", "0716").at("bundle_id"), 11);
  // A market with one bundle ignores the expiry
  EXPECT_EQ(bundle_value(root, "Single", "0716").at("bundle_id"), 20);
  EXPECT_THROW(bundle_value(root, "Multi", "0816"), std::invalid_argument);
  EXPECT_THROW(bundle_value(root, "Missing", "0716"), std::invalid_argument);
}

TEST(ConfigTest, AssetValue) {
  const auto& root = read_markets_json();
  for (const auto& market : root.items()) {
    const auto& bundles_val = market.value().at("bundle");
    if (bundles_val.contains("bundle_id")) {  // Market has one bundle
      for (const auto& asset : bundles_val.at("assets").items()) {
        EXPECT_EQ(&asset_value(root, market.key(), asset.key()),
                  &asset.value());
      }
    } else {  // Market has multiple bundles
      for (const auto& bundle_val : bundles_val) {
        for (const auto& asset : bundle_val.at("assets").items()) {
          EXPECT_EQ(&asset_value(root, market.key(), asset.key()),
                    &asset.value());
        }
      }
    }
  }
}

// Whether each lookup accepts Root as its JSON root
template <typename Root>
concept MarketAccepts = requires(Root&& root, const std::string& name) {
  market_value(std::forward<Root>(root), name);
};
template <typename Root>
concept BundleAccepts = requires(Root&& root, const std::string& name) {
  bundle_value(std::forward<Root>(root), name, name);
};
template <typename Root>
concept AssetAccepts = requires(Root&& root, const std::string& name) {
  asset_value(std::forward<Root>(root), name, name);
};

template <typename Root>
void ExpectLookups(bool accepted) {
  EXPECT_EQ(MarketAccepts<Root>, accepted);
  EXPECT_EQ(BundleAccepts<Root>, accepted);
  EXPECT_EQ(AssetAccepts<Root>, accepted);
}

TEST(ConfigTest, LookupRejectsTemporaryRoot) {
  // The result refers into the root, so a temporary root would dangle
  ExpectLookups<const nlohmann::json&>(true);
  ExpectLookups<nlohmann::json&>(true);
  ExpectLookups<nlohmann::json>(false);
  ExpectLookups<const nlohmann::json>(false);
}

TEST(ConfigTest, AssetExpiry) {
  EXPECT_EQ(asset_expiry("FRup0616"), "0616");
  EXPECT_EQ(asset_expiry("0616"), "0616");
  EXPECT_EQ(asset_expiry("Yes"), "");
  EXPECT_EQ(asset_expiry(""), "");
}

TEST(ConfigTest, AssetValueByName) {
  const auto& root = test_markets();
  // The last four characters pick the bundle
  EXPECT_EQ(asset_value(root, "Multi", "Up0716").at("id"), 110);
  EXPECT_EQ(asset_value(root, "Single", "Yes").at("id"), 200);
  EXPECT_THROW(asset_value(root, "Multi", "Down0716"), std::invalid_argument);
  EXPECT_THROW(asset_value(root, "Multi", "Up"), std::invalid_argument);
  EXPECT_THROW(asset_value(root, "Single", "No"), std::invalid_argument);
}

TEST(ConfigTest, DateStringToPtime) {
  // from_simple_string will return not_a_date_time date with empty string
  const auto nad = from_simple_string("");
  EXPECT_EQ(nad, boost::gregorian::date(boost::gregorian::not_a_date_time));

  // from_simple_string should match boost::gregorian when a correctly formatted
  // date time is provided
  const auto s("2016-12-31");
  EXPECT_EQ(from_simple_string(s), boost::gregorian::from_simple_string(s));
}

TEST(ConfigTest, ActiveBundle) {
  const auto& root = read_markets_json();
  const auto dt = boost::gregorian::from_simple_string("2016-12-30");

  // Copies, not references: GCC's -Wdangling-reference misfires when a call
  // returning a reference has temporary arguments (the string literals)
  const auto jan17_bundle_val = bundle_value(root, "FedPolicyB", "0117");
  EXPECT_TRUE(active_bundle(jan17_bundle_val, dt));

  const auto dec16_bundle_val = bundle_value(root, "FedPolicyB", "1216");
  EXPECT_FALSE(active_bundle(dec16_bundle_val, dt));
}

TEST(ConfigTest, ActiveBundleWithoutDates) {
  // No open or liquidation date: active on any date
  const auto bundle = nlohmann::json::object();
  EXPECT_TRUE(active_bundle(bundle, boost::gregorian::date(2016, 12, 30)));
  // Null dates count as missing
  const nlohmann::json nulls = {{"open_date", nullptr},
                                {"liquidation_date", nullptr}};
  EXPECT_TRUE(active_bundle(nulls, boost::gregorian::date(2016, 12, 30)));
}

TEST(ConfigTest, ActiveBundleDateBounds) {
  const nlohmann::json bundle = {{"open_date", "2016-06-01"},
                                 {"liquidation_date", "2016-07-22"}};
  using boost::gregorian::date;
  EXPECT_FALSE(active_bundle(bundle, date(2016, 5, 31)));
  EXPECT_TRUE(active_bundle(bundle, date(2016, 6, 1)));
  EXPECT_TRUE(active_bundle(bundle, date(2016, 7, 22)));
  EXPECT_FALSE(active_bundle(bundle, date(2016, 7, 23)));
}

TEST(ConfigTest, EmptyDateString) {
  EXPECT_TRUE(from_simple_string("").is_not_a_date());
}

}  // namespace iem
