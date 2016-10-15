// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/ticksoutsidesignal.hpp"

namespace iem::signal {

TicksOutsideSignal::TicksOutsideSignal(const Signal& signal):
    signal_(signal) {
}

Value TicksOutsideSignal::value(const Side& side, const Price& price) const {
  return ticks_outside_of(side,
                          price.ticks(),
                          static_cast<Ticks>(signal_.value(side, price)));
}

}  // namespace iem::signal
