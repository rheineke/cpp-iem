// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/contract.hpp"

#include <fstream>
#include <regex>

namespace iem {

std::ostream& operator<<(std::ostream& os, const MonthYear& my) {
  return os << "(" << my.first << ", " << static_cast<int>(my.second) << ")";
}

const Json::Value& read_markets_json(char const* filename) {
  // Open file in binary mode
  static Json::Value root;
  std::ifstream config_doc(filename, std::ifstream::binary);
  config_doc >> root;

  return root;
}

const Json::Value& empty_root() {
  static Json::Value empty_root;
  return empty_root;
}

Market::Market(const std::string& name, char const* filename): name_(name) {
  auto markets_dict = read_markets_json(filename);
  auto market_dict = markets_dict.get(name, empty_root());
  int default_value = 0;
  value_ = market_dict.get("id", default_value).asInt();
  if (value_ == default_value) {
    throw std::invalid_argument("Market id not found");
  }
}

std::ostream& operator<<(std::ostream& os, const Market& mkt) {
  os << "{\"name\":" << mkt.name() << ", \"value\":" << mkt.value() << "}";
  return os;
}

ContractBundle::ContractBundle(const std::string& market_name,
                               const MonthYear& expiration):
    market_(market_name), expiration_(expiration), bundle_id_(-1) {

}

std::ostream& operator<<(std::ostream& os, const ContractBundle& cb) {
  os << "{\"market\":" << static_cast<int>(cb.market().value())
     << ", \"expiration\":" << cb.expiration()
     << ", \"bundle_id\":" << cb.bundle_id() << "}";
  return os;
}

Contract::Contract(const std::string& contract_name):
  market_("TODO"), asset_id_(-1) {

}

std::ostream& operator<<(std::ostream& os, const Contract& c) {
  os << "{\"market\":" << c.market()
     << ", \"asset\":" << c.asset_id() << "}";
  return os;
}

}  // namespace iem
