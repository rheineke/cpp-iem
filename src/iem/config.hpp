// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_CONFIG_HPP_
#define CPPIEM_IEM_CONFIG_HPP_

#include <stdexcept>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "nlohmann/json.hpp"

namespace iem {

// A market, contract or bundle the markets file doesn't list.
class UnknownContract : public std::invalid_argument {
 public:
  using std::invalid_argument::invalid_argument;
};

constexpr auto kDefaultFilename = "conf/markets.json";

// Returns the JSON document at (relative) path filename, which must be strict
// JSON (no comments). Each file is read once; later calls with the same
// filename return the same document.
const nlohmann::json& read_markets_json(
    char const* filename = kDefaultFilename);

// The lookups below return references into json_root, so json_root must
// outlive them; the deleted overloads reject a temporary root.

// Returns the market node matching market_name. If market_name is not found,
// then an exception is thrown.
const nlohmann::json& market_value(const nlohmann::json& json_root,
                                   const std::string& market_name);
const nlohmann::json& market_value(const nlohmann::json&& json_root,
                                   const std::string& market_name) = delete;

// Returns the bundle node matching market_name and expiry_date_str. If the
// market contains only one expiry, then expiry_date_str is not used and can be
// any value. If a bundle node matching both inputs is not found, then an
// exception is thrown.
const nlohmann::json& bundle_value(const nlohmann::json& json_root,
                                   const std::string& market_name,
                                   const std::string& expiry_date_str);
const nlohmann::json& bundle_value(const nlohmann::json&& json_root,
                                   const std::string& market_name,
                                   const std::string& expiry_date_str) = delete;

// Returns the bundle expiry that ends an asset name, such as 0616 for
// FRup0616, or an empty string if asset_name is shorter than an expiry.
std::string asset_expiry(const std::string& asset_name);

// Returns the asset node matching market_name and asset_name. If asset_name
// matching both inputs is not found, then an exception is thrown.
const nlohmann::json& asset_value(const nlohmann::json& json_root,
                                  const std::string& market_name,
                                  const std::string& asset_name);
const nlohmann::json& asset_value(const nlohmann::json&& json_root,
                                  const std::string& market_name,
                                  const std::string& asset_name) = delete;

bool active_bundle(const nlohmann::json& bundle_value,
                   const boost::gregorian::date& current_date);

boost::gregorian::date from_simple_string(const std::string& s);

}  // namespace iem

#endif  // CPPIEM_IEM_CONFIG_HPP_
