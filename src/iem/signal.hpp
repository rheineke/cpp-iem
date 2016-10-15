// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_HPP_
#define iem_src_iem_signal_HPP_

#include <cmath>
#include <limits>
#include <memory>

#include "iem/price.hpp"

namespace iem {

using Value = float;

using SidedBinaryFunction =
    std::function<bool(const Side&, const Value, const Value)>;

class Signal {
 public:
  Signal() = default;
  explicit Signal(const Signal& signal) = default;
  Signal(Signal&& signal) noexcept = default;
  Signal& operator=(const Signal& signal) = default;
  Signal& operator=(Signal&& signal) noexcept = default;
  virtual ~Signal() = 0;

  virtual Value value(const Side& side, const Price& price) const = 0;
};

struct sided_equal_to {
  bool operator() (const Side&, const Value x, const Value y) const {
    return std::abs(x - y) < std::numeric_limits<Value>::epsilon();
  }
};

struct sided_less {
  bool operator() (const Side&, const Value x, const Value y) const {
    return x < y;
  }
};

struct sided_greater {
  bool operator() (const Side&, const Value x, const Value y) const {
    return x > y;
  }
};

struct sided_greater_equal {
  bool operator() (const Side&, const Value x, const Value y) const {
    return x >= y;
  }
};

struct is_inside {
  bool operator() (const Side& s, const Value x, const Value y) const {
    return is_inside_of(s, x, y);
  }
};

struct is_outside {
  bool operator() (const Side& s, const Value x, const Value y) const {
    return is_outside_of(s, x, y);
  }
};

}  // namespace iem

#endif
