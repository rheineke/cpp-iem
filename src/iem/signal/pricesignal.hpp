// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_pricesignal_HPP_
#define iem_src_iem_signal_pricesignal_HPP_

#include "iem/signal.hpp"

namespace iem::signal {

class PriceSignal final: public Signal {
 public:
  inline Value value(const Side&, const Price& price) const final {
    return price.ticks();
  }
};

}  // namespace iem::signal

#endif
