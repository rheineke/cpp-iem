// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/price.hpp"

#include <iomanip>
#include <string>

namespace iem {

Price::Price(const Ticks& value_in_ticks):
    value_in_ticks_(value_in_ticks) {
  if (value_in_ticks_ < NanTick || value_in_ticks_ > ticks_per_point()) {
    constexpr auto kFmt = "Expected price in ticks [%d, %d] but received %d";
    constexpr auto kBufferSize = 64;
    char buffer[kBufferSize];
    snprintf(buffer,
             kBufferSize,
             kFmt,
             NanTick,
             ticks_per_point(),
             value_in_ticks);
    throw std::invalid_argument(buffer);
  }
}

const Price& nanPrice()  {
  static const Price np;
  return np;
}

std::ostream& operator<<(std::ostream& os, const Price& p) {
  if (p == nanPrice()) {
    return os << "----";
  } else {
    return os << std::setiosflags(std::ios::fixed)
              << std::setprecision(3)
              << (p.ticks() / static_cast<double>(p.ticks_per_point()));
  }
}

std::istream& operator>>(std::istream& is, Price& p) {
  char c;
  if (is >> c && c == '-') {
    p.set(NanTick);
  } else {
    std::string px_str(1, c);
    while (is.get(c)) {
      px_str += c;
    }
    p.set(static_cast<Ticks>(std::stod(px_str) * 1000));
  }
  return is;
}

int snprintf_price(char* const s, std::size_t n, const Price& p) {
  if (p == nanPrice()) {
    return snprintf(s, n, "----");
  } else {
    const auto px = p.ticks() / static_cast<double>(p.ticks_per_point());
    return snprintf(s, n, "%.3f", px);
  }
}

const std::string to_string(const Price& px) {
  constexpr std::size_t kLen = 6;
  char c_str[kLen];
  snprintf_price(c_str, kLen, px);
  return std::string(c_str, kLen-1);
}

}  // namespace iem
