// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "strategy/common.hpp"

namespace strategy::common {

iem::Bundle fixed(const iem::ContractBundle& c_b,
                  const iem::Side& s,
                  const iem::Quantity qty) {
  return iem::Bundle(c_b, s, qty, iem::Counterparty::exchange);
}

void signalPtrDiff(const iem::Signal* lhs, const iem::Signal* rhs) {
  using boost::typeindex::type_id_with_cvr;

  const auto ptr_diff =
      reinterpret_cast<std::size_t>(rhs) - reinterpret_cast<std::size_t>(lhs);
  std::cout << "Signal pointer difference: "
            << type_id_with_cvr<decltype(*lhs)>().pretty_name()
            << " "
            << ptr_diff
            << std::endl;
}

} // namespace strategy::common
