// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_ARBITRAGE_STRATEGY_HPP_
#define CPPIEM_SRC_IEM_ARBITRAGE_STRATEGY_HPP_

#include <memory>
#include <vector>

#include "iem/order.hpp"

namespace iem {
  enum class Event { UP, SAME, DOWN };

  // Assume FedPolicyB for now
  Orders event_orders(const Event& event, const Quantity qty);

}  // namespace iem

#endif // CPPIEM_SRC_IEM_ARBITRAGE_STRATEGY_HPP_
