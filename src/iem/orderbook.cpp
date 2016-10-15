// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/orderbook.hpp"

#include "boost/variant/variant.hpp"

namespace iem {

OrderBook::OrderBook(const std::string& contract,
                     const Price& best_bid,
                     const bool best_bid_priority,
                     const Price& best_ask,
                     const bool best_ask_priority,
                     const Price& last_trade,
                     const unsigned int num_buy_orders,
                     const unsigned int num_sell_orders,
                     const position_t position):
    contract_(contract),
    best_bid_(best_bid),
    best_bid_priority_(best_bid_priority),
    best_ask_(best_ask),
    best_ask_priority_(best_ask_priority),
    last_trade_(last_trade),
    bid_orders_(),
    ask_orders_(),
    position_(position),
    executed_orders_() {
  bid_orders_.reserve(num_buy_orders);
  ask_orders_.reserve(num_sell_orders);
}

const std::string OrderBook::contract() const { return contract_; }

bool OrderBook::best_price_priority(const Side& side) const {
  return (side == Side::buy) ? best_bid_priority_ : best_ask_priority_;
}

const Price OrderBook::last_trade() const { return last_trade_; }

const Orders OrderBook::orders(const Side& side) const {
  return (side == Side::buy) ? bid_orders_ : ask_orders_;
}

position_t OrderBook::position() const { return position_; }

const ExecutedOrders OrderBook::executedOrders() const {
  return executed_orders_;
}

void OrderBook::update(const Orders& bid_orders, const Orders& ask_orders) {
  bid_orders_ = bid_orders;
  ask_orders_ = ask_orders;
}

void OrderBook::update(const ExecutedOrders& executed_orders) {
  executed_orders_ = executed_orders;
}

class OrdersVisitor final : public boost::static_visitor<bool> {
 public:
  explicit OrdersVisitor(std::ostream* const p_os):
      p_os_(p_os) {
  }

  bool operator()(const NumOrders num_orders) const {
    *p_os_ << static_cast<int>(num_orders);
    return false;
  }

  bool operator()(const Orders& orders) const {
    *p_os_ << orders.size() << '~';
    return false;
  }

 private:
  std::ostream* p_os_;
};

std::ostream& operator<<(std::ostream& os, const OrderInfo& orders) {
  // TODO: Replace with C++14 generic lambda?
  // apply_visitor_delayed_cpp14_t
//   auto str = boost::apply_visitor(
//       [](auto v) {
//         return boost::lexical_cast<std::string>(v);
//       },
//       variant_instance);
  boost::apply_visitor(OrdersVisitor(&os), orders);
  return os;
}

char priority_char(const OrderBook& ob, const Side& side) {
  return ob.best_price_priority(side) ? '*' : ' ';
}

std::ostream& operator<<(std::ostream& os, const OrderBook& ob) {
  static constexpr auto s = Side::sell;
  static constexpr auto b = Side::buy;
  os << "{\"name\":\"orderBook\""
     << " \"contract\":" << ob.contract()
     << " bid/ask trade: " << ob.best_price(b) << priority_char(ob, b)
     << '/' << ob.best_price(s) << priority_char(ob, s)
     << ' ' << ob.last_trade()
     << ", bid orders=" << ob.orders(b)
     << ", ask orders=" << ob.orders(s)
     << ']'
     << ", position=" << static_cast<int>(ob.position())
     << ", num executed orders=" << ob.executedOrders().size() << '}';

  return os;
}

int snprintf_orderbook(char* const s, const OrderBook& ob) {
  return 0;
}

}  // namespace iem
