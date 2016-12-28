// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_ORDERBOOK_HPP_
#define CPPIEM_IEM_ORDERBOOK_HPP_

#include <map>
#include <unordered_map>
#include <vector>

#include "boost/variant/variant_fwd.hpp"

#include "iem/price.hpp"
#include "iem/order.hpp"

namespace iem {

using NumOrders = uint_fast8_t;
using position_t = uint_fast16_t;
using Balance = double;

// IEM OrderBook. The order book is first in/first out (FIFO), contains top of
// book quotes only and no quantity information. If your order is first in the
// queue at the top of book price, then IEM displays this as an asterisk. The
// IEM orderbook also provides the last traded price, the number of outstanding
// orders you have on each side, and your current net position.
class OrderBook {
 public:
  OrderBook(const Contract& contract,
            const Price& best_bid,
            const bool best_bid_priority,
            const Price& best_ask,
            const bool best_ask_priority,
            const Price& last_trade,
            const NumOrders num_buy_orders,
            const NumOrders num_sell_orders,
            const position_t position);

  inline const Contract contract() const noexcept { return contract_; }

  inline const Price best_price(const Side& side) const noexcept {
    return (side == Side::BUY) ? best_bid_ : best_ask_;
  }

  // If true, then one of your own orders is first line at the best price
  inline bool best_price_priority(const Side& side) const noexcept {
    return (side == Side::BUY) ? best_bid_priority_ : best_ask_priority_;
  }

  inline const Price last_trade() const noexcept { return last_trade_; }

  inline NumOrders num_orders(const Side& side) const noexcept {
    return (side == Side::BUY) ? num_bid_orders_ : num_ask_orders_;
  }

  inline position_t position() const noexcept { return position_; }

 private:
  Contract contract_;
  Price best_bid_;
  bool best_bid_priority_;
  Price best_ask_;
  bool best_ask_priority_;
  Price last_trade_;
  NumOrders num_bid_orders_;
  NumOrders num_ask_orders_;
  position_t position_;
};

std::ostream& operator<<(std::ostream& os, const OrderBook& ob);

int snprintf_orderbook(char* const s, const OrderBook& ob);

using AssetOrderBooks = std::unordered_map<AssetId,
                                           const std::unique_ptr<OrderBook> >;

template<size_t N>
using MarketOrderBooks = std::array<iem::OrderBook*, N>;

}  // namespace iem

#endif  // CPPIEM_IEM_ORDERBOOK_HPP_
