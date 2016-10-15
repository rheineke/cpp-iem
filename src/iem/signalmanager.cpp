// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/signalmanager.hpp"

namespace iem {

void SignalManager::log(const Logger& logger) {
  snprintf(logger.getBuffer(),
           logger.kBufferSize,
           "Number of signals: %ld",
           signals_.size());
  logger.log();
}

}  // namespace iem
