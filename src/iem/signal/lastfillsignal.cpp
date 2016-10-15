// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/lastfillsignal.hpp"

#include <algorithm>

namespace iem {
namespace signal {

LastFillSignal::LastFillSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

Value LastFillSignal::value(const Side& side, const Price&) const {
  const auto& eos = orderbook_.executedOrders();
  // Assumes executed orders are sorted by time in descending order
  const auto it = std::find_if(
      eos.cbegin(),
      eos.cend(),
      [&side](const ExecutedOrder& eo) {
        return eo.side() == side && eo.price() != nanPrice();
      });

  return (it != eos.end()) ?
      it->price().ticks() : extreme_tick(otherSide(it->side()));
}

}  // namespace signal
}  // namespace iem
