// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "strategy.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

Strategy::Strategy(const std::string& login, const std::string& password):
  session_("", ""),
  asset_orderbooks_(),
  mkt_assets_(),
  market_managers_(),
  balance_(0),
  num_refreshes_(0),
  signal_manager_(),
  logger_() {
  logger_.log();
  init();
}

void Strategy::updateMarket(const MarketManager& market_manager) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::nanoseconds;

  constexpr std::size_t kBundle = 1;
  if (std::get<kBundle>(market_manager)) {
    const auto t0 = high_resolution_clock::now();
    std::get<kBundle>(market_manager)->update();
    const auto t1 = high_resolution_clock::now();

    snprintf(logger_.getBuffer(),
             logger_.kBufferSize,
             "%lld ns to complete bundle update",
             duration_cast<nanoseconds>(t1-t0).count());
    logger_.log();
  }

  constexpr std::size_t kSingle = 2;
  for (const auto& p_rm : std::get<kSingle>(market_manager)) {
    const auto t0 = high_resolution_clock::now();
    const auto d1 = p_rm->update();
    const auto t1 = high_resolution_clock::now();

    snprintf(logger_.getBuffer(),
             logger_.kBufferSize,
             "%lld ns to complete each prv",
             d1.count());
    logger_.log();

    snprintf(logger_.getBuffer(),
             logger_.kBufferSize,
             "%lld ns to complete asset: %d",
             duration_cast<nanoseconds>(t1-t0).count(),
             p_rm->contract().asset_id());
    logger_.log();
  }
}

void Strategy::warmup() {
  if (num_refreshes_ == 0) {
    snprintf(logger_.getBuffer(),
             logger_.kBufferSize,
             "[%u] refresh count",
             num_refreshes_);
    logger_.log();

    for (const auto& market_manager : market_managers_) {
      updateMarket(market_manager);
    }
  }
  num_refreshes_++;
}

void Strategy::refresh() {
  snprintf(logger_.getBuffer(),
           logger_.kBufferSize,
           "[%u] refresh count",
           num_refreshes_);
  logger_.log();

  double total_realized_profit = 0;
  for (const auto& market_manager : market_managers_) {
    const auto mkt = std::get<0>(market_manager);

    // Update market state

    snprintf(logger_.getBuffer(), logger_.kBufferSize, "Market orderbooks:");
    logger_.log();

    constexpr auto fmt("Buy/sell value: [%d,%d], realized profit: %.2f, volume:"
                       " [%d,%d]");

    updateMarket(market_manager);
  }

  snprintf(logger_.getBuffer(),
           logger_.kBufferSize,
           "Total realized profit: %.4f",
           total_realized_profit);
  logger_.log();

  num_refreshes_++;
}

template<std::size_t N>
std::array<iem::OrderBook*, N> Strategy::constructOrderbooks(
    const std::array<iem::Contract, N>& cs) {
  std::array<iem::OrderBook*, N> ob_ptrs;
  std::vector<iem::AssetId> assets;
  for (std::size_t i = 0; i < cs.size(); ++i) {
    const auto &c = cs[i];
    assets.emplace_back(c.asset_id());
  }

  return ob_ptrs;
}

template<std::size_t N>
void Strategy::initializeStrategy(
    const std::array<iem::Contract, N>& contracts,
    const boost::posix_time::ptime& active_expiration_datetime,
    const std::map<iem::AssetId, double>& asset_fair_values) {
  auto ob_ptrs = constructOrderbooks<N>(contracts);

  std::vector<std::unique_ptr<rule::RuleManager> > rule_managers;
}

void Strategy::init() {
  signal_manager_.log(logger_);
}
