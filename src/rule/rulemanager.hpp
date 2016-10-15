// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_rule_rulemanager_HPP_
#define iem_src_rule_rulemanager_HPP_

#include <chrono>
#include <vector>

#include "iem/orderbook.hpp"
#include "iem/session.hpp"
#include "iem/signal.hpp"
#include "rule/pricerulevisitor.hpp"

namespace rule {

class RuleManager {
 public:
  RuleManager(
      std::vector<std::unique_ptr<PriceRuleVisitor> > price_rule_visitors,
      iem::Session& session,
      const iem::OrderBook& orderbook,
      const boost::posix_time::ptime& add_expiration,
      const bool allow_orders);

  std::chrono::nanoseconds update();

  inline const iem::Contract contract() const {
    // TODO: Fix this
    return iem::Contract("TODO");
  }

 private:
  std::vector<std::unique_ptr<PriceRuleVisitor> > price_rule_visitors_;
  iem::Session& session_;
  const iem::OrderBook& orderbook_;
  boost::posix_time::ptime add_expiration_;
  iem::PriceTimeLimit ioc_ptl_;
  iem::Price price_;
  bool allow_orders_;

  bool sendOrderCancels(
      std::vector<const iem::Single>::iterator first_order_at_price,
      std::vector<const iem::Single>::iterator last_order_at_price,
      const iem::Quantity remain_quantity);

  bool sendOrder(const iem::Side& side,
                 const iem::Price& price,
                 const iem::Quantity order_quantity);

  void updateOrders(
      const iem::Side& side,
      const std::unique_ptr<PriceRuleVisitor>& price_rule_visitor);
};

}  // namespace rule

#endif
