// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_arbitrage_signal_HPP_
#define iem_src_iem_signal_arbitrage_signal_HPP_

#include <array>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"
#include "iem/signal/marketpricesumsignal.hpp"

namespace iem::signal {

template<std::size_t N>
class ArbitrageSignal final : public Signal {
 public:
  explicit ArbitrageSignal(const std::array<OrderBook*, N>& market_orderbooks):
      market_price_sum_signal_(market_orderbooks) {
  }

  Value value(const Side& side, const Price& price) const final {
    return is_inside_of(side,
                        market_price_sum_signal_.value(side, price),
                        static_cast<Value>(1000.));
  }

 private:
  MarketPriceSumSignal<N> market_price_sum_signal_;
};

}  // namespace iem::signal

#endif
