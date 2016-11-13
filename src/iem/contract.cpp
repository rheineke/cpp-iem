// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include <regex>
#include <string>

namespace iem {

std::ostream& operator<<(std::ostream& os, const MonthYear& my) {
  return os << "(" << my.first << ", " << static_cast<int>(my.second) << ")";
}

int _read_market_value(const std::string &name, char const *filename) {
  const auto& json_root = read_markets_json(filename);
  return market_value(json_root, name)["id"].asInt();
}

Market::Market(const std::string& name, char const* filename):
    name_(name), value_(_read_market_value(name, filename)) {

}

std::ostream& operator<<(std::ostream& os, const Market& mkt) {
  os << "{\"name\":" << mkt.name() << ", \"value\":" << mkt.value() << "}";
  return os;
}

std::string _expiration_string(const MonthYear& expiration) {
  // Yuck
  const auto mon_str = std::to_string(expiration.first);
  const auto mon_pad = std::string(2 - mon_str.length(), '0');
  const auto yr_str = std::to_string(expiration.second);
  const auto yr_pad = std::string(2 - yr_str.length(), '0');
  return mon_pad + mon_str + yr_pad + yr_str;
}

int _read_bundle_id(const std::string& market_name,
                    const MonthYear& expiration) {
  const auto json_root = read_markets_json();
  const auto expiry_date_str = _expiration_string(expiration);
  const auto bundle_val = bundle_value(json_root, market_name, expiry_date_str);
  return bundle_val["bundle_id"].asInt();
}

ContractBundle::ContractBundle(const std::string& market_name,
                               const MonthYear& expiration):
    market_(market_name),
    expiration_(expiration),
    bundle_id_(_read_bundle_id(market_name, expiration)) {

}

std::ostream& operator<<(std::ostream& os, const ContractBundle& cb) {
  os << "{\"market\":" << static_cast<int>(cb.market().value())
     << ", \"expiration\":" << cb.expiration()
     << ", \"bundle_id\":" << cb.bundle_id() << "}";
  return os;
}

int _read_asset_id(const std::string& market_name,
                   const std::string& contract_name) {
  const auto& json_root = read_markets_json();
  const auto& asset_val = asset_value(json_root, market_name, contract_name);
  return asset_val["id"].asInt();
}

int _read_asset_to_market_id(const std::string& market_name,
                             const std::string& contract_name) {
  const auto& json_root = read_markets_json();
  const auto& asset_val = asset_value(json_root, market_name, contract_name);
  return asset_val["order"].asInt();
}

Contract::Contract(const std::string& market_name,
                   const std::string& contract_name):
    market_(market_name),
    asset_id_(_read_asset_id(market_name, contract_name)),
    asset_to_market_id_(_read_asset_to_market_id(market_name, contract_name)) {

}

std::ostream& operator<<(std::ostream& os, const Contract& c) {
  os << "{\"market\":" << c.market()
     << ", \"asset\":" << c.asset_id()
     << ", \"asset_to_market\":" << c.asset_to_market_id() << "}";
  return os;
}

}  // namespace iem
