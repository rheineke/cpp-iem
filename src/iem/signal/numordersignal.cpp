// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/numordersignal.hpp"

namespace iem {
namespace signal {

NumOrderSignal::NumOrderSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

}  // namespace signal
}  // namespace iem
