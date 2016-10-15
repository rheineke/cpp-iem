// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/ticksinsidebestsignal.hpp"

namespace iem::signal {

TicksInsideBestSignal::TicksInsideBestSignal(const OrderBook& orderbook):
    orderbook_(orderbook) { }

Value TicksInsideBestSignal::value(const Side& side, const Price& price) const {
  const auto best_price_ticks = orderbook_.best_price(side).ticks();
  return ticks_inside_of(side, price.ticks(), best_price_ticks);
}

}  // namespace iem::signal
