// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_netpositionsignal_HPP_
#define iem_src_iem_signal_netpositionsignal_HPP_

#include <algorithm>
#include <array>
#include <limits>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

template<size_t N>
class NetPositionSignal final : public Signal {
 public:
  NetPositionSignal(const std::array<OrderBook*, N>& market_orderbooks,
                    const Contract& contract):
      market_orderbooks_(market_orderbooks), contract_(contract) {
  }

  Value value(const Side& side, const Price&) const final {
    net_position = 0;
    smallest_position = std::numeric_limits<position_t>::max();
    largest_position = std::numeric_limits<position_t>::min();
    for (const auto& ob_ptr : market_orderbooks_) {
//      if (ob_ptr->contract().asset() == contract_.asset()) {
//        net_position = ob_ptr->position();
//      }
      smallest_position = std::min(smallest_position, ob_ptr->position());
      largest_position = std::max(largest_position, ob_ptr->position());
    }

    net_position -= ((side == Side::buy) ?
                     smallest_position : largest_position);

    return net_position;
  }

 private:
  std::array<OrderBook*, N> market_orderbooks_;
  Contract contract_;
  mutable int net_position;
  mutable position_t smallest_position;
  mutable position_t largest_position;
};

}  // namespace iem::signal

#endif
