// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_numordersatpricesignal_HPP_
#define iem_src_iem_signal_numordersatpricesignal_HPP_

#include "iem/signal.hpp"
#include "iem/orderbook.hpp"

namespace iem::signal {

class NumOrdersAtPriceSignal final : public Signal {
 public:
  explicit NumOrdersAtPriceSignal(const OrderBook& orderbook);

  Value value(const Side& side, const Price& price) const final;

 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
