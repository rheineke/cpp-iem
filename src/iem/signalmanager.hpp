// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signalmanager_HPP_
#define iem_src_iem_signalmanager_HPP_

#include <memory>
#include <vector>

#include "iem/signal.hpp"
#include "iem/logger.hpp"

namespace iem {

using UniqueSignal = std::unique_ptr<Signal>;

class SignalManager {
 public:
  template<typename T, typename = std::enable_if_t<std::is_base_of<Signal, std::decay_t<T> >::value >, typename... Args>
  T* const makeSignal(Args&&... args) {
    auto up_signal = std::make_unique<T>(std::forward<Args>(args)...);
    auto p_signal = up_signal.get();
    signals_.emplace_back(std::move(up_signal));

    return p_signal;
  }

  void log(const Logger& logger);

 private:
  std::vector<UniqueSignal> signals_;
};

}  // namespace iem

#endif
