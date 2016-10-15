// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_rule_bundlerulemanager_HPP_
#define iem_src_rule_bundlerulemanager_HPP_

#include <vector>

#include "iem/orderbook.hpp"
#include "iem/session.hpp"
#include "iem/signal.hpp"

namespace rule {

using UniqueSignal = std::unique_ptr<iem::Signal>;

class BundleRuleManager {
 public:
  bool update();

  BundleRuleManager(std::vector<UniqueSignal> exchange_cparty_rules,
                    std::vector<UniqueSignal> participant_cparty_rules,
                    iem::Session& session,
                    const std::vector<iem::OrderBook*>& p_orderbooks,
                    const bool allow_orders);

 private:
  std::vector<UniqueSignal> exchange_cparty_rules_;
  std::vector<UniqueSignal> participant_cparty_rules_;

  iem::Session& session_;
  std::vector<iem::OrderBook*> p_orderbooks_;

  bool allow_orders_;

  bool sendBundleOrder(const iem::Side& side,
                       const std::vector<UniqueSignal>& rules,
                       const iem::Counterparty& cp);
};

}  // namespace rule

#endif
