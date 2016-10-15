// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_constantsignal_HPP_
#define iem_src_iem_signal_constantsignal_HPP_

#include "iem/signal.hpp"

namespace iem::signal {

class ConstantSignal final : public Signal {
 public:
  explicit ConstantSignal(const Value value);

  inline Value value(const Side&, const Price&) const final {
    return value_;
  }

 private:
  Value value_;
};

}  // namespace iem::signal

#endif
