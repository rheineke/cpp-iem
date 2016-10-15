// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_logonvaluesignal_HPP_
#define iem_src_iem_signal_logonvaluesignal_HPP_

#include "iem/signal.hpp"
#include "iem/logger.hpp"

namespace iem::signal {

class LogOnValueSignal final : public Signal {
 public:
  explicit LogOnValueSignal(const Logger& logger, const Signal& signal);

  Value value(const Side& side, const Price& price) const final;

 private:
  const Logger& logger_;
  const Signal& signal_;
  mutable Value value_;
};

}  // namespace iem::signal

#endif
