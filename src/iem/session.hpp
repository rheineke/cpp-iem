// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_SESSION_HPP_
#define CPPIEM_IEM_SESSION_HPP_

#include <memory>
#include <string>

#include "boost/network/protocol/http/client.hpp"

#include "iem/message.hpp"
#include "iem/orderbook.hpp"

namespace iem {

using ClientRequest = boost::network::http::client::request;
using ClientResponse = boost::network::http::client::response;

// IEM HTTP session. Session requires a username and password at construction,
// and any account specific requests for IEM require a session object. Session
// must perform an authentication before any other requests can be made. Logout
// is not required, but encouraged.
// Example:
//   Session s("username", "password);
//   s.authenticate();
//   ...
//   s.logout();
class Session {
 public:
  // IEM Session requires username and password at construction
  Session(const std::string& username, const std::string& password);
  // Session is unique and therefore cannot be copied, but moves are acceptable
  Session(const Session&) = delete;
  Session operator=(const Session&) = delete;

  // Don't throw exception on destruction!
//  ~Session() {
//    if (this->cookie_ != "") {
//      this->logout();
//    }
//  }

  inline const std::string username() const { return username_; }
  inline const std::string password() const { return password_; }
  inline const std::string cookie() const { return cookie_; }

  // Request a session cookie. This must be called before any other session
  // calls. If a valid session exists, then this method will request a session
  // that invalidates the existing one
  const ClientResponse authenticate();

  // Requests that the current session cookie be invalidated
  const ClientResponse logout();

  // Requests orderbook information for the specified market
  const std::vector<OrderBook> orderbook(const Market& market);

  // Requests position change messages for the specific contract
  const std::vector<HoldingMessage> holdings(const Contract& contract);

  // Requests outstanding orders for the specified contract and side
  const SingleOrders outstanding_orders(const Contract& contract,
                                        const Side& side);

  // Request to place a single market or limit order
  const ClientResponse place_order(const Single& order);

  // Request to place a fixed or market price bundle order
  const ClientResponse place_order(const Bundle& order);

  // Request to cancel a single limit order. Order must have its id set.
  const ClientResponse cancel_order(const Single& order);

  // Returns a list of new trader messages for the specified market. New is
  // defined as all messages since remove_messages(...) was last called.
  const std::vector<TraderMessage> messages(const Market& market);

  // Clears the list of new trader messages for the specified market.
  const ClientResponse remove_messages(const Market& market);

  // Returns a list of trader messages for the specified market.
  const std::vector<TraderMessage> portfolio(const Market& market);

 private:
  std::string login(const bool force_login);
  ClientRequest market_client_request(const Market &market,
                                      const std::string &query);

  const std::string username_;
  const std::string password_;
  // TODO(rheineke): Cookie and client should be wrapped (each?) in a mutex
  std::string cookie_;
  boost::network::http::client client_;
};

// snprintf function for a session object
int snprintf_session(char* const str, const Session& s);

// Unique session with a custom deleter to automatically log out
using UniqueSession = std::unique_ptr<const Session,
                                      void (*)(const Session* const)>;

UniqueSession make_unique_session(const std::string& username,
                                  const std::string& password);

// Private functions exposed for unit testing
const std::vector<OrderBook> _read_orderbooks_html(const std::string &body,
                                                   const Market& market);

Price _parse_price(const std::string &px_str);

position_t _parse_quantity(const std::string &qty_str);

const ClientRequest _single_order_request(const Single &order);

}  // namespace iem

#endif  // CPPIEM_IEM_SESSION_HPP_
