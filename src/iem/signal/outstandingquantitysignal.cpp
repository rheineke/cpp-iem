// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/outstandingquantitysignal.hpp"

#include <limits>
#include <numeric>

namespace iem::signal {

OutstandingQuantitySignal::OutstandingQuantitySignal(
    const OrderBook& orderbook):
    orderbook_(orderbook) {
}

Value OutstandingQuantitySignal::value(const Side& side, const Price&) const {
  const auto& orders = orderbook_.orders(side);
  const auto acc = std::accumulate(orders.cbegin(),
                                   orders.cend(),
                                   0,
                                   [](const auto& partial, const auto& s) {
                                     return partial + s.quantity();
                                   });
  return static_cast<Value>(acc);
}

}  // namespace iem::signal
