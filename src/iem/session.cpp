// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <utility>

#include "boost/property_tree/xml_parser.hpp"

namespace iem {

using ptree = boost::property_tree::ptree;
using StringPair = std::pair<std::string, std::string>;
using StringPairVector = std::vector<StringPair>;

constexpr auto _VAH("viewAssetHoldings");

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

const ClientResponse Session::authenticate() {
  auto location = login(false);
  if (location == "") {  // If location is empty string, then login failed
    std::cout << "Login failed; attempting forcing login..." << std::endl;
    location = login(true);  // Force login
  }

  // Market select request
  auto market_select_request = buildRequest(location);
  // Set cookie
  market_select_request << boost::network::header("Set-Cookie", cookie());
  // GET request
  return client_.get(market_select_request);
}

std::string Session::login(const bool force_login) {
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
  for (const auto& h : login_response.headers()) {
    if (h.first == "Set-Cookie") {
      cookie_ = h.second;
    } else if (h.first == "Location") {
      location = h.second;
    }
  }
  return location;
}

const ClientResponse Session::logout() {
  // Construct request
  auto logout_request = buildRequest("/iem/trader/TraderLogin.action?logout=");
  // Set cookie
  logout_request << boost::network::header("Cookie", this->cookie());
  // GET request
  const auto response = client_.get(logout_request);
  // Set cookie_ to empty string if successful
  if (status(response) == 200) {
    this->cookie_ = "";
  }
  return response;
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

const OrderBook _read_orderbook_html(ptree::const_assoc_iterator tr_it,
                                     const Market& market) {
  // Orderbook values
  std::string contract_name;
  Price best_bid_px;
  bool best_bid_priority;
  Price best_ask_px;
  bool best_ask_priority;
  Price last_trade_px;
  position_t position;
  unsigned int num_open_bid_orders;
  unsigned int num_open_ask_orders;

  auto td_its = tr_it->second.equal_range("td");
  constexpr auto c("contract");
  for (auto it = td_its.first; it != td_its.second; it++) {
    // XML attribute called class
    const auto& klass = it->second.get<std::string>("<xmlattr>.class", c);
    if (klass == c) {  // Contract name
      contract_name = it->second.data();
      boost::trim(contract_name);
    } else if (klass == "change-cell bestBidPrice") {  // Best bid price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      best_bid_px = _parse_price(data);
      best_bid_priority = data.find("*") != std::string::npos;
    } else if (klass == "change-cell bestAskPrice") {  // Best ask price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      best_ask_px = _parse_price(data);
      best_ask_priority = data.find("*") != std::string::npos;
    } else if (klass == "change-cell lastPrice") {  // Last trade price
      auto data = it->second.get_child("p").data();
      boost::trim(data);
      last_trade_px = _parse_price(data);
    } else if (klass == "change-cell quantity") {  // Position
      auto data = it->second.get<std::string>("<xmlattr>.value");
      position = _parse_quantity(data);
    } else if (klass == "yourBidsCell") {  // Number of open bid orders
      num_open_bid_orders = num_open_orders(it);
    } else if (klass == "yourAsksCell") {  // Number of open ask orders
      num_open_ask_orders = num_open_orders(it);
    }
  }

  return OrderBook(
      Contract(market.name(), contract_name),
      best_bid_px,
      best_bid_priority,
      best_ask_px,
      best_ask_priority,
      last_trade_px,
      num_open_bid_orders,
      num_open_ask_orders,
      position);
}

const std::string _table_html_string(const std::string& body) {
  // In order to increase likelihood of parsing valid XML, find substring that
  // covers all html tables
  const size_t pos = body.find("<table");
  const size_t len = body.rfind("table") - pos + 6;  // 6 is length of "<table"
  return body.substr(pos, len);
}

ptree _tbody_ptree(const std::string& body) {
  const auto tables_str = _table_html_string(body);
  // Boost ptree works on streams only
  std::istringstream is(tables_str);
  // ptree
  ptree pt;
  read_xml(is, pt);
  // XML nodes (terminology?) of interest
  return pt.get_child("table.tbody");
}

const std::vector<OrderBook> _read_orderbooks_html(const std::string &body,
                                                   const Market& market) {
  auto tbody = _tbody_ptree(body);
  const auto tr_its = tbody.equal_range("tr");

  std::vector<OrderBook> obs;
  for (auto it = tr_its.first; it != tr_its.second; it++) {
    obs.push_back(_read_orderbook_html(it, market));
  }
  return obs;
}

const std::vector<OrderBook> Session::orderbook(const Market& market) {
  // Construct request
  auto market_orderbook_request = buildRequest(
      "/iem/trader/MarketTrader.action",
      {
          {"market", std::to_string(market.value())}
      });
  // Set cookie
  market_orderbook_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(market_orderbook_request);
  return _read_orderbooks_html(body(response), market);
}

const HoldingMessage _read_message_html(ptree::const_assoc_iterator tr_it) {
  // Trader message values
  boost::posix_time::ptime date;
  std::string market_label;
  Action action;
  Quantity quantity = 0;
  Price price;

  auto td_its = tr_it->second.equal_range("td");
  int i = 0;
  for (auto it = td_its.first; it != td_its.second; it++) {
    auto data_str = it->second.data();
    boost::trim(data_str);
    if (i == 0) {  // date
      date = date_from_string(data_str);
    } else if (i == 1) {  // market
      market_label = data_str;
    } else if (i == 2) {  // action
      data_str = it->second.get_child("a").data();
      action = action_from_string(data_str);
    } else if (i == 4) {  // quantity
      quantity = _parse_quantity(data_str);
    } else if (i == 5) {  // price
      price = _parse_price(data_str);
    }

    i++;
  }

  return HoldingMessage(
      date,
      market_label,
      action,
      quantity,
      price);
}

const std::vector<HoldingMessage> _read_messages_html(const std::string& body) {
  auto tbody = _tbody_ptree(body);
  const auto tr_its = tbody.equal_range("tr");

  std::vector<HoldingMessage> msgs;
  for (auto it = tr_its.first; it != tr_its.second; it++) {
    msgs.push_back(_read_message_html(it));
  }
  return msgs;
}

const std::vector<HoldingMessage> Session::holdings(const Contract& contract) {
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
  return _read_messages_html(body(response));
}

const std::string _activity_type(Side side) {
  return (side == Side::BUY) ? "bid" : "ask";
}

const Side _side_from_string(const std::string& side_str) {
  if (side_str == "bid") {
    return Side::BUY;
  } else if (side_str == "ask") {
    return Side::SELL;
  } else {
    throw std::invalid_argument("Unknown side: " + side_str);
  }
}

OrderId _order_id(const std::string& href_str, const Side& side) {
  const std::string& key = (side == Side::BUY) ? "bidOrder=" : "askOrder=";
  const size_t pos = href_str.find(key) + key.size();
  const size_t len = href_str.find("&", pos) - pos;
  return std::stoi(href_str.substr(pos, len));
}

const Single _read_order_html(const ptree::const_assoc_iterator tr_it) {
  // Outstanding order values
  boost::posix_time::ptime order_date;
  std::string market_name;
  std::string contract_name;
  std::string order_type;
  Quantity quantity = 0;
  Price price;
  boost::posix_time::ptime expiration;
  OrderId order_id;

  auto td_its = tr_it->second.equal_range("td");
  int i = 0;
  for (auto it = td_its.first; it != td_its.second; it++) {
    auto data_str = it->second.data();
    boost::trim(data_str);
    if (i == 0) {  // order date
      order_date = date_from_string(data_str);
    } else if (i == 1) {  // market
      market_name = data_str;
    } else if (i == 2) {  // contract name
      contract_name = data_str;
    } else if (i == 3) {  // order type
      order_type = data_str;
    } else if (i == 4) {  // quantity
      quantity = _parse_quantity(data_str);
    } else if (i == 5) {  // price
      price = _parse_price(data_str);
    } else if (i == 6) {  // expiration date
      expiration = date_from_string(data_str);
    } else if (i == 7) {  // order id
      order_id = _order_id(it->second.get<std::string>("a.<xmlattr>.href"),
                           _side_from_string(order_type));
    }

    i++;
  }

  Single so(
      Contract(market_name, contract_name),
      _side_from_string(order_type),
      quantity,
      PriceTimeLimit(price, expiration)
  );
  so.set_id(order_id);
  return so;
}

const std::vector<Single> _read_orders_html(const std::string& body) {
  auto tbody = _tbody_ptree(body);
  const auto tr_its = tbody.equal_range("tr");

  std::vector<Single> os;
  for (auto it = tr_its.first; it != tr_its.second; it++) {
    os.push_back(_read_order_html(it));
  }
  return os;
}

const SingleOrders Session::outstanding_orders(const Contract& contract,
                                               const Side& side) {
  // Construct request
  auto outstanding_order_request = buildRequest(
      "/iem/trader/TraderActivity.action",
      {
          {"market", std::to_string(contract.market().value())},
          {"asset", std::to_string(contract.asset_id())},
          {"activityType", _activity_type(side)},
      });
  // Set cookie
  outstanding_order_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(outstanding_order_request);
  return _read_orders_html(body(response));
}

const ClientRequest limit_order_request(const Single& order) {
  using boost::gregorian::to_iso_extended_string;
  // Construct request
  const auto& c = order.contract();
  auto order_request = buildRequest(
      "/iem/trader/order/LimitOrder.action",
      {
          {"limitOrderAssetToMarket", std::to_string(c.asset_to_market_id())},
          {"orderType", _activity_type(order.side())},
          {"expirationDate", to_string(order.price_time_limit().expiration())},
          {"price", to_string(order.price_time_limit().price())},
          {"limitOrderQuantity", std::to_string(order.quantity())},
          {"placeLimitOrder", "Place Limit Order"},
          {"market", std::to_string(c.market().value())},
          {"_sourcePage", ""},  // Required?
      });
  return order_request;
}

const std::string _market_order_type(Side side) {
  return (side == Side::BUY) ? "buy" : "sell";
}

const ClientRequest market_order_request(const Single& order) {
  // Construct request
  const auto& c = order.contract();
  auto order_request = buildRequest(
      "/iem/trader/order/MarketOrder.action",
      {
          {"limitOrderAssetToMarket", std::to_string(c.asset_to_market_id())},
          {"orderType", _market_order_type(order.side())},
          {"marketOrderQuantity", std::to_string(order.quantity())},
          {"placeMarketOrder", "Place Market Order"},
          {"market", std::to_string(c.market().value())}
      });
  return order_request;
}

const ClientRequest _single_order_request(const Single &order) {
  if (order.price_time_limit().ioc()) {
    return market_order_request(order);
  } else {
    return limit_order_request(order);
  }
}

const ClientResponse Session::place_order(const Single& order) {
  // Construct request
  ClientRequest order_request = _single_order_request(order);
  // Set cookie
  order_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(order_request);
  return response;
}

const std::string _bundle_order_type(const Side& side, const Counterparty& cp) {
  const auto action_str = _market_order_type(side);
  const auto cp_str = (cp == Counterparty::EXCHANGE) ? "Fixed" : "Market";
  return action_str + "At" + cp_str;
}

const ClientRequest _bundle_order_request(const Bundle& order) {
  // Construct request
  const auto& cb = order.contract_bundle();
  auto order_request = buildRequest(
      "/iem/trader/order/BundleOrder.action",
      {
          {"bundle", std::to_string(cb.bundle_id())},
          {"orderType", _bundle_order_type(order.side(), order.counterparty())},
          {"bundleOrderQuantity", std::to_string(order.quantity())},
          {"placeBundleOrder", "Place Bundle Order"},
          {"market", std::to_string(cb.market().value())}
      });
  return order_request;
}

const ClientResponse Session::place_order(const Bundle& order) {
  // Construct request
  ClientRequest order_request = _bundle_order_request(order);
  // Set cookie
  order_request << boost::network::header("Cookie", cookie());
  // POST request
  const auto& response = client_.post(order_request);
  return response;
}

const std::string _action(const Side& side) {
  return (side == Side::BUY) ? "cancelBidOrder" : "cancelAskOrder";
}

const ClientResponse Session::cancel_order(const Single& order) {
  // Construct request
  if (valid_id(order)) {
    // Return something useful
    return ClientResponse();
  }

  const auto oid = (order.side() == Side::BUY) ? "bidOrder": "askOrder";
  const auto cxl = url_encode(
      {
          {_action(order.side()), ""},
          {"market", std::to_string(order.contract().market().value())},
          {oid, std::to_string(order.id())},
          {"asset", std::to_string(order.contract().asset_id())},
          {"activityType", _activity_type(order.side())}
      });
  auto cxl_request = buildRequest("/iem/trader/TraderActivity.action?" + cxl);
  // Set cookie
  cxl_request << boost::network::header("Cookie", this->cookie());
  // POST request
  const auto& response = client_.get(cxl_request);
  return response;
}

const TraderMessage _read_message_html(const std::string& market_name,
                                       ptree::const_assoc_iterator tr_it) {
  // Trader message values
  boost::posix_time::ptime date;
  MessageType msg_type;
  std::string contract_name;
  Action action;
  Quantity quantity = 0;
  Price price;
  boost::posix_time::ptime expiration_date;

  auto td_its = tr_it->second.equal_range("td");
  int i = 0;
  for (auto it = td_its.first; it != td_its.second; it++) {
    auto data_str = it->second.data();
    boost::trim(data_str);
    if (i == 0) {  // date
      date = date_from_string(data_str);
    } else if (i == 1) {  // msg_type
      msg_type = message_type_from_string(data_str);
    } else if (i == 2) {  // contract_name
      contract_name = data_str;
    } else if (i == 3) {  // action
      action = action_from_string(data_str);
    } else if (i == 4) {  // quantity
      quantity = std::stoi(data_str);
    } else if (i == 5) {  // price
      price = _parse_price(data_str);
    } else if (i == 6) {  // expiration_date
      expiration_date = expiration_date_from_string(data_str);
    }

    i++;
  }

  return TraderMessage(
      date,
      msg_type,
      contract_name,
      action,
      quantity,
      price,
      expiration_date);
}

const std::vector<TraderMessage> _read_messages_html(const Market& market,
                                                     const std::string& body) {
  auto tbody = _tbody_ptree(body);
  const auto tr_its = tbody.equal_range("tr");

  std::vector<TraderMessage> msgs;
  for (auto it = tr_its.first; it != tr_its.second; it++) {
    msgs.push_back(_read_message_html(market.name(), it));
  }
  return msgs;
}

const std::vector<TraderMessage> Session::messages(const Market& market) {
  // Construct request
  const auto request = market_client_request(market, "home");
  // GET request
  const auto& response = client_.get(request);
  return _read_messages_html(market, body(response));
}

const ClientResponse Session::remove_messages(const Market& market) {
  // Construct request
  const auto request = market_client_request(market, "removeMessages");
  // GET request
  const auto& response = client_.get(request);
  return response;
}

const std::vector<TraderMessage> Session::portfolio(const Market& market) {
  // Construct request
  const auto request = market_client_request(market, "viewPortfolio");
  // GET request
  const auto& response = client_.get(request);
  return _read_messages_html(market, body(response));
}

ClientRequest Session::market_client_request(const Market& market,
                                             const std::string& query) {
  // Construct request
  const auto msg = url_encode(
      {
          {query, ""},
          {"market", std::to_string(market.value())}
      });
  auto request = buildRequest("/iem/trader/TraderMessages.action?" + msg);
  request << boost::network::header("Cookie", this->cookie());
  return request;
}

int snprintf_session(char* const str, const Session& s) {
  static const auto fmt("{\"name\":\"session\", \"username\":\"%s\", "
                        "\"password\":\"%s\", \"cookie\":\"%s\"}");
  const auto buf_size = std::min<std::size_t>(sizeof(str), 160);
  return snprintf(str, buf_size, fmt, s.username().c_str(),
                  s.password().c_str(), s.cookie().c_str());
}

// TODO(rheineke): Write function
//UniqueSession make_unique_session(const std::string& username,
//                                  const std::string& password) {
//  return std::unique_ptr(new Session(username, password),
//                         decltype(&Session::logout));
//}

}  // namespace iem
