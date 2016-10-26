// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/pricetimelimit.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(PriceTimeLimitTest, ValidConstructorArguments) {
  EXPECT_THROW(PriceTimeLimit(Price(999), boost::date_time::not_a_date_time),
               std::invalid_argument);
}

TEST(PriceTimeLimitTest, IOC) {
  EXPECT_TRUE(PriceTimeLimit().ioc());

  const auto dt = boost::gregorian::date(2016, boost::date_time::Jan, 10);
  const auto t = boost::posix_time::time_duration(1, 2, 3);
  const auto expiration = boost::posix_time::ptime(dt, t);
  EXPECT_FALSE(PriceTimeLimit(Price(999), expiration).ioc());
}

}
