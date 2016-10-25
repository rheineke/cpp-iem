// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(PriceTimeLimitTest, Constructor) {
EXPECT_THROW(PriceTimeLimit(Price(999), boost::date_time::not_a_date_time),
std::invalid_argument);
}

}
