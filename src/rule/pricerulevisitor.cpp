// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "rule/pricerulevisitor.hpp"

#include <limits>
#include <vector>

#include "iem/signal/constraintsignal.hpp"

namespace rule {

void validate_rules(const std::vector<RuleSignal>& rule_signals,
                    const size_t expected_hashcode) {
  for (const auto& pr : rule_signals) {
    auto shc = typeid(*pr.first).hash_code();
    if (shc != expected_hashcode) {
      static constexpr auto fmt(
          "Expected add signal hash code: %u but received %u");
      throw std::invalid_argument(fmt);
    }
  }
}

PriceRuleVisitor::PriceRuleVisitor(const iem::OrderBook& orderbook,
                                   const std::vector<RuleSignal>& place_rules,
                                   const std::vector<RuleSignal>& cancel_rules):
    orderbook_(orderbook),
    side_(iem::Side::buy),
    current_price_(),
    side_price_count_(0),
    max_place_index_(-1),
    min_cancel_index_(-1),
    place_rules_(place_rules),
    cancel_rules_(cancel_rules) {
//   validate_rules(place_rules_,
//                  typeid(iem::signal::ConstraintSignal<1>).hash_code());
//   validate_rules(cancel_rules_,
//                  typeid(iem::signal::ConstraintSignal<1>).hash_code());
}

PriceRuleVisitor::~PriceRuleVisitor() = default;

iem::Quantity PriceRuleVisitor::cancelQuantityExceeding() {
  if (cancel_rules_.empty()) {
    return std::numeric_limits<iem::Quantity>::max();
  }

  auto remain_qty = std::numeric_limits<iem::Quantity>::max();
  for (size_t i = 0; i < cancel_rules_.size(); ++i) {
    const auto& rule_pair = cancel_rules_[i];
    const auto cs_value = rule_pair.first->value(side_, current_price_);
    if (cs_value > 0) {
      if (rule_pair.second < remain_qty) {
        min_cancel_index_ = static_cast<int>(i);
        min_cancel_columns_ = static_cast<int>(cs_value);
        remain_qty = static_cast<iem::Quantity>(rule_pair.second);
      }

      if (remain_qty == 0) {
        break;
      }
    }
  }

  return remain_qty;
}

iem::Quantity PriceRuleVisitor::placeOrderQuantity() {
  if (place_rules_.empty()) {
    return 0;
  }

  iem::Quantity order_qty = 0;
  int index = 0;
  for (const auto& rule_pair : place_rules_) {
    const auto cs_value = rule_pair.first->value(side_, current_price_);
    if (cs_value > 0) {
      if (rule_pair.second > order_qty) {
        max_place_index_ = index;
        max_place_columns_ = static_cast<int>(cs_value);
        order_qty = static_cast<iem::Quantity>(rule_pair.second);
      }
    }
    ++index;
  }

  return order_qty;
}

void PriceRuleVisitor::log(const bool place) {
  if (place) {
    std::cout << "{\"prv\":\"" << name()
              << "\", \"place\":\"" << max_place_index_
              << "\", \"col\":\"" << max_place_columns_ << "\"}" << std::endl;
  } else {
    std::cout << "{\"prv\":\"" << name()
              << "\", \"cancel\":\"" << min_cancel_index_
              << "\", \"col\":\"" << min_cancel_columns_ << "\"}" << std::endl;
  }
}

FullLiquidityPriceRuleVisitor::FullLiquidityPriceRuleVisitor(
    const iem::OrderBook& orderbook,
    const iem::Ticks ticks_outside_top,
    const std::vector<RuleSignal>& place_rules,
    const std::vector<RuleSignal>& cancel_rules):
    PriceRuleVisitor(orderbook, place_rules, cancel_rules),
    ticks_outside_top_(ticks_outside_top) {
}

void FullLiquidityPriceRuleVisitor::begin(const iem::Side& side) {
  side_price_count_ = 0;

  side_ = side;

  iem::Price const other_best_price =
      orderbook_.best_price(iem::otherSide(side));
  current_price_.set(other_best_price.ticks());

  const auto best_price_tick = orderbook_.best_price(side).ticks();
  const int unnormalized_most_outside_tick =
      iem::move_outside(side, best_price_tick, ticks_outside_top_);
  const int extreme_tick = iem::extreme_tick(side);
  most_outside_tick_ =
      iem::most_inside(side, extreme_tick, unnormalized_most_outside_tick);
}

iem::Ticks FullLiquidityPriceRuleVisitor::next() {
  if (iem::is_inside_of(side_, current_price_.ticks(), most_outside_tick_)) {
    current_price_.set(iem::move_outside(side_, current_price_.ticks()));
    ++side_price_count_;
  }
  return current_price_.ticks();
}

bool FullLiquidityPriceRuleVisitor::end() {
  const bool finished = (current_price_.ticks() == most_outside_tick_);
  if (finished) {
    std::cout << "Side " << side_ << " price count: " << side_price_count_
              << '\n';
  }
  return finished;
}

PriorityPriceRuleVisitor::PriorityPriceRuleVisitor(
    const iem::OrderBook& orderbook,
    const std::vector<RuleSignal>& place_rules,
    const std::vector<RuleSignal>& cancel_rules):
    PriceRuleVisitor(orderbook, place_rules, cancel_rules) { }

void PriorityPriceRuleVisitor::begin(const iem::Side& side) {
  side_ = side;
  current_price_.set(iem::move_inside(side,
                                      orderbook_.best_price(side).ticks()));

  const auto other_best_price_tick =
      orderbook_.best_price(iem::otherSide(side)).ticks();
  const auto spread_gap = iem::ticks_inside_of(side,
                                               other_best_price_tick,
                                               current_price_.ticks()) > 0;
  side_price_count_ = spread_gap ? 0 : 1;
}

iem::Ticks PriorityPriceRuleVisitor::next() {
  ++side_price_count_;
  return current_price_.ticks();
}

ExtremePriceRuleVisitor::ExtremePriceRuleVisitor(
    const iem::OrderBook& orderbook,
    const std::vector<RuleSignal>& place_rules,
    const std::vector<RuleSignal>& cancel_rules):
    PriceRuleVisitor(orderbook, place_rules, cancel_rules) {
}

void ExtremePriceRuleVisitor::begin(const iem::Side& side) {
  side_ = side;
  current_price_.set(((side == iem::Side::buy) ? 1 : 999));
  side_price_count_ = 0;
}

iem::Ticks ExtremePriceRuleVisitor::next() {
  ++side_price_count_;
  return current_price_.ticks();
}

bool price_less(const iem::Single& lhs, const iem::Single& rhs) {
  return lhs.price_time_limit().price() < rhs.price_time_limit().price();
}

bool price_greater(const iem::Single& lhs, const iem::Single& rhs) {
  return lhs.price_time_limit().price() > rhs.price_time_limit().price();
}

OrderPriceRuleVisitor::OrderPriceRuleVisitor(
    const iem::OrderBook& orderbook,
    const std::vector<RuleSignal>& place_rules,
    const std::vector<RuleSignal>& cancel_rules):
    PriceRuleVisitor(orderbook, place_rules, cancel_rules),
    orders_(),
    compare_(price_less) {
}

void OrderPriceRuleVisitor::begin(const iem::Side& side) {
  side_ = side;
  orders_ = orderbook_.orders(side);
  compare_ = (side_ == iem::Side::buy) ? price_greater : price_less;
  iters_ = {orders_.cbegin(), orders_.cbegin()};
  if (iters_.second != orders_.end()) {
    current_price_.set(iters_.first->price_time_limit().price().ticks());
  }
  side_price_count_ = 0;
}

iem::Ticks OrderPriceRuleVisitor::next() {
  iters_ = std::equal_range(iters_.first,
                            orders_.cend(),
                            *iters_.second,
                            compare_);
  current_price_.set(iters_.first->price_time_limit().price().ticks());
  ++side_price_count_;
  return current_price_.ticks();
}

bool OrderPriceRuleVisitor::end() {
  return iters_.second == orders_.end();
}

}  // namespace rule
