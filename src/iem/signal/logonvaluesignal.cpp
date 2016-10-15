// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signal/logonvaluesignal.hpp"

#include "boost/type_index.hpp"

namespace iem {
namespace signal {

LogOnValueSignal::LogOnValueSignal(const Logger& logger, const Signal& signal):
    logger_(logger),
    signal_(signal),
    value_(NAN) {
}

Value LogOnValueSignal::value(const Side& side, const Price& price) const {
  using boost::typeindex::type_id_with_cvr;

  value_ = signal_.value(side, price);
  snprintf(logger_.getBuffer(),
           logger_.kBufferSize,
           "%s.value(%s, %u) = %.2f",
           type_id_with_cvr<decltype(signal_)>().pretty_name().c_str(),
           (side == Side::buy) ? "buy" : "sell",
           price.ticks(),
           value_);
  logger_.log();

  return value_;
}

}  // namespace signal
}  // namespace iem
