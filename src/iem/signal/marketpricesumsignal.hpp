// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_marketpricesumsignal_HPP_
#define iem_src_iem_signal_marketpricesumsignal_HPP_

#include <array>
#include <limits>
#include <numeric>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

template<size_t N>
class MarketPriceSumSignal final : public Signal {
 public:
  explicit MarketPriceSumSignal(
      const std::array<OrderBook*, N>& market_orderbooks):
      market_orderbooks_(market_orderbooks) {
  }

  Value value(const Side& side, const Price&) const final {
    const auto sum = std::accumulate(
        market_orderbooks_.cbegin(),
        market_orderbooks_.cend(),
        0,
        [&side](const Ticks& partial, const OrderBook* p_ob){
          return partial + p_ob->best_price(side).ticks();
        });
    return (sum > 0) ? sum :
        ((side == Side::buy) ?
         std::numeric_limits<Value>::min() :
         std::numeric_limits<Value>::max());
  }

 private:
  std::array<OrderBook*, N> market_orderbooks_;
};

}  // namespace iem::signal

#endif
