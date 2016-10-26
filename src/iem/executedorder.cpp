// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/executedorder.hpp"

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace iem {

constexpr char sideChar(const Side& side) noexcept {
  return (side == Side::buy) ? 'B' : 'A';
}

std::ostream& operator<<(std::ostream& os,
                         const ExecutedOrder& executed_order) {
  return os << "ExecutedOrder{"
            << "side=" << sideChar(executed_order.side())
            << ", quantity=" << static_cast<int>(executed_order.quantity())
            << ", price=" << executed_order.price()
            << '}';
}

std::ostream& operator<<(std::ostream& os, const Position& position) {
  return os << "Position{"
            << "quantity=" << static_cast<int>(position.quantity())
            << ", volumeWeightedAveragePrice=" << position.price()
            << '}';
}

std::pair<double, Position> realizedPositionPair(
    const std::vector<ExecutedOrder>& eos) {
  double buy_value = 0, sell_value = 0;
  Quantity buy_qty = 0, sell_qty = 0;
  for (auto& eo : eos) {
    if (eo.price() == nanPrice()) {
      continue;
    }
    if (eo.side() == Side::buy) {
      buy_value += (eo.quantity() * eo.price().ticks());
      buy_qty += eo.quantity();
    } else {
      sell_value += (eo.quantity() * eo.price().ticks());
      sell_qty += eo.quantity();
    }
  }

  double avg_buy_px = buy_value / buy_qty;
  double avg_sell_px = sell_value / sell_qty;
  Quantity min_side_qty = std::min(buy_qty, sell_qty);
  int pos_qty = buy_qty - sell_qty;

  const auto realized_profit = min_side_qty * (avg_sell_px - avg_buy_px);
  const auto vwap = (pos_qty == 0) ?
      NAN : ((pos_qty < 0) ? avg_sell_px : avg_buy_px);

  return {realized_profit, Position(pos_qty, vwap)};
}

int_fast32_t priceQtySum(const std::vector<ExecutedOrder>& eos, const Side& s) {
  return std::accumulate(
      eos.cbegin(),
      eos.cend(),
      0,
      [&s](const int_fast32_t& partial, const ExecutedOrder& eo) {
        if (eo.side() == s && eo.price() != nanPrice()) {
          return partial + (eo.quantity() * eo.price().ticks());
        }
        return partial;
      });
}

int tradeVolume(const std::vector<ExecutedOrder>& eos, const Side& s) {
  return std::accumulate(eos.cbegin(),
                         eos.cend(),
                         0,
                         [&s](const int& partial, const ExecutedOrder& eo) {
                           if (eo.side() == s && eo.price() != nanPrice()) {
                             return partial + eo.quantity();
                           }

                           return partial;
                         });
}

}  // namespace iem
