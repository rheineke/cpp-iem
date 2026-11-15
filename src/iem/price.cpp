// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/price.hpp"

#include <algorithm>
#include <cctype>
#include <istream>
#include <ostream>
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
  // Through to_string: no locale, and the stream's format is left alone
  return os << to_string(p);
}

namespace {

// "0.512", "1", ".5", "0.50*" (IEM marks priority with '*'), "0.001000":
// whole points and three decimals (more only if zeros), nothing else. Integer
// arithmetic, so no locale and no rounding.
Ticks parse_ticks(const std::string& text) {
  std::string s = text;
  while (!s.empty() && s.back() == '*') {
    s.pop_back();
  }
  const auto dot = s.find('.');
  const std::string whole = s.substr(0, dot);
  std::string frac = (dot == std::string::npos) ? "" : s.substr(dot + 1);
  // IEM writes "0.001000": digits past the third must be zeros
  if (frac.size() > 3 && frac.find_first_not_of('0', 3) == std::string::npos) {
    frac.resize(3);
  }
  const auto digits = [](const std::string& d) {
    return std::all_of(d.begin(), d.end(),
                       [](unsigned char ch) { return std::isdigit(ch); });
  };
  if ((whole.empty() && frac.empty()) || !digits(whole) || !digits(frac) ||
      frac.size() > 3 || whole.size() > 1) {
    throw std::invalid_argument("Not a price: \"" + text + "\"");
  }
  int ticks = whole.empty() ? 0 : (whole[0] - '0') * 1000;
  int scale = 100;
  for (const char ch : frac) {
    ticks += (ch - '0') * scale;
    scale /= 10;
  }
  return static_cast<Ticks>(ticks);
}

}  // namespace

std::istream& operator>>(std::istream& is, Price& p) {
  std::string text;
  if (!(is >> text)) {  // Nothing to read: leave p as it was
    return is;
  }
  std::string rest;
  if (is >> rest) {  // more than one word
    throw std::invalid_argument("Not a price: \"" + text + " " + rest + "\"");
  }
  if (text.find_first_not_of('-') == std::string::npos) {  // "-" or "----"
    p.set(NanTick);
  } else {
    p = Price(parse_ticks(text));  // the constructor checks the range
  }
  return is;
}

int snprintf_price(char* const s, std::size_t n, const Price& p) {
  return snprintf(s, n, "%s", to_string(p).c_str());
}

const std::string to_string(const Price& px) {
  if (px == nanPrice()) {
    return "----";
  }
  // Integer arithmetic: no locale (never "0,500") and no truncated negatives
  const long ticks = px.ticks();
  const long per_point = px.ticks_per_point();
  const long magnitude = (ticks < 0) ? -ticks : ticks;
  std::string frac = std::to_string(magnitude % per_point);
  frac.insert(0, 3 - frac.size(), '0');
  return std::string(ticks < 0 ? "-" : "") +
         std::to_string(magnitude / per_point) + "." + frac;
}

}  // namespace iem
