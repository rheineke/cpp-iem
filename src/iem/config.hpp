// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_CONFIG_HPP_
#define CPPIEM_IEM_CONFIG_HPP_

#include "json/json.h"

namespace iem {

constexpr auto kDefaultFilename = "conf/markets.json";

const Json::Value& read_markets_json(char const* filename=kDefaultFilename);

const Json::Value market_value(const Json::Value& json_root,
                               const std::string& market_name);

const Json::Value bundle_value(const Json::Value& json_root,
                               const std::string& market_name,
                               const std::string& expiry_date_str);

const Json::Value asset_value(const Json::Value& json_root,
                              const std::string& market_name,
                              const std::string& asset_name);

}  // namespace iem

#endif  // CPPIEM_IEM_CONFIG_HPP_
