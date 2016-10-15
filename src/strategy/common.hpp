// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_strategy_common_HPP_
#define iem_src_strategy_common_HPP_

#include <map>
#include <vector>

#include "iem/signalmanager.hpp"
#include "iem/signal/signalfwd.hpp"
#include "rule/bundlerulemanager.hpp"
#include "rule/rulemanager.hpp"

namespace strategy::common {

template<size_t N>
std::vector<iem::OrderBook*> constructOrderbookVector(
    const iem::MarketOrderBooks<N>& p_obs) {
  std::vector<iem::OrderBook*> ob_ptr_v;
  for (const auto& ob_ptr : p_obs) {
    ob_ptr_v.emplace_back(ob_ptr);
  }
  return ob_ptr_v;
}

iem::Bundle fixed(const iem::ContractBundle& c_b,
                  const iem::Side& s,
                  const iem::Quantity qty);

template<size_t N>
std::unique_ptr<rule::BundleRuleManager> bundle(
    const iem::Session& session,
    const iem::MarketOrderBooks<N>& market_orderbooks) {
  using std::make_unique;
  using std::move;
  using std::unique_ptr;
  using std::vector;

  const auto& market_ob_v = constructOrderbookVector<N>(market_orderbooks);

  vector<unique_ptr<Signal> > e_rs;
  e_rs.emplace_back(make_unique<ArbitrageSignal<N> >(market_orderbooks));

  vector<unique_ptr<Signal> > p_rs;
  p_rs.emplace_back(make_unique<AgainstSignal>(
      make_unique<ArbitrageSignal<N> >(market_orderbooks)));

  bool allow_orders = false;

  return make_unique<rule::BundleRuleManager>(move(e_rs),
                                              move(p_rs),
                                              session,
                                              market_ob_v,
                                              allow_orders);
}

void signalPtrDiff(const iem::Signal* lhs, const iem::Signal* rhs);

template<size_t N>
std::unique_ptr<rule::RuleManager> ruleManager(
    const iem::Session& session,
    const iem::Logger& logger,
    const iem::OrderBook& ob,
    const boost::posix_time::ptime& add_expiration,
    const iem::MarketOrderBooks<N>& market_obs,
    const std::map<iem::AssetId, double>& asset_fair_values,
    iem::SignalManager* const p_sm) {
  using iem::is_inside;
  using iem::is_outside;
  using iem::sided_equal_to;
  using iem::sided_greater;
  using iem::sided_less;

  using iem::signal::ra_t;
  using rule::ExtremePriceRuleVisitor;
  using rule::OrderPriceRuleVisitor;
  using rule::PriceRuleVisitor;
  using rule::PriorityPriceRuleVisitor;
  using rule::RuleManager;
  using rule::RuleSignal;
  using std::make_unique;

  const auto c_fv = asset_fair_values.at(iem::Contract("TODO").asset_id());

  constexpr int max_os = 15;
  constexpr int max_net_pos = 35;
  constexpr int mkt_b = 986;
  constexpr int mkt_s = 1035;
  constexpr int buf = 10;
  constexpr int oq = 3;  // order quantity
  constexpr int remaining_quantity = 0;

  const auto tocs = p_sm->makeSignal<TicksOutsideSignal>(
      *p_sm->makeSignal<ConstantSignal>(c_fv));
  const auto ltocs = p_sm->makeSignal<LogOnValueSignal>(logger, *tocs);
  const auto tibs = p_sm->makeSignal<TicksInsideBestSignal>(ob);
  const auto ps   = p_sm->makeSignal<PositionSignal>(ob);
  const auto lps  = p_sm->makeSignal<LogOnValueSignal>(logger, *ps);

  std::vector<std::unique_ptr<PriceRuleVisitor> > prvs;
  {  // PriorityPriceRuleVisitor
    const auto prs  = p_sm->makeSignal<PrioritySignal>(ob);
    const auto mpss = p_sm->makeSignal<MarketPriceSumSignal<N> >(market_obs);
    const auto lmpss = p_sm->makeSignal<LogOnValueSignal>(logger, *mpss);
    const auto nos  = p_sm->makeSignal<NumOrderSignal>(ob);
    const auto nps  = p_sm->makeSignal<NetPositionSignal<N> >(market_obs, iem::Contract("TODO"));
    // const auto tolfs = p_sm->makeSignal<TicksOutsideSignal>(
    //     *p_sm->makeSignal<LastFillSignal>(ob));
    // const auto ltolfs = p_sm->makeSignal<LogOnValueSignal>(logger, *tolfs);

    constexpr int C = 1;
    constexpr int R = 7;
    std::array<ra_t<C>, R> add_cs {
        // Not currently first in line
        ra_t<C>(prs , sided_equal_to(), {{{          0,            0}}}),
        // Outside of fair value
        ra_t<C>(ltocs,  sided_greater(), {{{          0,            0}}}),
        // Not an arbitrage
        ra_t<C>(lmpss,     is_outside(), {{{      mkt_b,        mkt_s}}}),
        // Position: bid any time, don't offer if no position
        ra_t<C>(lps ,  sided_greater(), {{{         -1,            0}}}),
        // Number of orders: stop placing orders on a side if too many orders
        ra_t<C>(nos ,     sided_less(), {{{     max_os,       max_os}}}),
        // One tick inside current best bid
        // Should be only a sanity with PriorityPriceRuleVisitor
        ra_t<C>(tibs, sided_equal_to(), {{{          1,            1}}}),
        // Order should be at a better price than last fill on same side
//         ra_t<C>(ltolfs,sided_greater(), {{{          0,            0}}}),
        // Net position: don't {bid,offer} contract if too {long,short}
        ra_t<C>(nps ,     is_outside(), {{{max_net_pos, -max_net_pos}}}),
    };

    std::vector<RuleSignal> place_rules;
    const auto acs = p_sm->makeSignal<ConstraintSignal<C, R> >(add_cs);
    place_rules.emplace_back(acs, oq);

    std::array<ra_t<C>, 1> cxl_constraints {
        // Near arbitrage involving one or more of my orders
        ra_t<C>(mpss, is_inside(), {{{mkt_b+buf, mkt_s-buf}}}),
    };

    const auto cr_cs =
        p_sm->makeSignal<ConstraintSignal<C, 1> >(cxl_constraints);
    std::vector<RuleSignal> cxl_rules;
    cxl_rules.emplace_back(cr_cs, remaining_quantity);

    prvs.emplace_back(make_unique<PriorityPriceRuleVisitor>(ob,
                                                            place_rules,
                                                            cxl_rules));
  }  // End PriorityPriceRuleVisitor

  {  // Begin OrderPriceRuleVisitor
    const auto totps = p_sm->makeSignal<TicksOutsideSignal>(
        *p_sm->makeSignal<TopPriceSignal>(ob));
    const auto ncaps = p_sm->makeSignal<NumContractsAtPriceSignal>(ob);

    constexpr int C = 1;
    constexpr int R = 4;
    std::vector<RuleSignal> empty_add_rules;

    // Stack cancel signals
    std::vector<RuleSignal> stack_cxl_rules;
    std::array<ra_t<C>, R> stack_cxl_cs {
        // Order is within five tick outside top of book
        // probably competing for priority with another participant
        ra_t<C>(totps,    sided_less(), {{{   5,    5}}}),
        ra_t<C>(totps, sided_greater(), {{{   0,    0}}}),
        ra_t<C>(ncaps,    sided_less(), {{{oq+1, oq+1}}}),
        // Cancel only well outside fair value
        ra_t<C>(tocs , sided_greater(), {{{   1,    1}}}),
    };
    const auto sccs = p_sm->makeSignal<ConstraintSignal<C, R> >(stack_cxl_cs);
    stack_cxl_rules.emplace_back(sccs, remaining_quantity);

    prvs.emplace_back(make_unique<OrderPriceRuleVisitor>(ob,
                                                         empty_add_rules,
                                                         stack_cxl_rules));
  }  // End OrderPriceRuleVisitor

  {  // Begin ExtremePriceRuleVisitor
    const auto noaps = p_sm->makeSignal<NumOrdersAtPriceSignal>(ob);
    const auto nobus = p_sm->makeSignal<NumOrderBookUpdateSignal>(ob);

    constexpr int C = 1;  // columns
    constexpr int R = 3;  // rows
    constexpr int extreme_order_quantity = 25;
    std::array<ra_t<C>, R> extreme_add_cs {
        // Empty price level
        ra_t<C>(noaps, sided_equal_to(), {{{         0.,           0.}}}),
        // Nonzero number of orders, sanity check
        ra_t<C>(nobus,  sided_greater(), {{{         0.,           0.}}}),
        // Max net position
        ra_t<C>(ps   ,     is_outside(), {{{max_net_pos, -max_net_pos}}}),
    };
    std::vector<RuleSignal> extreme_place_rules;
    const auto eacs = p_sm->makeSignal<ConstraintSignal<C, R> >(extreme_add_cs);
    extreme_place_rules.emplace_back(eacs, extreme_order_quantity);

    // Extreme cancel signals
    std::vector<RuleSignal> extreme_cxl_rules;

    prvs.emplace_back(make_unique<ExtremePriceRuleVisitor>(ob,
                                                           extreme_place_rules,
                                                           extreme_cxl_rules));
  }  // End ExtremePriceRuleVisitor

  const bool allow_orders = false;

  return make_unique<RuleManager>(std::move(prvs),
                                  session,
                                  ob,
                                  add_expiration,
                                  allow_orders);
}

} // namespace strategy::common

#endif
