// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/toppricesignal.hpp"

namespace iem::signal {

TopPriceSignal::TopPriceSignal(const OrderBook& orderbook):
    orderbook_(orderbook) {
}

}  // namespace iem::signal
