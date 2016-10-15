// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/numorderbookupdatesignal.hpp"

namespace iem::signal {

NumOrderBookUpdateSignal::NumOrderBookUpdateSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

}  // namespace iem::signal
