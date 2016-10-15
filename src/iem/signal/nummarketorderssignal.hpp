// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_nummarketorderssignal_HPP_
#define iem_src_iem_signal_nummarketorderssignal_HPP_

#include <array>
#include <numeric>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

template<size_t N>
class NumMarketOrdersSignal final : public Signal {
 public:
  explicit NumMarketOrdersSignal(
      const std::array<OrderBook*, N>& market_orderbooks):
      market_orderbooks_(market_orderbooks) {
  }

  Value value(const Side&, const Price&) const final {
    return std::accumulate(market_orderbooks_.cbegin(),
                           market_orderbooks_.cend(),
                           0,
                           [](const auto& partial, const auto& p_ob) {
                             return partial
                                 + p_ob->orders(Side::buy).size()
                                 + p_ob->orders(Side::sell).size();
                           });
  }

 private:
  std::array<OrderBook*, N> market_orderbooks_;
};

}  // namespace iem::signal

#endif
