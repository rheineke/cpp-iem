// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/http_client.hpp"

#include <string>
#include <thread>

#include "gtest/gtest.h"
#include "httplib.h"

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

// The tests below run the real client against an HTTP server on localhost.

class LocalServer {
 public:
  LocalServer() {
    port_ = server.bind_to_any_port("127.0.0.1");
    thread_ = std::thread([this] { server.listen_after_bind(); });
    server.wait_until_ready();
  }
  ~LocalServer() {
    server.stop();
    thread_.join();
  }
  std::string url() const {
    return "http://127.0.0.1:" + std::to_string(port_);
  }

  httplib::Server server;

 private:
  int port_;
  std::thread thread_;
};

HttpRequest post(const std::string& path, const std::string& body) {
  return HttpRequest{HttpMethod::POST,
                     path,
                     {{"Cookie", "JSESSIONID=abc123"},
                      {"Content-Type", "application/x-www-form-urlencoded"}},
                     body};
}

std::string header(const NameValuePairs& headers, const std::string& name) {
  for (const auto& h : headers) {
    if (h.first == name) {
      return h.second;
    }
  }
  return "";
}

TEST(HttpClientTest, PostSendsMethodHeadersAndBody) {
  httplib::Request seen;
  LocalServer local;
  local.server.Post("/iem/trader/TraderLogin.action",
                    [&](const httplib::Request& req, httplib::Response& res) {
                      seen = req;
                      res.set_content("ok", "text/plain");
                    });

  const auto response = make_http_client(local.url())
                            ->send(post("/iem/trader/TraderLogin.action",
                                        "username=foo&loginSubmit=Sign%20in"));

  EXPECT_EQ(response.status, 200);
  EXPECT_EQ(response.body, "ok");
  EXPECT_EQ(seen.method, "POST");
  EXPECT_EQ(seen.body, "username=foo&loginSubmit=Sign%20in");
  EXPECT_EQ(seen.get_header_value("Cookie"), "JSESSIONID=abc123");
  EXPECT_EQ(seen.get_header_value("Content-Type"),
            "application/x-www-form-urlencoded");
  EXPECT_EQ(seen.get_header_value("Content-Length"), "34");
}

TEST(HttpClientTest, QueryIsSentAsEncoded) {
  httplib::Request seen;
  LocalServer local;
  local.server.Get("/iem/trader/TraderMessages.action",
                   [&](const httplib::Request& req, httplib::Response& res) {
                     seen = req;
                     res.set_content("", "text/html");
                   });

  make_http_client(local.url())
      ->send(HttpRequest{HttpMethod::GET,
                         "/iem/trader/TraderMessages.action?home=&market=51"
                         "&note=a%20b%26c",
                         {},
                         ""});

  EXPECT_EQ(seen.method, "GET");
  EXPECT_TRUE(seen.has_param("home"));
  EXPECT_EQ(seen.get_param_value("market"), "51");
  // Decoded once by the server, so the client did not encode it again
  EXPECT_EQ(seen.get_param_value("note"), "a b&c");
}

TEST(HttpClientTest, RedirectIsReturnedNotFollowed) {
  LocalServer local;
  local.server.Post(
      "/login", [](const httplib::Request&, httplib::Response& res) {
        res.status = 302;
        res.set_header("Location", "/next");
        res.headers.emplace("Set-Cookie", "JSESSIONID=abc123; Path=/iem");
        res.headers.emplace("Set-Cookie", "AWSALB=xyz");
      });
  local.server.Get("/next",
                   [](const httplib::Request&, httplib::Response& res) {
                     res.set_content("followed", "text/plain");
                   });

  const auto response =
      make_http_client(local.url())->send(post("/login", "a=1"));

  EXPECT_EQ(response.status, 302);
  EXPECT_EQ(header(response.headers, "Location"), "/next");
  int set_cookies = 0;
  for (const auto& h : response.headers) {
    set_cookies += (h.first == "Set-Cookie");
  }
  EXPECT_EQ(set_cookies, 2);
}

TEST(HttpClientTest, GzipResponseIsDecompressed) {
  const std::string page(100000, 'x');
  std::string accept_encoding;
  LocalServer local;
  local.server.Get("/page", [&](const httplib::Request& req,
                                httplib::Response& res) {
    accept_encoding = req.get_header_value("Accept-Encoding");
    res.set_content(page, "text/html");  // gzipped when the client accepts it
  });

  const auto response =
      make_http_client(local.url())
          ->send(HttpRequest{
              HttpMethod::GET, "/page", {{"Accept-Encoding", "gzip"}}, ""});

  EXPECT_EQ(accept_encoding, "gzip");
  EXPECT_EQ(header(response.headers, "Content-Encoding"), "gzip");
  EXPECT_EQ(response.body, page);
}

TEST(HttpClientTest, NoServerThrows) {
  std::string url;
  {
    LocalServer local;
    url = local.url();
  }  // Server stopped: nothing listens on the port now
  EXPECT_THROW(make_http_client(url)->send(post("/", "")), std::runtime_error);
}

}  // namespace iem
