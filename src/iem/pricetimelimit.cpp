// Copyright 2015 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

#include <string>

#include "iem/contract.hpp"

#ifdef DEBUG
#include "debug.hpp"
#endif

namespace iem {

const boost::posix_time::ptime PriceTimeLimit::expiration() const {
  return expiration_;
}

bool PriceTimeLimit::ioc() const noexcept { return ioc_; }

PriceTimeLimit::PriceTimeLimit(const Price& price,
                               const boost::posix_time::ptime& expiration):
    price_(price), expiration_(expiration), ioc_(is_ioc(price, expiration)) {
#ifdef DEBUG
  debug::log<PriceTimeLimit>(this, "price time limit ctor");
#endif
}
PriceTimeLimit::PriceTimeLimit():
    PriceTimeLimit(Price(), boost::date_time::not_a_date_time) { }

bool PriceTimeLimit::is_ioc(const Price& price,
                            const boost::posix_time::ptime& expiration) {
  using boost::date_time::not_a_date_time;
  using boost::posix_time::to_iso_extended_string;

  if (price == nanPrice() && expiration == not_a_date_time) {
    return true;
  } else if (price != nanPrice()) {
    return false;
  } else {
    constexpr auto kBufSize = 96;
    char s[kBufSize];
    constexpr auto fst = "Price ";
    std::size_t n = strlen(fst);
    strncpy(s, fst, n);
    n += snprintf_price(s + n, kBufSize - n, price);
    constexpr auto fmt = " and expiration %s is not valid combination";
    snprintf(s + n, kBufSize - n, fmt,
             to_iso_extended_string(expiration).c_str());
    throw std::invalid_argument(s);
  }
}

// int snprintf_pricetimelimit(char* const s,
//                             std::size_t n,
//                             const PriceTimeLimit& ptl) {
//   if (ptl.ioc()) {
//     return strncpy(s, "IOC", n);
//   } else {
//     return snprintf(s, n, "GoodTill{price=%s, expiration=%s}", "Foo", "Bar");
//   }
// }

boost::gregorian::date _read_expiration_date(const std::string& market_name,
                                             const std::string& asset_name) {
  const auto& json_root = read_markets_json();
  const auto& expiry_str = asset_name.substr(asset_name.length() - 4);
  const auto& bundle_val = bundle_value(json_root, market_name, expiry_str);
  const auto& expiry_dt_str = bundle_val["expiry_date"].asString();
  return boost::gregorian::from_simple_string(expiry_dt_str);
}

boost::posix_time::ptime read_expiration_ptime(const std::string& market_name,
                                               const std::string& asset_name) {
  const auto expiry_dt = _read_expiration_date(market_name, asset_name);
  const auto t(boost::posix_time::time_duration(23, 59, 59));
  return boost::posix_time::ptime(expiry_dt, t);
}

const std::string to_string(const boost::posix_time::ptime& expiration_ptime) {
  if (expiration_ptime == boost::posix_time::not_a_date_time) {
    return "No expiration";
  }
  std::ostringstream ss;
  // Locale object owns the facet instance so no memory leak here
  auto* p_tod_facet(new boost::posix_time::time_facet("%Y-%m-%d %I:%M %p"));
  ss.imbue(std::locale(ss.getloc(), p_tod_facet));
  ss << expiration_ptime;
  return ss.str();
}

std::ostream& operator<<(std::ostream& os, const PriceTimeLimit& ptl) {
  if (ptl.ioc()) {
    return os << "IOC";
  } else {
    os << "GoodTill{price=" << ptl.price() <<
        ", expiration=" << to_string(ptl.expiration()) << '}';
    return os;
  }
}

}  // namespace iem
