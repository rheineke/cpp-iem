// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/http_client.hpp"

#include "gtest/gtest.h"

namespace iem {

TEST(HttpClientTest, UrlEncodeKeepsUnreservedCharacters) {
  EXPECT_EQ(url_encode({{"aZ09", "-._~"}}), "aZ09=-._~");
}

TEST(HttpClientTest, UrlEncodeEscapesEverythingElse) {
  EXPECT_EQ(url_encode({{"a b", "x&y=z"}, {"loginSubmit", "Sign in"}}),
            "a%20b=x%26y%3Dz&loginSubmit=Sign%20in");
  EXPECT_EQ(url_encode({{"p", "100%/?#+"}}), "p=100%25%2F%3F%23%2B");
}

TEST(HttpClientTest, UrlEncodeEmptyValues) {
  EXPECT_EQ(url_encode({}), "");
  EXPECT_EQ(url_encode({{"logout", ""}}), "logout=");
}

}  // namespace iem
