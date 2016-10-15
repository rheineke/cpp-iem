// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_outstandingquantitysignal_HPP_
#define iem_src_iem_signal_outstandingquantitysignal_HPP_

#include <vector>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

class OutstandingQuantitySignal final : public Signal {
 public:
  explicit OutstandingQuantitySignal(const OrderBook& orderbook);

  Value value(const Side& side, const Price& price) const final;

 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
