// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_SIDE_HPP_
#define CPPIEM_SRC_IEM_SIDE_HPP_

#include <cstdint>
#include <iosfwd>

namespace iem {

enum class Side : int_fast8_t { buy = 0, sell = 1 } __attribute__((packed));

constexpr char sideChar(const Side& side) noexcept {
  return (side == Side::buy) ? 'B' : 'A';
}

constexpr Side static otherSide(const Side& side) noexcept {
  return (side == Side::buy) ? Side::sell : Side::buy;
}

std::ostream& operator<<(std::ostream& os, const Side& s);

}  // namespace iem

#endif
