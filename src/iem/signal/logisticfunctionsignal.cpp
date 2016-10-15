// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/logisticfunctionsignal.hpp"

#include <cmath>

namespace iem {
namespace signal {

LogisticFunctionSignal::LogisticFunctionSignal(
    std::unique_ptr<Signal> p_signal):
    p_signal_(std::move(p_signal)), beta_0_(0), beta_1_(0) {
}

Value LogisticFunctionSignal::value(const Side& side,
                                    const Price& price) const {
  const auto val = exp(-(beta_0_ + beta_1_ * p_signal_->value(side, price)));
  return static_cast<Value>(1./(1. + val));
}

}  // namespace signal
}  // namespace iem
