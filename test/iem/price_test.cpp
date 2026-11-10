// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "iem/price.hpp"

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

// #include "spacemeasure_test.hpp"

namespace iem {

// TEST(PriceTest, SpaceMeasure) {
//   constexpr std::size_t price_align_size = 16;
//   EXPECT_EQ(price_align_size, alignSize<Price>());
// }

TEST(PriceTest, NoThrowMovable) {
  EXPECT_TRUE(std::is_nothrow_move_constructible<Price>::value);
}

TEST(PriceTest, ConstructorException) {
  ASSERT_THROW(Price(NanTick - 1), std::invalid_argument);
}

TEST(PriceTest, ConstructorExceptionMessage) {
  try {
    Price(1001);
    FAIL() << "Expected std::invalid_argument";
  } catch (const std::invalid_argument& e) {
    EXPECT_EQ(std::string(e.what()), "Expected price in ticks [" +
                                         std::to_string(NanTick) +
                                         ", 1000] but received 1001");
  }
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
  std::map<Price, std::string> price_expected_str_map{
      {Price(999), "0.999"},
      {Price(), "----"},
  };

  constexpr std::size_t kLen = 8;
  char c_str[kLen];

  for (const auto& it : price_expected_str_map) {
    // Test snprint_price directly
    snprintf_price(c_str, kLen, it.first);
    EXPECT_EQ(std::string(c_str), it.second);

    // Test to_string
    EXPECT_EQ(to_string(it.first), it.second);
  }
}

TEST(PriceTest, StreamOutput) {
  std::ostringstream os;
  os << Price(12) << ' ' << Price();
  EXPECT_EQ(os.str(), "0.012 ----");
}

TEST(PriceTest, StreamInput) {
  Price p;
  std::istringstream("0.250") >> p;
  EXPECT_EQ(p, Price(250));
  std::istringstream("-") >> p;
  EXPECT_EQ(p, nanPrice());
}

TEST(PriceTest, StreamInputFromEmptyStream) {
  Price p(5);
  std::istringstream is("");
  is >> p;
  EXPECT_TRUE(is.fail());
  EXPECT_EQ(p, Price(5));
}

}  // namespace iem
