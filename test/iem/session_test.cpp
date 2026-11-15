// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "boost/property_tree/xml_parser.hpp"
#include "fake_http_client.hpp"
#include "gtest/gtest.h"
#include "iem_pages.hpp"

namespace iem {

TEST(SessionTest, Session) {
  const auto usr = "foo";
  const auto pwd = "bar";
  const Session s(usr, pwd);
  EXPECT_EQ(s.username(), usr);
  EXPECT_EQ(s.password(), pwd);
  EXPECT_EQ(s.cookie(), "");
}

TEST(SessionTest, LogLineHasNoSecrets) {
  const Session s("foo", "secret-password");
  char buf[256];

  snprintf_session(buf, sizeof(buf), s);

  const std::string line(buf);
  EXPECT_EQ(line,
            "{\"name\":\"session\", \"username\":\"foo\", "
            "\"logged_in\":false}");
  EXPECT_EQ(line.find("secret-password"), std::string::npos);
}

TEST(SessionTest, LogLineTruncatesToBuffer) {
  const Session s("foo", "bar");
  char buf[8];

  snprintf_session(buf, sizeof(buf), s);

  EXPECT_EQ(std::string(buf), "{\"name\"");
}

TEST(SessionTest, PriceParser) {
  EXPECT_EQ(_parse_price(""), nanPrice());
  EXPECT_EQ(_parse_price(" - "), nanPrice());
  for (const auto* text : {"0.512", " 0.512 ", "0.512*", "0.512 *"}) {
    EXPECT_EQ(_parse_price(text), Price(512)) << text;
  }
  for (const auto* bad : {"0.5 0.6", "0.5 x", "x"}) {
    EXPECT_THROW(_parse_price(bad), std::invalid_argument) << bad;
  }
}

TEST(SessionTest, PositionParser) {
  EXPECT_EQ(_parse_quantity(""), 0);
  EXPECT_EQ(_parse_quantity("300"), 300);  // was read as 44
  EXPECT_EQ(_parse_quantity("-12"), -12);  // a short position
  EXPECT_THROW(_parse_quantity("12abc"), std::invalid_argument);
  EXPECT_EQ(_parse_quantity("37.000000"), 37);  // how IEM writes it
  EXPECT_THROW(_parse_quantity("1.5"), std::invalid_argument);
  EXPECT_EQ(_parse_quantity(" 3 "), 3);
  EXPECT_EQ(_parse_quantity("  "), 0);
  EXPECT_EQ(_parse_order_quantity("50000"), 50000);
  EXPECT_THROW(_parse_order_quantity("-1"), std::out_of_range);  // was 255
  EXPECT_THROW(_parse_order_quantity("0"), std::out_of_range);
}

TEST(SessionTest, TableEndsAtItsOwnEndTag) {
  const std::string body =
      "<html><table><tbody></tbody></table><script>var table = 1;</script>"
      "</html>";
  EXPECT_EQ(_table_html_string(body), "<table><tbody></tbody></table>");
  EXPECT_THROW(_table_html_string("<html>logged out</html>"),
               std::runtime_error);
}

// Session tests below run against FakeHttpClient: no network.

constexpr auto kSetCookie("JSESSIONID=abc123; Path=/iem; Secure; HttpOnly");
// What Session sends back: name=value only
constexpr auto kCookie("JSESSIONID=abc123");
constexpr auto kLocation("/iem/trader/MarketSelection.action");

HttpResponse login_redirect() {
  return HttpResponse{
      302, {{"Set-Cookie", kSetCookie}, {"Location", kLocation}}, ""};
}

// Session that has logged in; the login requests are cleared from server
Session logged_in_session(FakeServer* server) {
  Session s("foo", "bar", make_fake_client(server));
  server->responses = {login_redirect(), ok("")};
  s.authenticate();
  server->requests.clear();
  return s;
}

TEST(SessionTest, AuthenticatePostsLoginForm) {
  FakeServer server;
  server.responses = {login_redirect(), ok("market select")};
  Session s("foo", "bar", make_fake_client(&server));

  const auto response = s.authenticate();

  ASSERT_EQ(server.requests.size(), 2u);
  const auto& login = server.requests[0];
  EXPECT_EQ(login.method, HttpMethod::POST);
  EXPECT_EQ(login.path, "/iem/trader/TraderLogin.action");
  EXPECT_EQ(login.body,
            "forceLogin=false&username=foo&password=bar"
            "&loginSubmit=Sign%20in&_sourcePage=");
  EXPECT_EQ(header(login.headers, "Content-Type"),
            "application/x-www-form-urlencoded");
  EXPECT_EQ(header(login.headers, "Content-Length"),
            std::to_string(login.body.size()));

  EXPECT_EQ(s.cookie(), kCookie);

  // Then follows the redirect with the session cookie
  const auto& select = server.requests[1];
  EXPECT_EQ(select.method, HttpMethod::GET);
  EXPECT_EQ(select.path, kLocation);
  EXPECT_EQ(header(select.headers, "Cookie"), kCookie);
  // Set-Cookie is a response header; it does not belong on a request
  EXPECT_EQ(header(select.headers, "Set-Cookie"), "");
  EXPECT_EQ(response.body, "market select");
}

TEST(SessionTest, AuthenticateForcesLoginWhenNotRedirected) {
  FakeServer server;
  server.responses = {ok("already logged in elsewhere"), login_redirect(),
                      ok("")};
  Session s("foo", "bar", make_fake_client(&server));

  s.authenticate();

  ASSERT_EQ(server.requests.size(), 3u);
  EXPECT_EQ(server.requests[0].body.find("forceLogin=false"), 0u);
  EXPECT_EQ(server.requests[1].body.find("forceLogin=true"), 0u);
  EXPECT_EQ(server.requests[2].path, kLocation);
  EXPECT_EQ(s.cookie(), kCookie);
}

TEST(SessionTest, AuthenticateKeepsEveryCookie) {
  FakeServer server;
  // Lowercase names, as HTTP/2 servers and some proxies send them
  server.responses = {
      HttpResponse{302,
                   {{"set-cookie", "JSESSIONID=abc123; Path=/iem"},
                    {"set-cookie", "AWSALB=xyz; Expires=Wed, 21 Oct 2026"},
                    {"location", kLocation}},
                   ""},
      ok("")};
  Session s("foo", "bar", make_fake_client(&server));

  s.authenticate();

  EXPECT_EQ(s.cookie(), "JSESSIONID=abc123; AWSALB=xyz");
  ASSERT_EQ(server.requests.size(), 2u);
  EXPECT_EQ(server.requests[1].path, kLocation);
}

TEST(SessionTest, AuthenticateFollowsAbsoluteLocation) {
  FakeServer server;
  server.responses = {HttpResponse{302,
                                   {{"Set-Cookie", kSetCookie},
                                    {"Location",
                                     "https://iem.uiowa.edu/iem/trader/"
                                     "MarketSelection.action?a=1"}},
                                   ""},
                      ok("")};
  Session s("foo", "bar", make_fake_client(&server));

  s.authenticate();

  ASSERT_EQ(server.requests.size(), 2u);
  EXPECT_EQ(server.requests[1].path, "/iem/trader/MarketSelection.action?a=1");
}

TEST(SessionTest, LocationPath) {
  EXPECT_EQ(_location_path("/iem/a.action?x=1"), "/iem/a.action?x=1");
  EXPECT_EQ(_location_path("https://iem.uiowa.edu/iem/a.action"),
            "/iem/a.action");
  EXPECT_EQ(_location_path("https://iem.uiowa.edu"), "/");
  EXPECT_EQ(_location_path(""), "");
}

TEST(SessionTest, LogoutClearsCookieOnSuccess) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};

  s.logout();

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].method, HttpMethod::GET);
  EXPECT_EQ(server.requests[0].path, "/iem/trader/TraderLogin.action?logout=");
  EXPECT_EQ(header(server.requests[0].headers, "Cookie"), kCookie);
  EXPECT_EQ(s.cookie(), "");
}

TEST(SessionTest, LogoutKeepsCookieOnFailure) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {HttpResponse{500, {}, ""}};

  s.logout();

  EXPECT_EQ(s.cookie(), kCookie);
}

TEST(SessionTest, OrderBook) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok(pages::kOrderBook)};
  const Market mkt("FedPolicyB");

  const auto obs = s.orderbook(mkt);

  ASSERT_EQ(server.requests.size(), 1u);
  const auto& request = server.requests[0];
  EXPECT_EQ(request.method, HttpMethod::POST);
  EXPECT_EQ(request.path, "/iem/trader/MarketTrader.action");
  EXPECT_EQ(request.body, "market=" + std::to_string(mkt.value()));
  EXPECT_EQ(header(request.headers, "Cookie"), kCookie);

  ASSERT_EQ(obs.size(), 2u);
  EXPECT_EQ(obs[0].contract().asset_id(),
            Contract(mkt.name(), "FRdown1216").asset_id());
  EXPECT_EQ(obs[0].best_price(Side::BUY), Price(12));
  EXPECT_TRUE(obs[0].best_price_priority(Side::BUY));
  EXPECT_EQ(obs[0].best_price(Side::SELL), Price(20));
  EXPECT_FALSE(obs[0].best_price_priority(Side::SELL));
  EXPECT_EQ(obs[0].last_trade(), Price(15));
  EXPECT_EQ(obs[0].position(), 3);
  EXPECT_EQ(obs[0].num_orders(Side::BUY), 2u);
  EXPECT_EQ(obs[0].num_orders(Side::SELL), 1u);

  EXPECT_EQ(obs[1].contract().asset_id(),
            Contract(mkt.name(), "FRsame1216").asset_id());
  EXPECT_EQ(obs[1].best_price(Side::BUY), nanPrice());
  EXPECT_EQ(obs[1].best_price(Side::SELL), Price(990));
  EXPECT_TRUE(obs[1].best_price_priority(Side::SELL));
  EXPECT_EQ(obs[1].position(), 0);
  EXPECT_EQ(obs[1].num_orders(Side::SELL), 0u);
}

TEST(SessionTest, OrderBookWithoutTableThrows) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("<html><body>Session expired</body></html>")};

  EXPECT_ANY_THROW(s.orderbook(Market("FedPolicyB")));
}

TEST(SessionTest, OutstandingOrders) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok(pages::kOutstandingBids)};
  const Contract c("FedPolicyB", "FRup1216");

  const auto orders = s.outstanding_orders(c, Side::BUY);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].path, "/iem/trader/TraderActivity.action");
  EXPECT_EQ(server.requests[0].body,
            url_encode({{"market", std::to_string(c.market().value())},
                        {"asset", std::to_string(c.asset_id())},
                        {"activityType", "bid"}}));

  ASSERT_EQ(orders.size(), 1u);
  EXPECT_EQ(orders[0].id(), 4700000);
  EXPECT_EQ(orders[0].side(), Side::BUY);
  EXPECT_EQ(orders[0].quantity(), 1);
  EXPECT_EQ(orders[0].price_time_limit().price(), Price(1));
  EXPECT_EQ(orders[0].contract().asset_id(), c.asset_id());
}

TEST(SessionTest, Holdings) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok(pages::kHoldings)};
  const Contract c("FedPolicyB", "FRsame1216");

  const auto msgs = s.holdings(c);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].body,
            url_encode({{"market", std::to_string(c.market().value())},
                        {"asset", std::to_string(c.asset_id())},
                        {"activityType", "holdings"},
                        {"viewAssetHoldings", "25"}}));
  ASSERT_EQ(msgs.size(), 1u);
  EXPECT_EQ(msgs[0].market_label(), "FedPolicyB");
  EXPECT_EQ(msgs[0].action(), Action::BUY);
  EXPECT_EQ(msgs[0].quantity(), 5);
  EXPECT_EQ(msgs[0].price(), Price(950));
}

TEST(SessionTest, Messages) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok(pages::kTraderMessages)};
  const Market mkt("FedPolicyB");

  const auto msgs = s.messages(mkt);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].method, HttpMethod::GET);
  EXPECT_EQ(server.requests[0].path,
            "/iem/trader/TraderMessages.action?home=&market=" +
                std::to_string(mkt.value()));
  EXPECT_EQ(header(server.requests[0].headers, "Cookie"), kCookie);
  ASSERT_EQ(msgs.size(), 1u);
  EXPECT_EQ(msgs[0].type(), MessageType::ORDER);
  EXPECT_EQ(msgs[0].contract(), "FRsame1216");
  EXPECT_EQ(msgs[0].action(), Action::BID_ENTERED);
  EXPECT_EQ(msgs[0].quantity(), 2);
  EXPECT_EQ(msgs[0].unit_price(), Price(500));
  EXPECT_EQ(msgs[0].expiration_date(), boost::posix_time::not_a_date_time);
}

TEST(SessionTest, PlaceLimitOrder) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("placed")};
  const Contract c("FedPolicyB", "FRsame1216");
  const PriceTimeLimit ptl(Price(1), boost::posix_time::not_a_date_time);
  const Single o(c, Side::BUY, 1, ptl);

  const auto response = s.place_order(o);

  ASSERT_EQ(server.requests.size(), 1u);
  const auto& request = server.requests[0];
  EXPECT_EQ(request.method, HttpMethod::POST);
  EXPECT_EQ(request.path, "/iem/trader/order/LimitOrder.action");
  EXPECT_EQ(
      request.body,
      url_encode({
          {"limitOrderAssetToMarket", std::to_string(c.asset_to_market_id())},
          {"orderType", "bid"},
          {"expirationDate", "No expiration"},
          {"price", to_string(Price(1))},
          {"limitOrderQuantity", "1"},
          {"placeLimitOrder", "Place Limit Order"},
          {"market", std::to_string(c.market().value())},
          {"_sourcePage", ""},
      }));
  EXPECT_EQ(header(request.headers, "Cookie"), kCookie);
  EXPECT_EQ(response.body, "placed");
}

TEST(SessionTest, PlaceMarketOrder) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  const Contract c("FedPolicyB", "FRsame1216");
  const Single o(c, Side::SELL, 2, PriceTimeLimit::market());

  s.place_order(o);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].path, "/iem/trader/order/MarketOrder.action");
  EXPECT_EQ(
      server.requests[0].body,
      url_encode({
          {"limitOrderAssetToMarket", std::to_string(c.asset_to_market_id())},
          {"orderType", "sell"},
          {"marketOrderQuantity", "2"},
          {"placeMarketOrder", "Place Market Order"},
          {"market", std::to_string(c.market().value())},
      }));
}

TEST(SessionTest, PlaceBundleOrder) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  const ContractBundle cb("FedPolicyB", MonthYear(boost::gregorian::Dec, 16));
  const Bundle b(cb, Side::BUY, 1, Counterparty::EXCHANGE);

  s.place_order(b);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].path, "/iem/trader/order/BundleOrder.action");
  EXPECT_EQ(server.requests[0].body,
            url_encode({
                {"bundle", std::to_string(cb.bundle_id())},
                {"orderType", "buyAtFixed"},
                {"bundleOrderQuantity", "1"},
                {"placeBundleOrder", "Place Bundle Order"},
                {"market", std::to_string(cb.market().value())},
            }));
}

TEST(SessionTest, CancelOrderWithoutIdSendsNothing) {
  FakeServer server;
  auto s = logged_in_session(&server);
  const Contract c("FedPolicyB", "FRup1216");
  const Single o(c, Side::BUY, 1,
                 PriceTimeLimit(Price(1), boost::posix_time::not_a_date_time));

  EXPECT_THROW(s.cancel_order(o), std::invalid_argument);
  EXPECT_TRUE(server.requests.empty());
}

TEST(SessionTest, CancelOrder) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  const Contract c("FedPolicyB", "FRup1216");
  Single o(c, Side::BUY, 1,
           PriceTimeLimit(Price(1), boost::posix_time::not_a_date_time));
  o.set_id(4700000);

  s.cancel_order(o);

  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_EQ(server.requests[0].method, HttpMethod::GET);
  EXPECT_EQ(server.requests[0].path,
            "/iem/trader/TraderActivity.action?" +
                url_encode({{"cancelBidOrder", ""},
                            {"market", std::to_string(c.market().value())},
                            {"bidOrder", "4700000"},
                            {"asset", std::to_string(c.asset_id())},
                            {"activityType", "bid"}}));
  EXPECT_EQ(header(server.requests[0].headers, "Cookie"), kCookie);
}

struct LimitCase {
  Side side;
  Quantity quantity;
  std::string expiration;  // "" for none
  std::string order_type;
};

TEST(SessionTest, LimitOrdersCarrySideQuantityAndExpiry) {
  const Contract c("FedPolicyB", "FRsame1216");
  for (const auto& lc :
       std::vector<LimitCase>{{Side::BUY, 1, "", "bid"},
                              {Side::SELL, 37, "", "ask"},
                              {Side::BUY, 37, "2016-12-14 23:59:00", "bid"},
                              {Side::SELL, 1, "2016-12-14 23:59:00", "ask"}}) {
    FakeServer server;
    auto s = logged_in_session(&server);
    server.responses = {ok("")};
    const auto expiry =
        lc.expiration.empty()
            ? boost::posix_time::ptime(boost::posix_time::not_a_date_time)
            : boost::posix_time::time_from_string(lc.expiration);
    const PriceTimeLimit ptl(Price(512), expiry);
    s.place_order(Single(c, lc.side, lc.quantity, ptl));

    ASSERT_EQ(server.requests.size(), 1u);
    const auto& body = server.requests[0].body;
    EXPECT_NE(body.find("orderType=" + lc.order_type), std::string::npos);
    EXPECT_NE(body.find("limitOrderQuantity=" + std::to_string(lc.quantity)),
              std::string::npos);
    EXPECT_NE(body.find("price=0.512"), std::string::npos);
    EXPECT_NE(body.find("expirationDate=" +
                        url_encode({{"", to_string(expiry)}}).substr(1)),
              std::string::npos)
        << body;
  }
}

TEST(SessionTest, MarketBuyIsSentAsBuy) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  const Contract c("FedPolicyB", "FRsame1216");
  s.place_order(Single(c, Side::BUY, 37, PriceTimeLimit::market()));
  const auto& body = server.requests[0].body;
  EXPECT_NE(body.find("orderType=buy"), std::string::npos);
  EXPECT_NE(body.find("marketOrderQuantity=37"), std::string::npos);
}

TEST(SessionTest, BundleOrdersNameSideAndCounterparty) {
  const ContractBundle cb("FedPolicyB", MonthYear(boost::gregorian::Dec, 16));
  for (const auto& [side, cp, type] :
       std::vector<std::tuple<Side, Counterparty, std::string>>{
           {Side::BUY, Counterparty::EXCHANGE, "buyAtFixed"},
           {Side::SELL, Counterparty::EXCHANGE, "sellAtFixed"},
           {Side::BUY, Counterparty::PARTICIPANT, "buyAtMarket"},
           {Side::SELL, Counterparty::PARTICIPANT, "sellAtMarket"}}) {
    FakeServer server;
    auto s = logged_in_session(&server);
    server.responses = {ok("")};
    s.place_order(Bundle(cb, side, 12, cp));
    const auto& body = server.requests[0].body;
    EXPECT_NE(body.find("orderType=" + type), std::string::npos) << type;
    EXPECT_NE(body.find("bundleOrderQuantity=12"), std::string::npos);
  }
}

TEST(SessionTest, AskCancelUsesTheAskFields) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  const Contract c("FedPolicyB", "FRup1216");
  Single o(c, Side::SELL, 1,
           PriceTimeLimit(Price(1), boost::posix_time::not_a_date_time));
  o.set_id(4700001);
  s.cancel_order(o);
  const auto& path = server.requests[0].path;
  EXPECT_NE(path.find("cancelAskOrder="), std::string::npos);
  EXPECT_NE(path.find("askOrder=4700001"), std::string::npos);
  EXPECT_NE(path.find("activityType=ask"), std::string::npos);
}

TEST(SessionTest, LimitPricesOutsideTheMarketAreRefused) {
  FakeServer server;
  auto s = logged_in_session(&server);
  const Contract c("FedPolicyB", "FRsame1216");
  for (const Ticks bad : {Ticks{0}, Ticks{-1}}) {
    const PriceTimeLimit ptl(Price(bad), boost::posix_time::not_a_date_time);
    EXPECT_THROW(s.place_order(Single(c, Side::BUY, 1, ptl)),
                 std::invalid_argument);
  }
  EXPECT_TRUE(server.requests.empty());
}

TEST(SessionTest, FailedResponsesSayWhetherAnOrderMayHaveBeenPlaced) {
  const Contract c("FedPolicyB", "FRsame1216");
  const Single o(
      c, Side::BUY, 1,
      PriceTimeLimit(Price(512), boost::posix_time::not_a_date_time));
  const auto place = [&o](const HttpResponse& response) {
    FakeServer server;
    auto s = logged_in_session(&server);
    server.responses = {response};
    s.place_order(o);
  };
  // A server or gateway error may come after the order was placed
  EXPECT_THROW(place({500, {}, "error"}), HttpOutcomeUnknown);
  EXPECT_THROW(place({502, {}, "bad gateway"}), HttpOutcomeUnknown);
  EXPECT_THROW(
      place({302, {{"Location", "/iem/trader/TraderLogin.action"}}, ""}),
      SessionExpired);
  // Refused: nothing was done, and the session is still good
  std::string thrown = "nothing";
  try {
    place({403, {}, "forbidden"});
  } catch (const HttpOutcomeUnknown&) {
    thrown = "HttpOutcomeUnknown";
  } catch (const SessionExpired&) {
    thrown = "SessionExpired";
  } catch (const std::runtime_error&) {
    thrown = "runtime_error";
  }
  EXPECT_EQ(thrown, "runtime_error");
}

TEST(SessionTest, EveryRequestNoticesAnExpiredSession) {
  const HttpResponse expired{
      302, {{"Location", "/iem/trader/TraderLogin.action"}}, ""};
  const Market m("FedPolicyB");
  const Contract c("FedPolicyB", "FRup1216");
  Single o(c, Side::BUY, 1,
           PriceTimeLimit(Price(1), boost::posix_time::not_a_date_time));
  o.set_id(4700001);
  const std::vector<std::pair<std::string, std::function<void(Session&)>>>
      calls{
          {"orderbook", [&](Session& s) { s.orderbook(m); }},
          {"holdings", [&](Session& s) { s.holdings(c); }},
          {"outstanding_orders",
           [&](Session& s) { s.outstanding_orders(c, Side::BUY); }},
          {"cancel_order", [&](Session& s) { s.cancel_order(o); }},
          {"messages", [&](Session& s) { s.messages(m); }},
          {"remove_messages", [&](Session& s) { s.remove_messages(m); }},
          {"portfolio", [&](Session& s) { s.portfolio(m); }},
  };
  for (const auto& [name, call] : calls) {
    FakeServer server;
    auto s = logged_in_session(&server);
    server.responses = {expired};
    EXPECT_THROW(call(s), SessionExpired) << name;
  }
}

TEST(SessionTest, MissingPriceIsNotSentAsAMarketOrder) {
  FakeServer server;
  auto s = logged_in_session(&server);
  // An empty book side gives no price: that must not become a market order
  const PriceTimeLimit no_price(nanPrice(), boost::posix_time::not_a_date_time);
  EXPECT_THROW(s.place_order(Single(Contract("FedPolicyB", "FRsame1216"),
                                    Side::BUY, 1, no_price)),
               std::invalid_argument);
  EXPECT_TRUE(server.requests.empty());
}

TEST(SessionTest, OversizedOrdersAreNotSent) {
  FakeServer server;
  auto s = logged_in_session(&server);
  const Contract c("FedPolicyB", "FRsame1216");
  const PriceTimeLimit ptl(Price(512), boost::posix_time::not_a_date_time);
  EXPECT_THROW(s.place_order(Single(c, Side::BUY, kMaxOrderQuantity + 1, ptl)),
               std::invalid_argument);
  const ContractBundle cb("FedPolicyB", MonthYear(boost::gregorian::Dec, 16));
  EXPECT_THROW(s.place_order(Bundle(cb, Side::BUY, kMaxOrderQuantity + 1,
                                    Counterparty::EXCHANGE)),
               std::invalid_argument);
  EXPECT_TRUE(server.requests.empty());
}

TEST(SessionTest, AuthenticateFailsWhenEvenAForcedLoginIsRefused) {
  FakeServer server;
  server.responses = {ok("bad password"), ok("bad password")};
  Session s("foo", "wrong", make_fake_client(&server));
  EXPECT_THROW(s.authenticate(), std::runtime_error);
  EXPECT_EQ(server.requests.size(), 2u);
}

TEST(SessionTest, ReadTradeMessageHTML) {
  const auto s =
      "<table id=\"data-table\" class=\"table table-striped form-table "
      "jumbotronTable\" aria-describedby=\"Trader activity table\">\n"
      "<h2>Outstanding Bid: FRup1216</h2>\n"
      "<thead>\n"
      "    <tr>\n"
      "        <th><label for=\"order.date\">Order Date</label></th>\n"
      "        <th><label for=\"market\">Market</label></th>\n"
      "        <th><label for=\"contract\">Contract</label></th>\n"
      "        <th><label for=\"order.type\">Order Type</label></th>\n"
      "        <th><label for=\"quantity\">Quantity</label></th>\n"
      "        <th><label for=\"price\">Price</label></th>\n"
      "        <th><label for=\"expiration\">Expiration</label></th>\n"
      "        <th><label for=\"order.cancelOrder\">Cancel Order</label></th>\n"
      "    </tr>\n"
      "</thead>\n"
      "<tbody>\n"
      "    <tr>\n"
      "        <td>2016-11-02 20:30:54.257</td>\n"
      "        <td>FedPolicyB</td>\n"
      "        <td>FRup1216</td>\n"
      "        <td>bid</td>\n"
      "        <td>1.000000</td>\n"
      "        <td>0.001000</td>\n"
      "        <td></td>\n"
      "        <td>\n"
      "        <a data-confirm-button=\"Yes, cancel this order\" "
      "ata-cancel-button=\"No, take me back\" data-text=\"Are you sure you "
      "wish to cancel this order?\" "
      "href=\"/iem/trader/"
      "TraderActivity.action?cancelBidOrder=&amp;market=51&amp;bidOrder="
      "4700000&amp;asset=3054&amp;activityType=bid\" title=\"Cancel this "
      "order\" class=\"submitBtn confirm\"><img class=\"cancelOrderBtn\" "
      "src=\"/iem/images/x_icon_red.png\" alt=\"Cancel this bid order\"/>\n"
      "        </a>\n"
      "        </td>\n"
      "    </tr>\n"
      "</tbody>\n"
      "</table>";

  // Boost ptree works on streams only
  std::istringstream is(s);
  // ptree
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(is, pt);
  const auto tbody = pt.get_child("table.tbody");
  const auto tr_its = tbody.equal_range("tr");
  EXPECT_EQ(std::distance(tr_its.first, tr_its.second), 1);
}

TEST(SessionTest, StringsAreNotCopied) {
  const Session s("foo", "bar");
  EXPECT_TRUE((std::is_same_v<decltype(s.username()), const std::string&>));
  EXPECT_TRUE((std::is_same_v<decltype(s.password()), const std::string&>));
  EXPECT_TRUE((std::is_same_v<decltype(s.cookie()), const std::string&>));
}

// A table with one row and the given cells
std::string one_row_page(const std::string& cells) {
  return "<html><body><table class=\"table\"><tbody><tr>" + cells +
         "</tr></tbody></table></body></html>";
}

TEST(SessionTest, OrderBookRowWithMissingCells) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok(one_row_page(R"(<td class="contract">FRup1216</td>)"))};

  const auto obs = s.orderbook(Market("FedPolicyB"));

  // Missing cells read as no price, no priority and no orders
  ASSERT_EQ(obs.size(), 1u);
  EXPECT_EQ(obs[0].best_price(Side::BUY), nanPrice());
  EXPECT_FALSE(obs[0].best_price_priority(Side::BUY));
  EXPECT_EQ(obs[0].best_price(Side::SELL), nanPrice());
  EXPECT_FALSE(obs[0].best_price_priority(Side::SELL));
  EXPECT_EQ(obs[0].last_trade(), nanPrice());
  EXPECT_EQ(obs[0].position(), 0);
  EXPECT_EQ(obs[0].num_orders(Side::BUY), 0u);
  EXPECT_EQ(obs[0].num_orders(Side::SELL), 0u);
}

TEST(SessionTest, LogoutOnExitLogsOutAfterAnError) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {ok("")};
  EXPECT_THROW(
      {
        const LogoutOnExit logout(&s);
        throw std::runtime_error("a request failed");
      },
      std::runtime_error);
  ASSERT_EQ(server.requests.size(), 1u);
  EXPECT_NE(server.requests[0].path.find("logout="), std::string::npos);
  EXPECT_TRUE(s.cookie().empty());
}

TEST(SessionTest, LogoutOnExitSkipsSessionsNotLoggedIn) {
  FakeServer server;
  Session s("foo", "bar", make_fake_client(&server));
  {
    const LogoutOnExit logout(&s);
  }
  EXPECT_TRUE(server.requests.empty());
}

TEST(SessionTest, LogoutOnExitNeverThrows) {
  FakeServer server;
  auto s = logged_in_session(&server);
  // No response queued: the fake client throws, and the guard swallows it
  EXPECT_NO_THROW({ const LogoutOnExit logout(&s); });
  EXPECT_EQ(server.requests.size(), 1u);
}

TEST(SessionTest, OrderBookSkipsContractsMarketsJsonDoesNotList) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok("<html><body><table class=\"table\"><tbody>"
         "<tr><td class=\"contract\">FRnew0117</td></tr>"
         "<tr><td class=\"contract\">FRup1216</td></tr>"
         "</tbody></table></body></html>")};

  const auto obs = s.orderbook(Market("FedPolicyB"));

  ASSERT_EQ(obs.size(), 1u);
  EXPECT_EQ(obs[0].contract().asset_id(),
            Contract("FedPolicyB", "FRup1216").asset_id());
}

TEST(SessionTest, OrderBookWithNoKnownContractThrows) {
  // Not one row read: more likely the page changed than markets.json
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok(one_row_page(R"(<td class="contract">FRnew0117</td>)"))};
  EXPECT_THROW(s.orderbook(Market("FedPolicyB")), std::runtime_error);
}

TEST(SessionTest, OutstandingOrderWithoutIdCell) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok(one_row_page("<td>2016-11-02 20:30:54.257</td><td>FedPolicyB</td>"
                      "<td>FRup1216</td><td>bid</td><td>1.000000</td>"
                      "<td>0.001000</td><td></td>"))};

  const auto orders =
      s.outstanding_orders(Contract("FedPolicyB", "FRup1216"), Side::BUY);

  ASSERT_EQ(orders.size(), 1u);
  EXPECT_EQ(orders[0].id(), std::numeric_limits<OrderId>::lowest());
}

TEST(SessionTest, HoldingsRowWithoutActionThrows) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok(one_row_page("<td>2016-11-03 10:15:00.000</td><td>FedPolicyB</td>"))};

  EXPECT_THROW(s.holdings(Contract("FedPolicyB", "FRsame1216")),
               std::invalid_argument);
}

TEST(SessionTest, MessagesRowWithoutActionThrows) {
  FakeServer server;
  auto s = logged_in_session(&server);
  server.responses = {
      ok(one_row_page("<td>2016-11-04 09:00:00.000</td><td>Order</td>"))};

  EXPECT_THROW(s.messages(Market("FedPolicyB")), std::invalid_argument);
}

}  // namespace iem
