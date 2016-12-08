// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_SIDE_HPP_
#define CPPIEM_IEM_SIDE_HPP_

#include <cstdint>
#include <iosfwd>

namespace iem {

enum class Side : int_fast8_t { BUY = 0, SELL = 1 } __attribute__((packed));

constexpr Side static otherSide(const Side& side) noexcept {
  return (side == Side::BUY) ? Side::SELL : Side::BUY;
}

std::ostream& operator<<(std::ostream& os, const Side& s);

}  // namespace iem

#endif  // CPPIEM_IEM_SIDE_HPP_
