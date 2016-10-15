// Copyright 2015 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

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

std::ostream& operator<<(std::ostream& os, const PriceTimeLimit& ptl) {
  if (ptl.ioc()) {
    return os << "IOC";
  } else {
    os << "GoodTill{price=" << ptl.price() <<
        ", expiration=" << ptl.expiration() << '}';
    return os;
  }
}

} // namespace iem
