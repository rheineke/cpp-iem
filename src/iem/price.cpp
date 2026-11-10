// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/price.hpp"

#include <iomanip>
#include <stdexcept>
#include <string>

namespace iem {

Price::Price(const Ticks& value_in_ticks) : value_in_ticks_(value_in_ticks) {
  if (value_in_ticks_ < NanTick || value_in_ticks_ > ticks_per_point()) {
    throw std::invalid_argument(
        "Expected price in ticks [" + std::to_string(NanTick) + ", " +
        std::to_string(ticks_per_point()) + "] but received " +
        std::to_string(value_in_ticks));
  }
}

const Price& nanPrice() {
  static const Price np;
  return np;
}

std::ostream& operator<<(std::ostream& os, const Price& p) {
  if (p == nanPrice()) {
    return os << "----";
  } else {
    return os << std::setiosflags(std::ios::fixed) << std::setprecision(3)
              << (static_cast<double>(p.ticks()) /
                  static_cast<double>(p.ticks_per_point()));
  }
}

std::istream& operator>>(std::istream& is, Price& p) {
  char c = '\0';
  if (!(is >> c)) {  // Nothing to read: leave p as it was
    return is;
  }
  if (c == '-') {
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
    const auto px = static_cast<double>(p.ticks()) /
                    static_cast<double>(p.ticks_per_point());
    return snprintf(s, n, "%.3f", px);
  }
}

const std::string to_string(const Price& px) {
  // Longest output is "0.999" or "1.000": 5 characters and a NUL
  constexpr std::size_t kBufferSize = 6;
  const std::size_t len = (px == nanPrice()) ? 5 : kBufferSize;
  char c_str[kBufferSize];
  snprintf_price(c_str, len, px);
  return std::string(c_str, len - 1);
}

}  // namespace iem
