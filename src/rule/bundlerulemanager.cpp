// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "rule/bundlerulemanager.hpp"

namespace rule {

BundleRuleManager::BundleRuleManager(
    std::vector<UniqueSignal> exchange_cparty_rules,
    std::vector<UniqueSignal> participant_cparty_rules,
    iem::Session& session,
    const std::vector<iem::OrderBook*>& p_orderbooks,
    const bool allow_orders):
    exchange_cparty_rules_(std::move(exchange_cparty_rules)),
    participant_cparty_rules_(std::move(participant_cparty_rules)),
    session_(session),
    p_orderbooks_(p_orderbooks),
    allow_orders_(allow_orders) {
}

bool BundleRuleManager::sendBundleOrder(const iem::Side& side,
                                        const std::vector<UniqueSignal>& rules,
                                        const iem::Counterparty& cp) {
  if (rules.empty()) {
    return false;
  }

  iem::Quantity order_qty = 0;
  for (const auto& rule : rules) {
    const auto rule_quantity = rule->value(side, iem::nanPrice());
    if (rule_quantity > order_qty) {
      order_qty = static_cast<iem::Quantity>(rule_quantity);
    }
  }

  if (order_qty > 0) {
    // TODO(rheineke): Use a pool of recycled order objects
    const auto cb = iem::ContractBundle("TODO",
                                        {boost::date_time::months_of_year::Dec,
                                         13});
    // const auto cb = p_orderbooks_.at(0)->contract().contract_bundle();
    const auto& b_o = iem::Bundle(cb, side, order_qty, cp);

    std::cout << "Bundle rule manager would send order: " << b_o << '\n';

    if (allow_orders_) {
      session_.place_order(b_o);
    }

    return true;
  }

  return false;
}

bool BundleRuleManager::update() {
  using iem::Counterparty::exchange;
  using iem::Counterparty::participant;
  using iem::Side;

  bool sent = false;
  sent |= sendBundleOrder(Side::buy , exchange_cparty_rules_, exchange);
  sent |= sendBundleOrder(Side::sell, exchange_cparty_rules_, exchange);
  sent |= sendBundleOrder(Side::buy , participant_cparty_rules_, participant);
  sent |= sendBundleOrder(Side::sell, participant_cparty_rules_, participant);

  return sent;
}

}  // namespace rule
