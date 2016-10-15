// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_rule_pricerulevisitor_HPP_
#define iem_src_rule_pricerulevisitor_HPP_

#include <utility>
#include <vector>

#include "boost/type_index.hpp"

#include "iem/orderbook.hpp"
#include "iem/signal.hpp"

namespace rule {

using RuleSignal = std::pair<iem::Signal*, int>;

class PriceRuleVisitor {
 public:
  PriceRuleVisitor(const iem::OrderBook& orderbook,
                   const std::vector<RuleSignal>& place_rules,
                   const std::vector<RuleSignal>& cancel_rules);
  PriceRuleVisitor(const PriceRuleVisitor& pv) = delete;
  PriceRuleVisitor(PriceRuleVisitor&& pv) noexcept = default;
  PriceRuleVisitor& operator=(const PriceRuleVisitor& pv) = delete;
  PriceRuleVisitor& operator=(PriceRuleVisitor&& pv) noexcept = default;
  virtual ~PriceRuleVisitor() = 0;

  virtual void begin(const iem::Side& side) = 0;
  virtual iem::Ticks next() = 0;
  virtual bool end() = 0;
  iem::Quantity cancelQuantityExceeding();
  iem::Quantity placeOrderQuantity();
  void log(const bool place);

 protected:
  const iem::OrderBook& orderbook_;
  iem::Side side_;
  iem::Price current_price_;
  int side_price_count_;
  int max_place_index_;
  int max_place_columns_;
  int min_cancel_index_;
  int min_cancel_columns_;

 private:
  virtual const char* name() = 0;

  std::vector<RuleSignal> place_rules_;
  std::vector<RuleSignal> cancel_rules_;
};

class FullLiquidityPriceRuleVisitor final : public PriceRuleVisitor {
 public:
  FullLiquidityPriceRuleVisitor(const iem::OrderBook& orderbook,
                                const iem::Ticks ticks_outside_top,
                                const std::vector<RuleSignal>& place_rules,
                                const std::vector<RuleSignal>& cancel_rules);
  void begin(const iem::Side& side) final;
  iem::Ticks next() final;
  bool end() final;

 private:
  inline const char* name() override final {
    using boost::typeindex::type_id_with_cvr;
    return type_id_with_cvr<decltype(this)>().pretty_name().c_str();
  }

  iem::Ticks ticks_outside_top_;
  iem::Ticks most_outside_tick_;
};

class PriorityPriceRuleVisitor final : public PriceRuleVisitor {
 public:
  PriorityPriceRuleVisitor(const iem::OrderBook& orderbook,
                           const std::vector<RuleSignal>& place_rules,
                           const std::vector<RuleSignal>& cancel_rules);
  void begin(const iem::Side& side) final;
  iem::Ticks next() final;
  inline bool end() final { return (side_price_count_ > 0); }

 private:
  inline const char* name() override final {
    using boost::typeindex::type_id_with_cvr;
    return type_id_with_cvr<decltype(this)>().pretty_name().c_str();
  }
};

class ExtremePriceRuleVisitor final : public PriceRuleVisitor {
 public:
  ExtremePriceRuleVisitor(const iem::OrderBook& orderbook,
                          const std::vector<RuleSignal>& place_rules,
                          const std::vector<RuleSignal>& cancel_rules);
  void begin(const iem::Side& side) final;
  iem::Ticks next() final;
  inline bool end() final { return side_price_count_ > 0; }

 private:
  inline const char* name() override final {
    using boost::typeindex::type_id_with_cvr;
    return type_id_with_cvr<decltype(this)>().pretty_name().c_str();
  }
};

using OrderRange = std::pair<iem::Orders::const_iterator,
                             iem::Orders::const_iterator>;

// Iterate over unique order prices
class OrderPriceRuleVisitor final : public PriceRuleVisitor {
 public:
  OrderPriceRuleVisitor(const iem::OrderBook& orderbook,
                        const std::vector<RuleSignal>& place_rules,
                        const std::vector<RuleSignal>& cancel_rules);
  void begin(const iem::Side& side) final;
  iem::Ticks next() final;
  bool end() final;

 private:
  inline const char* name() override final {
    using boost::typeindex::type_id_with_cvr;
    return type_id_with_cvr<decltype(this)>().pretty_name().c_str();
  }

  iem::Orders orders_;
  OrderRange iters_;
  std::function<bool(const iem::Single&, const iem::Single&)> compare_;
};

}  // namespace rule

#endif
