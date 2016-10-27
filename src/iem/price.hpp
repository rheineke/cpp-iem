// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_PRICE_HPP_
#define CPPIEM_SRC_IEM_PRICE_HPP_

#include <algorithm>
#include <functional>
#include <iosfwd>

#include "iem/side.hpp"

namespace iem {

using Ticks = int_fast16_t;

constexpr Ticks static move_outside(const Side& side,
                                    const Ticks ticks,
                                    const Ticks tick_diff) noexcept {
  return (side == Side::buy) ? (ticks - tick_diff) : (ticks + tick_diff);
}

constexpr Ticks static move_outside(const Side& side,
                                    const Ticks ticks) noexcept {
  return move_outside(side, ticks, 1);
}

constexpr Ticks static move_inside(const Side& side,
                                   const Ticks ticks,
                                   const Ticks tick_diff) noexcept {
  return (side == Side::buy) ? (ticks + tick_diff) : (ticks - tick_diff);
}

constexpr Ticks static move_inside(const Side& side,
                                   const Ticks ticks) noexcept {
  return move_inside(side, ticks, 1);
}

template<typename T>
constexpr Ticks static most_inside(const Side& side,
                                   const T& lhs,
                                   const T& rhs) {
  return (side == Side::buy) ?
      static_cast<Ticks>(std::max(lhs, rhs)):
      static_cast<Ticks>(std::min(lhs, rhs));
}

template<typename T>
constexpr Ticks static most_outside(const Side& side,
                                    const T& lhs,
                                    const T& rhs) {
  return (side == Side::buy) ? std::min(lhs, rhs) : std::max(lhs, rhs);
}

constexpr Ticks static extreme_tick(const Side& side) noexcept {
  return (side == Side::buy) ? 1 : 999;
}

constexpr Ticks ticks_inside_of(const Side& side,
                                const Ticks lhs,
                                const Ticks rhs) noexcept {
  return (side == Side::buy) ? lhs - rhs : rhs - lhs;
}

constexpr Ticks ticks_outside_of(const Side& side,
                                 const Ticks lhs,
                                 const Ticks rhs) noexcept {
  return (side == Side::buy) ? rhs - lhs : lhs - rhs;
}

template<typename T>
constexpr bool is_inside_of(const Side& side,
                            const T& lhs,
                            const T& rhs) noexcept {
  return (side == Side::buy) ? lhs > rhs : lhs < rhs;
}

template<typename T>
constexpr bool is_outside_of(const Side& side,
                             const T& lhs,
                             const T& rhs) noexcept {
  return (side == Side::buy) ? lhs < rhs : lhs > rhs;
}

// Carefully chosen so that even one nan will make a sum negative but not
// overflow
// TODO(rheineke): kMaxMarketContracts can be calculated from strategy::
static constexpr int kMaxMarketContracts = 5;
static constexpr Ticks NanTick = -((kMaxMarketContracts - 1) * 1000 + 1);

class Price {
 public:
  explicit Price(const Ticks& value_in_ticks = NanTick);
  // Price(const Price&) = delete;
  // Price& operator=(const Price&) = delete;

  inline Ticks ticks() const noexcept { return value_in_ticks_; }
  inline Ticks ticks_per_point() const noexcept { return ticks_per_point_; }
  inline void set(const Ticks ticks) noexcept { value_in_ticks_ = ticks; }

 private:
  Ticks value_in_ticks_;
  Ticks ticks_per_point_ = 1000;
};

const Price& nanPrice();

std::ostream& operator<<(std::ostream& os, const Price& p);

std::istream& operator>>(std::istream& is, Price& p);

int snprintf_price(char* const s, std::size_t n, const Price& px);

const std::string to_string(const Price& px);

inline bool operator==(const Price& lhs, const Price& rhs) {
  return lhs.ticks() == rhs.ticks();
}

inline bool operator!=(const Price& lhs, const Price& rhs) {
  return !operator==(lhs, rhs);
}

inline bool operator<(const Price& lhs, const Price& rhs) {
  return lhs.ticks() < rhs.ticks();
}

inline bool operator>(const Price& lhs, const Price& rhs) {
  return operator<(rhs, lhs);
}

inline bool operator<=(const Price& lhs, const Price& rhs) {
  return !operator>(lhs, rhs);
}

inline bool operator>=(const Price& lhs, const Price& rhs) {
  return !operator<(lhs, rhs);
}

}  // namespace iem

#endif
