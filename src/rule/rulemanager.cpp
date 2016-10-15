// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "rule/rulemanager.hpp"

namespace rule {

RuleManager::RuleManager(
    std::vector<std::unique_ptr<PriceRuleVisitor> > price_rule_visitors,
    iem::Session& session,
    const iem::OrderBook& orderbook,
    const boost::posix_time::ptime& add_expiration,
    const bool allow_orders):
    price_rule_visitors_(std::move(price_rule_visitors)),
    session_(session),
    orderbook_(orderbook),
    add_expiration_(add_expiration),
    ioc_ptl_(),
    price_(),
    allow_orders_(allow_orders) {
}

// TODO: Replace function signature with Boost.Range?
bool RuleManager::sendOrderCancels(
    std::vector<const iem::Single>::iterator first_order_at_price,
    std::vector<const iem::Single>::iterator last_order_at_price,
    const iem::Quantity remain_quantity) {
  bool cancel_sent = false;
  if (remain_quantity == std::numeric_limits<iem::Quantity>::max()) {
    return cancel_sent;
  }

  int resting_qty = 0;
  for (auto it = first_order_at_price; it != last_order_at_price; ++it) {
    const auto o = *it;
    // Ensure that quantity is quantity left, not original quantity
    resting_qty += o.quantity();
    if (resting_qty > remain_quantity) {
      std::cout << "Send order cancel (" << o.side()
                << ", " << o.price_time_limit().price() << "): "
                << o << std::endl;
      cancel_sent = true;
      if (allow_orders_) {
        session_.cancel_order(o);
      }
    }
  }

  return cancel_sent;
}

bool RuleManager::sendOrder(const iem::Side& side,
                            const iem::Price& price,
                            const iem::Quantity order_quantity) {
  if (order_quantity <= 0) {
    return false;
  }

  iem::PriceTimeLimit ptl(price, add_expiration_);
  iem::Single a_o(iem::Contract("TODO"), side, order_quantity, ptl);

  std::cout << "Send liquidity adding order (" << side << ", " << price << "): "
            << a_o << '\n';

  if (allow_orders_) {
    session_.place_order(a_o);
  }

  return true;
}

void RuleManager::updateOrders(
    const iem::Side& side,
    const std::unique_ptr<PriceRuleVisitor>& price_rule_visitor) {
  price_rule_visitor->begin(side);
  while (!price_rule_visitor->end()) {
    price_.set(price_rule_visitor->next());

    // Check cancel rules only if something to cancel
    auto& os = orderbook_.orders(side);
    // TODO(rheineke): Can improve first iterator here
    auto first_o_it = std::find_if(
        os.cbegin(),
        os.cend(),
        [this](const auto& s) {
          return s.price_time_limit().price() == this->price_;
        });

    // Cancellations
    bool cancel_sent = false;
    if (first_o_it != os.end()) {
      const auto remain_qty = price_rule_visitor->cancelQuantityExceeding();
      auto last_o_it = std::find_if(
          first_o_it,
          os.end(),
          [this](const auto& s) {
            return s.price_time_limit().price() != this->price_;
          });
      cancel_sent = sendOrderCancels(first_o_it, last_o_it, remain_qty);
      if (cancel_sent) {
        price_rule_visitor->log(false);
      }
    }

    if (!cancel_sent) {
      const auto order_qty = price_rule_visitor->placeOrderQuantity();
      const bool order_sent = sendOrder(side, price_, order_qty);
      if (order_sent) {
        price_rule_visitor->log(true);
      }
    }
  }
}

std::chrono::nanoseconds RuleManager::update() {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::nanoseconds;

  int num_calls = 0;
  auto total_duration = 0;
  for (const auto& price_rule_visitor : price_rule_visitors_) {
    const auto t0 = high_resolution_clock::now();
    updateOrders(iem::Side::buy, price_rule_visitor);
    const auto t1 = high_resolution_clock::now();
    total_duration += duration_cast<nanoseconds>(t1-t0).count();
    ++num_calls;
    const auto t2 = high_resolution_clock::now();
    updateOrders(iem::Side::sell, price_rule_visitor);
    const auto t3 = high_resolution_clock::now();
    total_duration += duration_cast<nanoseconds>(t3-t2).count();
    ++num_calls;
  }

  return nanoseconds(total_duration / num_calls);
}

}  // namespace rule
