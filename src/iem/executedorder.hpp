// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_executedorder_HPP_
#define iem_src_iem_executedorder_HPP_

#include <algorithm>
#include <utility>
#include <vector>

#include "iem/order.hpp"
#include "iem/price.hpp"
#include "iem/side.hpp"

namespace iem {

class ExecutedOrder {
 public:
  constexpr ExecutedOrder(const Side& side,
                          const Quantity quantity,
                          const Price& price):
    side_(side), quantity_(quantity), price_(price) {
  }

  inline Side side() const noexcept { return side_; }
  inline Quantity quantity() const noexcept { return quantity_; }
  inline Price price() const noexcept { return price_; }

 private:
  Side side_;
  Quantity quantity_;
  Price price_;
  // TODO(rheineke): Missing fields
  // Date + TimeOfDay
  // Contract
  // OrderId
  // TradeId
  // Bundle or Single
};

using ExecutedOrders = std::vector<ExecutedOrder>;

std::ostream& operator<<(std::ostream& os, const ExecutedOrder& executed_order);

class Position {
 public:
  constexpr Position(const int quantity,
                     const double volume_weighted_average_price):
    quantity_(quantity), price_(volume_weighted_average_price) {
  }

  constexpr int quantity() const noexcept { return quantity_; }
  constexpr double price() const noexcept { return price_; }

 private:
  int quantity_;
  double price_;
};

std::ostream& operator<<(std::ostream& os, const Position& position);

std::pair<double, Position> realizedPositionPair(
    const std::vector<ExecutedOrder>& eos);

int_fast32_t priceQtySum(const std::vector<ExecutedOrder>& eos, const Side& s);

int tradeVolume(const std::vector<ExecutedOrder>& eos, const Side& s);

}  // namespace iem

#endif
