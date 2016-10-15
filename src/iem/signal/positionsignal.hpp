// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_positionsignal_HPP_
#define iem_src_iem_signal_positionsignal_HPP_

#include <vector>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

class PositionSignal final : public Signal {
 public:
  explicit PositionSignal(const OrderBook& orderbook);

  inline Value value(const Side&, const Price&) const final {
    return orderbook_.position();
  }

 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
