// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_CONTRACT_HPP_
#define CPPIEM_IEM_CONTRACT_HPP_

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
// Bundle expiry
using MonthYear = std::pair<Month, Year>;

std::ostream& operator<<(std::ostream& os, const MonthYear& month_year);

// IEM market. A market contains zero, one or many active (not expired) bundles.
class Market final {
 public:
  // Constructor requires a valid market name, and optionally a JSON config file
  // that defines all valid markets and their attributes
  Market(const std::string& name, char const* filename=kDefaultFilename);

  inline const std::string name() const noexcept { return name_; }
  // Unique numeric id provided by IEM
  inline MarketId value() const noexcept { return value_; }
 private:
  std::string name_;
  MarketId value_;
};

std::ostream& operator<<(std::ostream& os, const Market& mkt);

// A bundle of related contracts in a given market. Each market contains zero,
// one or more active (not expired) bundles.
class ContractBundle final {
 public:
  // Constructor requires a market name and an expiration month/year pair
  ContractBundle(const std::string& market_name, const MonthYear& expiration);

  inline Market market() const noexcept { return market_; }
  inline const MonthYear expiration() const noexcept { return expiration_; }
  // Unique numeric id provided by IEM for each bundle
  inline BundleId bundle_id() const noexcept { return bundle_id_; }

 private:
  Market market_;
  MonthYear expiration_;
  BundleId bundle_id_;
};

std::ostream& operator<<(std::ostream& os, const ContractBundle& cb);

// An IEM contract. To date, all IEM contracts are binary and therefore two or
// more contracts are required to define all possibilities in a given bundle.
class Contract final {
 public:
  // Constructor requires market name and contract name
  Contract(const std::string& market_name, const std::string& contract_name);

  inline Market market() const noexcept { return market_; }
  // Unique numeric id provided by IEM
  inline AssetId asset_id() const noexcept { return asset_id_; }
  // Second unique numeric id provided by IEM
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

#endif  // CPPIEM_IEM_CONTRACT_HPP_
