// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/positionsignal.hpp"

namespace iem::signal {

PositionSignal::PositionSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

}  // namespace iem::signal
