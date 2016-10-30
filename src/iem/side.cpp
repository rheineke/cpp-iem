// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/side.hpp"

#include <sstream>

namespace iem {

std::ostream& operator<<(std::ostream& os, const Side& side) {
  return os << ((side == Side::BUY) ? "buy" : "SELL");
}

}  // namespace iem
