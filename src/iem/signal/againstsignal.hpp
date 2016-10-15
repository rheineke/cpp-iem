// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_againstsignal_HPP_
#define iem_src_iem_signal_againstsignal_HPP_

#include "iem/signal.hpp"

namespace iem::signal {

class AgainstSignal final : public Signal {
 public:
  explicit AgainstSignal(std::unique_ptr<Signal> p_signal);

  Value value(const Side& side, const Price& price) const final;
 private:
  std::unique_ptr<Signal> p_signal_;
};

}  // namespace iem::signal

#endif
