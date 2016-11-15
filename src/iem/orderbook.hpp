// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_ORDERBOOK_HPP_
#define CPPIEM_SRC_IEM_ORDERBOOK_HPP_

#include <map>
#include <unordered_map>
#include <vector>

#include "boost/variant/variant_fwd.hpp"

#include "iem/executedorder.hpp"
#include "iem/price.hpp"
#include "iem/order.hpp"

namespace iem {

using NumOrders = uint_fast8_t;
using position_t = uint_fast16_t;
using Balance = double;

class OrderBook {
 public:
  OrderBook(const std::string& contract,
            const Price& best_bid,
            const bool best_bid_priority,
            const Price& best_ask,
            const bool best_ask_priority,
            const Price& last_trade,
            const unsigned int num_buy_orders,
            const unsigned int num_sell_orders,
            const position_t position);

  const std::string contract() const;

  inline const Price best_price(const Side& side) const {
    return (side == Side::BUY) ? best_bid_ : best_ask_;
  }

  bool best_price_priority(const Side& side) const;

  const Price last_trade() const;

  const SingleOrders orders(const Side& side) const;

  position_t position() const;

  const ExecutedOrders executedOrders() const;

  void update(const SingleOrders& bid_orders,
              const SingleOrders& ask_orders);

  void update(const ExecutedOrders& executed_orders);

  inline uint_fast64_t num_updates() const { return num_updates_; }

 private:
  std::string contract_;
  Price best_bid_;
  bool best_bid_priority_;
  Price best_ask_;
  bool best_ask_priority_;
  Price last_trade_;
  SingleOrders bid_orders_;
  SingleOrders ask_orders_;
  position_t position_;

  ExecutedOrders executed_orders_;

  uint_fast64_t num_updates_;
};

using OrderInfo = boost::variant<NumOrders, SingleOrders>;

std::ostream& operator<<(std::ostream& os, const OrderInfo& orders);

std::ostream& operator<<(std::ostream& os, const OrderBook& ob);

int snprintf_orderbook(char* const s, const OrderBook& ob);

using AssetOrderBooks = std::unordered_map<AssetId,
                                           const std::unique_ptr<OrderBook> >;

template<size_t N>
using MarketOrderBooks = std::array<iem::OrderBook*, N>;

}  // namespace iem

#endif
