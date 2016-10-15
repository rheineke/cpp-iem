// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_prioritysignal_HPP_
#define iem_src_iem_signal_prioritysignal_HPP_

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

class PrioritySignal final : public Signal {
 public:
  explicit PrioritySignal(const OrderBook& orderbook);

  inline Value value(const Side& side, const Price&) const final {
    return orderbook_.best_price_priority(side);
  }
 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
