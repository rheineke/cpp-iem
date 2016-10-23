// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include <fstream>
#include <regex>
#include <string>

namespace iem {

std::ostream& operator<<(std::ostream& os, const MonthYear& my) {
  return os << "(" << my.first << ", " << static_cast<int>(my.second) << ")";
}

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

int _read_market_value(const std::string &name, char const *filename) {
  const auto id_value = read_markets_json(filename)[name]["id"];

  if (id_value.isNull()) {
    throw std::invalid_argument("Market name not found");
  }

  return id_value.asInt();
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
  const auto bundles_value = read_markets_json()[market_name]["bundle"];

  Json::Value id_value;
  if (bundles_value.isObject()) {
    id_value = bundles_value[_expiration_string(expiration)];
  } else {
    id_value = bundles_value;
  }

  if (id_value == Json::Value::nullSingleton()) {
    throw std::invalid_argument("Bundle id not found");
  }

  return id_value.asInt();
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

const std::string _read_market_name(const std::string &contract_name) {
  const auto mkts_dict = read_markets_json();

  for (const auto& market_name : mkts_dict.getMemberNames()) {
    const auto& mkt_dict = mkts_dict[market_name];
    const auto asset_value = mkt_dict["assets"][contract_name];
    if (!asset_value.isNull()) {
      return market_name;
    }
  }

  throw std::invalid_argument("Contract name not found");
}

int _read_contract_id(const std::string& contract_name) {
  const auto mkts_dict = read_markets_json();
  for (const auto& market_name : mkts_dict.getMemberNames()) {
    const auto& mkt_dict = mkts_dict[market_name];
    const auto asset_value = mkt_dict["assets"][contract_name];

    if (!asset_value.isNull()) {
      return asset_value["id"].asInt();
    }
  }

  throw std::invalid_argument("Contract name not found");
}

Contract::Contract(const std::string& contract_name):
    market_(_read_market_name(contract_name)),
    asset_id_(_read_contract_id(contract_name)) {

}

std::ostream& operator<<(std::ostream& os, const Contract& c) {
  os << "{\"market\":" << c.market()
     << ", \"asset\":" << c.asset_id() << "}";
  return os;
}

}  // namespace iem
