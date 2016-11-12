// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_CONTRACT_HPP_
#define CPPIEM_SRC_IEM_CONTRACT_HPP_

#include <functional>
#include <iosfwd>
#include <string>
#include <utility>

#include <boost/date_time/date_defs.hpp>

#include "iem/config.hpp"

namespace iem {

using MarketId = uint_fast16_t;
using AssetId = uint_fast16_t;
using AssetToMarketId = uint_fast16_t;
using BundleId = uint_fast16_t;

using Month = boost::date_time::months_of_year;
using Year = uint_fast8_t;
// TODO(rheineke): Consider replacing with gregorian::date(year, month, 0?)
using MonthYear = std::pair<Month, Year>;

std::ostream& operator<<(std::ostream& os, const MonthYear& month_year);

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
  explicit Contract(const std::string& market_name,
                    const std::string& contract_name);

  inline Market market() const noexcept { return market_; }
  // TODO(rheineke): Is asset id used in any way?
  inline AssetId asset_id() const noexcept { return asset_id_; }
  inline AssetToMarketId asset_to_market_id() const noexcept {
    return asset_to_market_id_;
  }
 private:
  Market market_;
  AssetId asset_id_;
  AssetToMarketId asset_to_market_id_;
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
