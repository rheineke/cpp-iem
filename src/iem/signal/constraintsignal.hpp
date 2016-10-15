// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_constraintsignal_HPP_
#define iem_src_iem_signal_constraintsignal_HPP_

#include <array>
#include <cmath>
#include <utility>

#include "iem/signal.hpp"

namespace iem {

using SidedValue = std::pair<Value, Value>;

namespace signal {

template<size_t N>
using ra_t = std::tuple<Signal*,
                        SidedBinaryFunction,
                        std::array<SidedValue, N> >;

template<size_t N, size_t M>
class ConstraintSignal final : public Signal {
 public:
  explicit ConstraintSignal(const std::array<ra_t<N>, M>& row_arguments):
      row_arguments_(row_arguments),
      satisfied_columns_(0),
      set_(static_cast<int>(std::pow(2, N) - 1)) {
  }

  Value value(const Side& side, const Price& price) const final {
    if (row_arguments_.empty()) {  // Bitset calculation assumes nonzero vector
      return 0.;
    }

    satisfied_columns_ = set_;
    for (const auto& row : row_arguments_) {
      const int value = static_cast<int>(row_value(row, side, price));
      satisfied_columns_ &= value;
      if (satisfied_columns_ == 0) {
        break;
      }
    }

    return satisfied_columns_;
  }

 private:
  Value row_value(const ra_t<N>& row,
                  const Side& side,
                  const Price& price) const {
    const auto value = std::get<0>(row)->value(side, price);
    satisfied_indices_ = 0;
    mask_ = 1;
    for (const auto& t : std::get<2>(row)) {
      const Value side_t = (side == Side::buy) ? t.first : t.second;
      satisfied_indices_ |= ((std::get<1>(row)(side, value, side_t)) ?
                             mask_ : 0);
      mask_ <<= 1;
    }

    return satisfied_indices_;
  }

  std::array<ra_t<N>, M> row_arguments_;
  mutable int satisfied_indices_;
  mutable int mask_;

  mutable int satisfied_columns_;
  int set_;
};

}  // namespace signal
}  // namespace iem

#endif
