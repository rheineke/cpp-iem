// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/event_strategy.hpp"

namespace iem {

std::vector<std::unique_ptr<Order>> event_orders(const Event& event,
                                                 const Quantity qty) {
  std::vector<std::unique_ptr<Order>> os;

  const std::string mkt_name("FedPolicyB");
  const std::string expiry_str("1116");
  const auto itm_asset_name = "FRsame1116";
  const auto exp_ptime(read_expiration_ptime(mkt_name, itm_asset_name));

  // Buy outstanding orders for in-the-money (ITM) contract
  const PriceTimeLimit itm_ptl(Price(999), exp_ptime);
  const Contract itm_c(itm_asset_name);
  os.emplace_back(std::make_unique<Single>(itm_c, Side::BUY, qty, itm_ptl));

  // Buy bundle from exchange (optional)
  const ContractBundle cb(mkt_name, MonthYear(boost::date_time::Nov, 16));
  os.emplace_back(std::make_unique<Bundle>(cb, Side::BUY, qty,
                                           Counterparty::EXCHANGE));

  // Sell out-of-the-money (OTM) contracts
  const std::array<std::string, 2> otm_asset_names{{"FRup1116", "FRdown1116"}};
  const PriceTimeLimit otm_ptl(Price(1), exp_ptime);
  for (const auto& otm_asset_name : otm_asset_names) {
    const Contract otm_c(otm_asset_name);
    os.emplace_back(std::make_unique<Single>(otm_c, Side::SELL, qty, otm_ptl));
  }
  return os;
}

}  // namespace iem