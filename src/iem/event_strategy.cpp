// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

#include <array>
#include <string>
#include <utility>
#include <vector>

namespace iem {

Orders event_orders(const Event& event, const Quantity qty) {
  Orders os;

  const std::string mkt_name("FedPolicyB");
  // The contract that pays: the one named after what the Fed did
  const std::array<std::pair<Event, std::string>, 3> assets{{
      {Event::UP, "FRup1116"},
      {Event::SAME, "FRsame1116"},
      {Event::DOWN, "FRdown1116"},
  }};
  std::string itm_asset_name;
  std::vector<std::string> otm_asset_names;
  for (const auto& [e, name] : assets) {
    if (e == event) {
      itm_asset_name = name;
    } else {
      otm_asset_names.push_back(name);
    }
  }
  const auto exp_ptime(read_expiration_ptime(mkt_name, itm_asset_name));

  // Buy outstanding orders for in-the-money (ITM) contract
  const PriceTimeLimit itm_ptl(Price(999), exp_ptime);
  const Contract itm_c(mkt_name, itm_asset_name);
  os.emplace_back(std::make_unique<Single>(itm_c, Side::BUY, qty, itm_ptl));

  // Buy bundle from exchange (optional)
  const ContractBundle cb(mkt_name, MonthYear(boost::date_time::Nov, 16));
  os.emplace_back(
      std::make_unique<Bundle>(cb, Side::BUY, qty, Counterparty::EXCHANGE));

  // Sell out-of-the-money (OTM) contracts
  const PriceTimeLimit otm_ptl(Price(1), exp_ptime);
  for (const auto& otm_asset_name : otm_asset_names) {
    const Contract otm_c(mkt_name, otm_asset_name);
    os.emplace_back(std::make_unique<Single>(otm_c, Side::SELL, qty, otm_ptl));
  }
  return os;
}

}  // namespace iem
