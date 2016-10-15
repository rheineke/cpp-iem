// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_strategy_HPP_
#define iem_src_strategy_HPP_

#include <list>
#include <map>
#include <string>
#include <vector>

#include "iem/contract.hpp"
#include "iem/logger.hpp"
#include "iem/session.hpp"
#include "iem/signalmanager.hpp"
#include "rule/bundlerulemanager.hpp"
#include "rule/rulemanager.hpp"

using MarketManager =
    std::tuple<iem::Market,
               std::unique_ptr<rule::BundleRuleManager>,
               std::vector<std::unique_ptr<rule::RuleManager> > >;

using MarketManagers = std::list<MarketManager>;

class Strategy final {
 public:
  Strategy(const std::string& login, const std::string& password);
  Strategy(const Strategy& strategy) = delete;
  Strategy(Strategy&& strategy) = delete;
  Strategy& operator=(const Strategy& strategy) = delete;
  Strategy& operator=(Strategy&& strategy) = delete;

  void warmup();
  void refresh();

 private:
  template<size_t N>
  std::array<iem::OrderBook*, N> constructOrderbooks(
      const std::array<iem::Contract, N>& cs);

  template<size_t N>
  void initializeStrategy(
      const std::array<iem::Contract, N>& contracts,
      const boost::posix_time::ptime& active_expiration_datetime,
      const std::map<iem::AssetId, double>& asset_fair_values);

  void init();

  void updateMarket(const MarketManager& market_manager);

  iem::Session session_;
  iem::AssetOrderBooks asset_orderbooks_;
  std::unordered_map<iem::Market, std::vector<iem::AssetId> > mkt_assets_;
  MarketManagers market_managers_;
  iem::Balance balance_;
  unsigned int num_refreshes_;
  iem::SignalManager signal_manager_;
  iem::Logger logger_;
};

#endif
