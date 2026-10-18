// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

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

TEST(SessionTest, PriceParser) {
  auto px = _parse_price("");
  EXPECT_EQ(px, nanPrice());
}

TEST(SessionTest, PositionParser) {
  auto qty = _parse_quantity("");
  EXPECT_EQ(qty, 0);
}

// Session tests below run against FakeHttpClient: no network.

constexpr auto kCookie("JSESSIONID=abc123; Path=/iem");
constexpr auto kLocation("/iem/trader/MarketSelection.action");

HttpResponse login_redirect() {
  return HttpResponse{
      302, {{"Set-Cookie", kCookie}, {"Location", kLocation}}, ""};
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
            url_encode({{"market", "contract.market"},
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
  // No price and no expiration: immediate or cancel, sent as a market order
  const PriceTimeLimit ioc(nanPrice(), boost::posix_time::not_a_date_time);
  const Single o(c, Side::SELL, 2, ioc);

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

  const auto response = s.cancel_order(o);

  EXPECT_TRUE(server.requests.empty());
  EXPECT_EQ(response.status, 0);
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

}  // namespace iem
