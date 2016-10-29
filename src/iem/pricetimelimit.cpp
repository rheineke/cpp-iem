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
  using boost::posix_time::to_simple_string;

  if (price == nanPrice() && expiration == not_a_date_time) {
    return true;
  } else if (price != nanPrice() && expiration != not_a_date_time) {
    return false;
  } else {
    constexpr auto kBufSize = 96;
    char s[kBufSize];
    constexpr auto fst = "Price ";
    std::size_t n = strlen(fst);
    strncpy(s, fst, n);
    n += snprintf_price(s + n, kBufSize - n, price);
    constexpr auto fmt = " and expiration %s is not valid combination";
    snprintf(s + n, kBufSize - n, fmt, "???"); // to_simple_string(expiration).c_str()
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
  const auto expiry_dt_value = read_markets_json()[market_name]["expiry_date"];
  if (expiry_dt_value.isNull()) {
    throw std::invalid_argument("Market expiry_date name not found");
  }

  std::string expiry_date_str;
  if (expiry_dt_value.isObject()) {
    // Assume that asset_name contains expiry_date key
    // TODO(rheineke): More robust way of splitting asset name?
    const auto expiry_date_key = asset_name.substr(asset_name.size() - 4);
    expiry_date_str = expiry_dt_value[expiry_date_key].asString();
  } else {
    expiry_date_str = expiry_dt_value.asString();
  }
  return boost::gregorian::from_simple_string(expiry_date_str);
}

boost::posix_time::ptime read_expiration_ptime(const std::string& market_name,
                                               const std::string& asset_name) {
  const auto expiry_dt = _read_expiration_date(market_name, asset_name);
  const auto t(boost::posix_time::time_duration(23, 59, 59));
  return boost::posix_time::ptime(expiry_dt, t);
}

std::ostream& operator<<(std::ostream& os, const PriceTimeLimit& ptl) {
  if (ptl.ioc()) {
    return os << "IOC";
  } else {
    os << "GoodTill{price=" << ptl.price() <<
        ", expiration=" << ptl.expiration() << '}';
    return os;
  }
}

}  // namespace iem
