// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
//

#include "fatfinger_strategy.hpp"

namespace iem {

std::vector<std::unique_ptr<Order>> fatfinger_orders(const Market market,
                                                     const Quantity qty) {
  std::vector<std::unique_ptr<Order>> os;

  const auto& mkt_dict = read_markets_json()[market.name()];

  return {};
}

}  // namespace iem
