// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_ticksinsidebestsignal_HPP_
#define iem_src_iem_signal_ticksinsidebestsignal_HPP_

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

class TicksInsideBestSignal final : public Signal {
 public:
  explicit TicksInsideBestSignal(const OrderBook& orderbook);

  Value value(const Side& side, const Price& price) const final;

 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
