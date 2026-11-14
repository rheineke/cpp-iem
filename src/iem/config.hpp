// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_CONFIG_HPP_
#define CPPIEM_IEM_CONFIG_HPP_

#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "nlohmann/json.hpp"

namespace iem {

constexpr auto kDefaultFilename = "conf/markets.json";

// Returns the JSON document at (relative) path filename. The first call reads
// the file; later calls return the same document and ignore filename.
const nlohmann::json& read_markets_json(
    char const* filename = kDefaultFilename);

// Returns the market node matching market_name. If market_name is not found,
// then an exception is thrown.
const nlohmann::json& market_value(const nlohmann::json& json_root,
                                   const std::string& market_name);

// Returns the bundle node matching market_name and expiry_date_str. If the
// market contains only one expiry, then expiry_date_str is not used and can be
// any value. If a bundle node matching both inputs is not found, then an
// exception is thrown.
const nlohmann::json& bundle_value(const nlohmann::json& json_root,
                                   const std::string& market_name,
                                   const std::string& expiry_date_str);

// Returns the asset node matching market_name and asset_name. If asset_name
// matching both inputs is not found, then an exception is thrown.
const nlohmann::json& asset_value(const nlohmann::json& json_root,
                                  const std::string& market_name,
                                  const std::string& asset_name);

bool active_bundle(const nlohmann::json& bundle_value,
                   const boost::gregorian::date& current_date);

boost::gregorian::date from_simple_string(const std::string& s);

}  // namespace iem

#endif  // CPPIEM_IEM_CONFIG_HPP_
