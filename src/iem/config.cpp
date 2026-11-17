// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "config.hpp"

#include <fstream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>

namespace iem {

namespace {

nlohmann::json parse_file(char const* filename) {
  std::ifstream file(filename);
  if (!file) {
    throw std::invalid_argument("Cannot read " + std::string(filename));
  }
  return nlohmann::json::parse(file);
}

// Returns object[key], or throws std::invalid_argument naming the missing key:
// UnknownContract when the key is a bundle the file doesn't list yet.
const nlohmann::json& member(const nlohmann::json& object,
                             const std::string& key, const std::string& what,
                             const bool unknown_bundle = false) {
  const auto it = object.find(key);
  if (it == object.end()) {
    const auto message = "Unknown " + what + ": " + key;
    if (unknown_bundle) {
      throw UnknownContract(message);
    }
    throw std::invalid_argument(message);
  }
  return *it;
}

// Returns the date stored at key, or not_a_date_time if the key is missing
boost::gregorian::date date_member(const nlohmann::json& object,
                                   const std::string& key) {
  const auto it = object.find(key);
  if (it == object.end() || it->is_null()) {
    return boost::gregorian::date(boost::gregorian::not_a_date_time);
  }
  return from_simple_string(it->get<std::string>());
}

}  // namespace

const nlohmann::json& read_markets_json(char const* filename) {
  // One parsed file per name, so a second file isn't silently ignored. A
  // throw leaves it unset, so the next call tries again.
  static std::mutex mutex;
  static std::map<std::string, nlohmann::json> roots;
  const std::lock_guard<std::mutex> lock(mutex);
  const auto it = roots.find(filename);
  if (it != roots.end()) {
    return it->second;
  }
  return roots.emplace(filename, parse_file(filename)).first->second;
}

const nlohmann::json& market_value(const nlohmann::json& json_root,
                                   const std::string& market_name) {
  return member(json_root, market_name, "market name");
}

const nlohmann::json& bundle_value(const nlohmann::json& json_root,
                                   const std::string& market_name,
                                   const std::string& expiry_date_str) {
  const auto& bundle_val = market_value(json_root, market_name).at("bundle");
  if (bundle_val.contains("bundle_id")) {  // Market has one bundle
    return bundle_val;
  }
  return member(bundle_val, expiry_date_str, "bundle expiration", true);
}

std::string asset_expiry(const std::string& asset_name) {
  constexpr std::size_t kExpiryLength = 4;  // MMYY
  if (asset_name.size() < kExpiryLength) {
    return std::string();
  }
  return asset_name.substr(asset_name.size() - kExpiryLength);
}

const nlohmann::json& asset_value(const nlohmann::json& json_root,
                                  const std::string& market_name,
                                  const std::string& asset_name) {
  const auto expiry_str = asset_expiry(asset_name);
  const auto& bundle_val = bundle_value(json_root, market_name, expiry_str);
  return member(bundle_val.at("assets"), asset_name, "asset name");
}

boost::gregorian::date from_simple_string(const std::string& s) {
  if (s.empty()) {
    return boost::gregorian::date(boost::gregorian::not_a_date_time);
  }
  return boost::gregorian::from_simple_string(s);
}

bool active_bundle(const nlohmann::json& bundle_value,
                   const boost::gregorian::date& current_date) {
  // Date is after open date
  const auto open_date = date_member(bundle_value, "open_date");
  const bool opened_after =
      open_date.is_not_a_date() || (open_date <= current_date);

  // Date is before liquidation date
  const auto liq_date = date_member(bundle_value, "liquidation_date");
  const bool liquidated_after =
      liq_date.is_not_a_date() || (liq_date >= current_date);

  return opened_after && liquidated_after;
}

}  // namespace iem
