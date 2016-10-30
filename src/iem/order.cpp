// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/order.hpp"

namespace iem {

std::ostream& operator<<(std::ostream& os, const Counterparty& cp) {
  switch (cp) {
    case Counterparty::EXCHANGE:
      return os << "exchange";
    case Counterparty::PARTICIPANT:
      return os << "participant";
  }
}

Side Order::side() const noexcept { return side_; }
Quantity Order::quantity() const noexcept { return quantity_; }
Counterparty Order::counterparty() const { return counterparty_; }

std::ostream& operator<<(std::ostream& os, const Order& o) {
  o.print(&os);
  return os;
}

Order::Order(const Side& side,
             const Quantity quantity,
             const PriceTimeLimit& price_time_limit,
             const Counterparty& counterparty):
    side_(side),
    quantity_(quantity),
    price_time_limit_(price_time_limit),
    counterparty_(counterparty) {
}

const Contract Single::contract() const { return contract_; }

void Single::print(std::ostream* const p_os) const {
  *p_os << "SingleOrder{"
        << "side=" << side()
        << ", quantity=" << static_cast<int>(quantity())
        << ", priceTimeLimit=" << price_time_limit()
        << ", counterparty=" << counterparty()
        << ", contract=" << contract() << "}";
}

Single::Single(const Contract& contract,
               const Side& side,
               const Quantity quantity,
               const PriceTimeLimit& price_time_limit):
    Order(side, quantity, price_time_limit, Counterparty::PARTICIPANT),
    contract_(contract) {
}

void Bundle::print(std::ostream* const p_os) const {
  *p_os << "{\"name\":\"bundleOrder\""
        << ", \"side\":" << side()
        << ", \"quantity\":" << quantity()
        << ", \"priceTimeLimit\":" << price_time_limit()
        << ", \"counterparty\":" << counterparty()
        << ", \"contractBundle\":" << contract_bundle() << "}";
}

Bundle::Bundle(const ContractBundle& contract_bundle,
               const Side& side,
               const Quantity quantity,
               const Counterparty& counterparty):
    Order(side, quantity, PriceTimeLimit(), counterparty),
    contract_bundle_(contract_bundle) {
}

}  // namespace iem
