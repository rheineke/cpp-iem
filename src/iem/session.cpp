// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include <numeric>
#include <string>
#include <vector>
#include <utility>

#include "boost/property_tree/xml_parser.hpp"

namespace iem {

using ptree = boost::property_tree::ptree;
using StringPair = std::pair<std::string, std::string>;
using StringPairVector = std::vector<StringPair>;

constexpr auto _VAH = "viewAssetHoldings";

Session::Session(const std::string& username, const std::string& password):
    username_(username), password_(password), cookie_(""), client_() {
}

const std::string url_encode(const std::vector<StringPair>& nvs) {
  using boost::network::uri::encoded;
  auto op = [](const std::string& a, const StringPair& p) {
    return a.empty() ?
           encoded(p.first) + '=' + encoded(p.second) :
           a + '&' + encoded(p.first) + '=' + encoded(p.second);
  };
  return std::accumulate(nvs.begin(), nvs.end(), std::string{}, op);
}

ClientRequest buildRequest(const std::string &path,
                           const StringPairVector& name_value_pairs = {}) {
  // URI
  boost::network::uri::uri url;
  url << boost::network::uri::scheme("https")
      << boost::network::uri::host("iem.uiowa.edu")
      << boost::network::uri::path(path);
  ClientRequest request(url);
  // Add default headers
  using boost::network::header;
  // Python requests default headers
  request << header("User-Agent", "python-requests/2.10.0")
          << header("Accept-Encoding", "gzip, deflate")
          << header("Accept", "*/*");
          // << header("Connection", "keep-alive");
  // Optional body
  if (name_value_pairs.size()) {
    const auto body = url_encode(name_value_pairs);
    request << boost::network::body(body);
    request << header("Content-Length", std::to_string(body.length()))
            << header("Content-Type", "application/x-www-form-urlencoded");
  }
  return request;
}

const ClientResponse Session::authenticate(const bool force_login) {
  // Login request
  auto login_request = buildRequest(
      "/iem/trader/TraderLogin.action",
      {
          {"forceLogin", force_login ? "true" : "false"},  // Required
          {"username", this->username()},
          {"password", this->password()},
          {"loginSubmit", "Sign in"},  // Required
          {"_sourcePage", ""},  // Required
      });
  // POST request
  const auto login_response = client_.post(login_request);
  // Extract important headers - IEM requests a redirect
  std::string location;
  for (auto h : login_response.headers()) {
    if (h.first == "Set-Cookie") {
      cookie_ = h.second;
    } else if (h.first == "Location") {
      location = h.second;
    }
  }

  // Market select request
  auto market_select_request = buildRequest(location);
  // Set cookie
  market_select_request << boost::network::header("Set-Cookie", cookie());
  // GET request
  return client_.get(market_select_request);
}

const ClientResponse Session::logout() {
  // Construct request
  auto logout_request = buildRequest("/iem/trader/TraderLogin.action?logout=");
  // Set cookie
  logout_request << boost::network::header("Cookie", this->cookie());
  // GET request
  return client_.get(logout_request);
}

Price _parse_price(const std::string &px_str) {
  if (px_str.size()) {
    std::istringstream iss(px_str);
    Price p;
    iss >> p;
    return p;
  }
  return nanPrice();
}

position_t _parse_quantity(const std::string &qty_str) {
  if (qty_str.size()) {
    return std::stoi(qty_str);
  }
  return 0;
}

const ptree& empty_ptree() {
  static ptree t;
  return t;
}

unsigned int num_open_orders(ptree::const_assoc_iterator it) {
  auto obo_form = it->second.get_child("form", empty_ptree());
  std::string value;
  if (obo_form == empty_ptree()) {
    value = it->second.data();
  } else {
    // viewAssetHoldings input
    auto input_its = obo_form.equal_range("input");
    for (auto it = input_its.first; it != input_its.second; it++) {
      if (it->second.get<std::string>("<xmlattr>.name") == _VAH) {
        value = it->second.get<std::string>("<xmlattr>.value");
      }
    }
  }
  boost::trim(value);
  return std::stoi(value);
}

const OrderBook _read_html(ptree::const_assoc_iterator tr_it) {
  // Orderbook values
  std::string contract_name;
  Price bb;
  bool bbp;
  Price ba;
  bool bap;
  Price lp;
  position_t pos;
  unsigned int noo;
  unsigned int nao;

  auto td_its = tr_it->second.equal_range("td");
  const std::string& c = "contract";
  for (auto it = td_its.first; it != td_its.second; it++) {
    // XML attribute called class
    const auto& klass = it->second.get<std::string>("<xmlattr>.class", c);
    if (klass == c) {  // Contract name
      contract_name = it->second.data();
      boost::trim(contract_name);
    } else if (klass == "change-cell bestBidPrice") {  // Best bid price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      bb = _parse_price(data);
      bbp = data.find("*") != std::string::npos;
    } else if (klass == "change-cell bestAskPrice") {  // Best ask price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      ba = _parse_price(data);
      bap = data.find("*") != std::string::npos;
    } else if (klass == "change-cell lastPrice") {  // Last trade price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      lp = _parse_price(data);
    } else if (klass == "change-cell quantity") {  // Position
      auto data = it->second.get<std::string>("<xmlattr>.value");
      pos = _parse_quantity(data);
    } else if (klass == "yourBidsCell") {  // Number of open bid orders
      noo = num_open_orders(it);
    } else if (klass == "yourAsksCell") {  // Number of open ask orders
      nao = num_open_orders(it);
    }
  }

  return OrderBook(contract_name, bb, bbp, ba, bap, lp, noo, nao, pos);
}

const std::vector<OrderBook> read_html(const std::string& body) {
  // In order to increase likelihood of parsing valid XML, find substring that
  // covers all html tables. Might assume one table.
  size_t pos = body.find("<table");
  size_t len = body.rfind("table") - pos + 6;
  auto tables_str = body.substr(pos, len);
  // Boost ptree works on streams only
  std::istringstream is(tables_str);
  // ptree
  ptree pt;
  read_xml(is, pt);
  // XML nodes(terminology?) of interest
  const auto table = pt.get_child("table");
  const auto tbody = table.get_child("tbody");
  // As a test, let's get the first contract(terminology?) name
  const auto tr = tbody.get_child("tr");
  const auto tr_its = tbody.equal_range("tr");

  std::vector<OrderBook> obs;
  for (auto it = tr_its.first; it != tr_its.second; it++) {
    obs.push_back(_read_html(it));
  }
  return obs;
}

const std::vector<OrderBook> Session::market_orderbook(int market) {
  // Construct request
  auto market_orderbook_request = buildRequest(
      "/iem/trader/MarketTrader.action",
      {{"market", std::to_string(market)}}
  );
  // Set cookie
  market_orderbook_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(market_orderbook_request);
  return read_html(body(response));
}

const ClientResponse Session::asset_holdings(const Contract& contract) {
  // Construct request
  auto asset_holdings_request = buildRequest(
      "/iem/trader/TraderActivity.action",
      {
          {"market", "contract.market"},
          {"asset", std::to_string(contract.asset_id())},
          {"activityType", "holdings"},
          {_VAH, std::to_string(25)}  // Number of transactions? Required?
      });
  // Set cookie
  asset_holdings_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(asset_holdings_request);
  return response;
}

const ClientResponse Session::place_order(const Order& order) {
  // Construct request
  auto order_request = buildRequest("order/LimitOrder.action");
  // TODO(rheineke): Differentiate order types
  // Set cookie
  order_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(order_request);
  return response;
}

const std::string _limit_order_type(Side side) {
  return (side == Side::buy) ? "bid" : "ask";
}

const std::string _expiration_date(const Single& order) {
  return to_iso_extended_string(order.price_time_limit().expiration().date());
}

const ClientRequest limit_order_request(const Single& order) {
  using boost::gregorian::to_iso_extended_string;
  // Construct request
  const auto& c = order.contract();
  auto order_request = buildRequest(
      "order/LimitOrder.action",
      {
          {"limitOrderAssetToMarket", std::to_string(c.asset_to_market_id())},
          {"orderType", _limit_order_type(order.side())},
          {"expirationDate", _expiration_date(order)},
          {"price", ""},
          {"limitOrderQuantity", std::to_string(order.quantity())},
          {"placeLimitOder", "Place Limit Order"},
          {"market", std::to_string(c.market().value())}
      });
  return order_request;
}

const ClientResponse Session::cancel_order(const Order& order) {
  // Construct request
  auto cancel_order_request = buildRequest("TraderActivity.action");
  // TODO(rheineke): Differentiate order types
  // Set cookie
  cancel_order_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(cancel_order_request);
  return response;
}

int snprintf_session(char* const str, const Session& s) {
  static const auto fmt("{\"name\":\"session\", \"username\":\"%s\", "
                        "\"password\":\"%s\", \"cookie\":\"%s\"}");

  return snprintf(str, 160, fmt, s.username().c_str(), s.password().c_str(),
                  s.cookie().c_str());
}

}  // namespace iem
