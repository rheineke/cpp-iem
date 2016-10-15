// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/againstsignal.hpp"

namespace iem::signal {

AgainstSignal::AgainstSignal(std::unique_ptr<Signal> p_signal):
    p_signal_(std::move(p_signal)) {
}

Value AgainstSignal::value(const Side& side, const Price& price) const {
  return p_signal_->value(otherSide(side), price);
}

}  // namespace iem::signal
