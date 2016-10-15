// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/numordersatpricesignal.hpp"

#include <algorithm>

namespace iem {
namespace signal {

NumOrdersAtPriceSignal::NumOrdersAtPriceSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

Value NumOrdersAtPriceSignal::value(const Side& side,
                                    const Price& price) const {
  const auto& os = orderbook_.orders(side);
  return std::count_if(os.cbegin(),
                       os.cend(),
                       [&price](const Single& o) {
                         return o.price_time_limit().price() == price;
                       });
}

}  // namespace signal
}  // namespace iem
