// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/numcontractsatpricesignal.hpp"

#include <numeric>

namespace iem::signal {

NumContractsAtPriceSignal::NumContractsAtPriceSignal(
    const OrderBook& orderbook):
    orderbook_(orderbook) {
}

Value NumContractsAtPriceSignal::value(const Side& side,
                                       const Price& price) const {
  const auto& os = orderbook_.orders(side);
  return std::accumulate(
      os.cbegin(),
      os.cend(),
      0,
      [&price](const iem::Quantity& partial, const Single& o) {
        return (o.price_time_limit().price() == price) ?
            (partial + o.quantity()) : partial;
      });
}

}  // namespace iem::signal
