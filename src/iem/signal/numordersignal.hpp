// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_numordersignal_HPP_
#define iem_src_iem_signal_numordersignal_HPP_

#include <vector>

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace iem::signal {

class NumOrderSignal final : public Signal {
 public:
  explicit NumOrderSignal(const OrderBook& orderbook);

  inline Value value(const Side& side, const Price&) const final {
    return orderbook_.orders(side).size();
  }
 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
