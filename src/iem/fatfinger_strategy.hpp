// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#ifndef CPPIEM_IEM_FATFINGER_STRATEGY_HPP_
#define CPPIEM_IEM_FATFINGER_STRATEGY_HPP_

#include <memory>
#include <vector>

#include "iem/order.hpp"

namespace iem {

Orders fatfinger_orders(const Market& market,
                        const std::string& expiry_date,
                        const Quantity qty);

}  // namespace iem


#endif  // CPPIEM_IEM_FATFINGER_STRATEGY_HPP_
