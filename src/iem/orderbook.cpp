// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/orderbook.hpp"

namespace iem {

OrderBook::OrderBook(const Contract& contract,
                     const Price& best_bid,
                     const bool best_bid_priority,
                     const Price& best_ask,
                     const bool best_ask_priority,
                     const Price& last_trade,
                     const NumOrders num_buy_orders,
                     const NumOrders num_sell_orders,
                     const position_t position):
    contract_(contract),
    best_bid_(best_bid),
    best_bid_priority_(best_bid_priority),
    best_ask_(best_ask),
    best_ask_priority_(best_ask_priority),
    last_trade_(last_trade),
    num_bid_orders_(num_buy_orders),
    num_ask_orders_(num_sell_orders),
    position_(position) {
}

char priority_char(const OrderBook& ob, const Side& side) {
  return ob.best_price_priority(side) ? '*' : ' ';
}

std::ostream& operator<<(std::ostream& os, const OrderBook& ob) {
  constexpr auto s = Side::SELL;
  constexpr auto b = Side::BUY;
  os << "{\"name\":\"orderBook\""
     << " \"contract\":" << ob.contract()
     << " bid/ask trade: " << ob.best_price(b) << priority_char(ob, b)
     << '/' << ob.best_price(s) << priority_char(ob, s)
     << ' ' << ob.last_trade()
     << ", num bid orders=" << ob.num_orders(b)
     << ", num ask orders=" << ob.num_orders(s)
     << ']'
     << ", position=" << static_cast<int>(ob.position()) << '}';

  return os;
}

int snprintf_orderbook(char* const s, const OrderBook& ob) {
  return 0;
}

}  // namespace iem
