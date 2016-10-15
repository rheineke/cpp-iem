// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_numorderbookupdatesignal_HPP_
#define iem_src_iem_signal_numorderbookupdatesignal_HPP_

#include "iem/signal.hpp"
#include "iem/orderbook.hpp"

namespace iem::signal {

class NumOrderBookUpdateSignal final : public Signal {
 public:
  explicit NumOrderBookUpdateSignal(const OrderBook& orderbook);

  inline Value value(const Side&, const Price&) const final {
    return orderbook_.num_updates();
  };

 private:
  const OrderBook& orderbook_;
};

}  // namespace iem::signal

#endif
