// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
//

#ifndef CPPIEM_FATFINGER_STRATEGY_HPP_
#define CPPIEM_FATFINGER_STRATEGY_HPP_

#include <memory>
#include <vector>

#include "iem/order.hpp"

namespace iem {

std::vector<std::unique_ptr<Order>> fatfinger_orders(const Market market,
                                                     const Quantity qty);

}  // namespace iem


#endif //CPPIEM_FATFINGER_STRATEGY_HPP_
