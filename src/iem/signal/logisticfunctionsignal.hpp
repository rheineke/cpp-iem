// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_logisticfunctionsignal_HPP_
#define iem_src_iem_signal_logisticfunctionsignal_HPP_

#include "iem/signal.hpp"

namespace iem::signal {

class LogisticFunctionSignal final : public Signal {
 public:
  explicit LogisticFunctionSignal(std::unique_ptr<Signal> p_signal);

  Value value(const Side& side, const Price& price) const final;

 private:
  std::unique_ptr<Signal> p_signal_;
  Value beta_0_;
  Value beta_1_;
};

}  // namespace iem::signal

#endif
