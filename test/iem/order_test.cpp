// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/order.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(OrderTest, Constructor) {

const Contract c("FRup1116");
const PriceTimeLimit ptl(Price(1), 0);
const Single& o(c, Side::buy, 1, );
EXPECT_THROW(PriceTimeLimit(Price(999), boost::date_time::not_a_date_time),
    std::invalid_argument);

}

}  // namespace iem