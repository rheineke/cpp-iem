// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "config.hpp"

#include <fstream>
#include <iostream>

namespace iem {

const Json::Value& read_markets_json(char const* filename) {
  // Open file in binary mode
  static Json::Value root;
  // TODO(rheineke): Check filename for existence
  if (root.isNull()) {
    std::ifstream config_doc(filename, std::ifstream::binary);
    config_doc >> root;
  }

  return root;
}

const Json::Value market_value(const Json::Value& json_root,
                               const std::string& market_name) {
  const auto& mkt_value = json_root[market_name];
  if (mkt_value.isNull()) {
    throw std::invalid_argument("Unknown market name" + market_name);
  }
  return mkt_value;
}

const Json::Value bundle_value(const Json::Value& json_root,
                               const std::string& market_name,
                               const std::string& expiry_date_str) {
  const auto& mkt_value = market_value(json_root, market_name);
  auto bundle_value = mkt_value["bundle"];
  if (bundle_value["bundle_id"].isNull()) {
    bundle_value = bundle_value[expiry_date_str];
  }
  if (bundle_value.isNull()) {
    throw std::invalid_argument("Unknown bundle expiration " + expiry_date_str);
  }
  return bundle_value;
}

}  // namespace iem
