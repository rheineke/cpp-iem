// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef IEM_SRC_IEM_CONTRACT_HPP_
#define IEM_SRC_IEM_CONTRACT_HPP_

#include <functional>
#include <iosfwd>
#include <string>
#include <utility>

#include <boost/date_time/date_defs.hpp>

#include "json/json.h"

namespace iem {

using MarketId = uint_fast16_t;
using AssetId = uint_fast16_t;
using BundleId = uint_fast16_t;

using Month = boost::date_time::months_of_year;
using Year = uint_fast8_t;
using MonthYear = std::pair<Month, Year>;

std::ostream& operator<<(std::ostream& os, const MonthYear& month_year);

constexpr auto kDefaultFilename = "conf/markets.json";

const Json::Value& read_markets_json(char const* filename=kDefaultFilename);

class Market final {
 public:
  Market(const std::string& name, char const* filename=kDefaultFilename);

  inline const std::string name() const noexcept { return name_; }
  inline MarketId value() const noexcept { return value_; }
 private:
  std::string name_;
  MarketId value_;
};

std::ostream& operator<<(std::ostream& os, const Market& mkt);

class ContractBundle final {
 public:
  ContractBundle(const std::string& market_name, const MonthYear& expiration);

  inline Market market() const noexcept { return market_; }
  inline const MonthYear expiration() const noexcept { return expiration_; }
  inline BundleId bundle_id() const noexcept { return bundle_id_; }

 private:
  Market market_;
  MonthYear expiration_;
  BundleId bundle_id_;
};

std::ostream& operator<<(std::ostream& os, const ContractBundle& cb);

class Contract final {
 public:
  explicit Contract(const std::string& contract_name);

  inline Market market() const noexcept { return market_; }
  inline AssetId asset_id() const noexcept { return asset_id_; }
 private:
  Market market_;
  AssetId asset_id_;
};

std::ostream& operator<<(std::ostream& os, const Contract& c);

const std::string _read_market_name(const std::string &contract_name);

}  // namespace iem

namespace std {

template<>
struct hash<iem::Market> {
  decltype(auto) operator()(const iem::Market& arg) const {
    std::hash<iem::MarketId> hasher;
    return hasher(arg.value());
  }
};

}  // namespace std

#endif
