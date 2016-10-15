// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_ticksoutsidesignal_HPP_
#define iem_src_iem_signal_ticksoutsidesignal_HPP_

#include "iem/signal.hpp"

namespace iem::signal {

class TicksOutsideSignal final : public Signal {
 public:
  explicit TicksOutsideSignal(const Signal& signal);

  Value value(const Side& side, const Price& price) const final;

 private:
  const Signal& signal_;
};

}  // namespace iem::signal

#endif
