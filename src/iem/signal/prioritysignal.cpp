// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/prioritysignal.hpp"

namespace iem::signal {

PrioritySignal::PrioritySignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

}  // namespace iem::signal
