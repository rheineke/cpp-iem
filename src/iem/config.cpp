// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "config.hpp"

#include <fstream>
#include <mutex>

#include <boost/filesystem.hpp>

namespace iem {

using std::operator ""s;

std::shared_ptr<const Json::Value> json_root;
std::once_flag json_flag;

const void init_json_root(char const* filename) {
//  namespace fs = std::experimental::filesystem;
  namespace fs = boost::filesystem;
  fs::path p(filename);
  if (!fs::exists(p)) {
    const auto msg = "Path does not exist: "s + std::string(filename);
    throw std::invalid_argument(msg);
  }
  // Open file in binary mode
  auto p_root = new Json::Value;
  if (p_root->isNull()) {
    std::ifstream config_doc(filename, std::ifstream::binary);
    config_doc >> *p_root;
  }

  json_root.reset(p_root);
}

const Json::Value& read_markets_json(char const* filename) {
  std::call_once(json_flag, init_json_root, filename);
  return json_root.operator*();
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
  const auto& bundle_val = mkt_value["bundle"];

  if (!bundle_val["bundle_id"].isNull()) {  // Market has one bundle
    return bundle_val;
  }

  const auto& expiry_bundle_value = bundle_val[expiry_date_str];
  if (expiry_bundle_value.isNull()) {
    throw std::invalid_argument("Unknown bundle expiration " + expiry_date_str);
  }
  return expiry_bundle_value;
}

const Json::Value asset_value(const Json::Value& json_root,
                              const std::string& market_name,
                              const std::string& asset_name) {
  const auto& expiry_str = asset_name.substr(asset_name.length() - 4);
  const auto& bundle_val = bundle_value(json_root, market_name, expiry_str);
  return bundle_val["assets"][asset_name];
}

boost::gregorian::date from_simple_string(const std::string& s) {
  if (s == "") {
    return boost::gregorian::date(boost::gregorian::not_a_date_time);
  }
  return boost::gregorian::from_simple_string(s);
}

bool active_bundle(const Json::Value& bundle_value,
                   const boost::gregorian::date& current_date) {
  using boost::gregorian::date;
  const date nad(boost::gregorian::not_a_date_time);

  // Date is after open date
  const auto open_date_value = bundle_value["open_date"];

  date open_date(boost::gregorian::not_a_date_time);
  if (!open_date_value.isNull()) {
    open_date = from_simple_string(open_date_value.asString());
  }

  const bool opened_after = (open_date == nad) | (open_date <= current_date);

  // Date is before liquidation date
  const auto liquidation_date_value = bundle_value["liquidation_date"];

  date liq_date(boost::gregorian::not_a_date_time);
  if (!liquidation_date_value.isNull()) {
    liq_date = from_simple_string(liquidation_date_value.asString());
  }

  const bool liquidated_after = (liq_date == nad) | (liq_date >= current_date);

  return opened_after & liquidated_after;
}

}  // namespace iem
