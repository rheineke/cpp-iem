// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
//

#include "fatfinger_strategy.hpp"

namespace iem {

Orders fatfinger_orders(const Market& market,
                        const std::string& expiry_date,
                        const Quantity qty) {
  Orders os;

  const auto& root = read_markets_json();
  const auto& assets_val = asset_value(root, market.name(), expiry_date);

  // TODO(rheineke): Compare against orderbook for existing orders
  for (const auto& asset_name : assets_val.getMemberNames()) {
    Contract c(market.name(), asset_name);

    // Buy order
    PriceTimeLimit buy_ptl(Price(1), boost::date_time::not_a_date_time);
    os.emplace_back(std::make_unique<Single>(c, Side::BUY, qty, buy_ptl));

    // Sell order
    PriceTimeLimit sell_ptl(Price(999), boost::date_time::not_a_date_time);
    os.emplace_back(std::make_unique<Single>(c, Side::SELL, qty, sell_ptl));
  }

  return os;
}

}  // namespace iem
