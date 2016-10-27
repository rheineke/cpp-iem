// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "iem/price.hpp"

#include "gtest/gtest.h"

// #include "spacemeasure_test.hpp"

namespace iem {

//TEST(PriceTest, SpaceMeasure) {
//  constexpr std::size_t price_align_size = 16;
//  EXPECT_EQ(price_align_size, alignSize<Price>());
//}

TEST(PriceTest, NoThrowMovable) {
  EXPECT_TRUE(std::is_nothrow_move_constructible<Price>::value);
}

TEST(PriceTest, ConstructorException) {
  ASSERT_THROW(Price(NanTick - 1), std::invalid_argument);
}

TEST(PriceTest, MethodTicksSetsCorrectly) {
  Price p;
  const Ticks t = 450;
  p.set(t);
  EXPECT_EQ(t, p.ticks());
}

TEST(PriceTest, TicksPerPoint) {
  Price p;
  EXPECT_EQ(1000, p.ticks_per_point());
}

TEST(PriceTest, SessionStringFormat) {
  Price px(999);
  // Test snprint_price directly
  constexpr std::size_t kLen = 16;
  char c_str[kLen];
  snprintf_price(c_str, kLen, px);
  EXPECT_EQ(".999", ".999");
  // Test to_string
  EXPECT_EQ(to_string(px), "0.999");
}

}  // namespace iem