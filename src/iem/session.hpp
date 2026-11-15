// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_SESSION_HPP_
#define CPPIEM_IEM_SESSION_HPP_

#include <memory>
#include <stdexcept>
#include <string>

#include "iem/http_client.hpp"
#include "iem/message.hpp"
#include "iem/orderbook.hpp"

namespace iem {

// The IEM session expired (requests redirect to the login page): log in
// again. Nothing was done.
class SessionExpired : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

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
  // client defaults to the live IEM site; tests pass a fake
  Session(const std::string& username, const std::string& password,
          std::unique_ptr<HttpClient> client = make_iem_http_client());
  // Session is unique and therefore cannot be copied, but moves are acceptable
  //  Session(const Session&) = delete;
  //  Session operator=(const Session&) = delete;

  // Don't throw exception on destruction!
  //  ~Session() {
  //    if (this->cookie_ != "") {
  //      this->logout();
  //    }
  //  }

  inline const std::string& username() const { return username_; }
  inline const std::string& password() const { return password_; }
  inline const std::string& cookie() const { return cookie_; }

  // Request a session cookie. This must be called before any other session
  // calls. If a valid session exists, then this method will request a session
  // that invalidates the existing one
  const HttpResponse authenticate();

  // Requests that the current session cookie be invalidated
  const HttpResponse logout();

  // Requests orderbook information for the specified market
  const std::vector<OrderBook> orderbook(const Market& market);

  // Requests position change messages for the specific contract
  const std::vector<HoldingMessage> holdings(const Contract& contract);

  // Requests outstanding orders for the specified contract and side
  const SingleOrders outstanding_orders(const Contract& contract,
                                        const Side& side);

  // Request to place a single market or limit order
  const HttpResponse place_order(const Single& order);

  // Request to place a fixed or market price bundle order
  const HttpResponse place_order(const Bundle& order);

  // Request to cancel a single limit order. Order must have its id set.
  const HttpResponse cancel_order(const Single& order);

  // Returns a list of new trader messages for the specified market. New is
  // defined as all messages since remove_messages(...) was last called.
  const std::vector<TraderMessage> messages(const Market& market);

  // Clears the list of new trader messages for the specified market.
  const HttpResponse remove_messages(const Market& market);

  // Returns a list of trader messages for the specified market.
  const std::vector<TraderMessage> portfolio(const Market& market);

 private:
  std::string login(const bool force_login);
  HttpRequest market_client_request(const Market& market,
                                    const std::string& query);

  const std::string username_;
  const std::string password_;
  // TODO(rheineke): Cookie and client should be wrapped (each?) in a mutex
  std::string cookie_;
  std::unique_ptr<HttpClient> client_;
};

// snprintf function for a session object
// Writes the username and whether the session is logged in, as JSON, into
// str (size bytes). The password and cookie are never written.
int snprintf_session(char* const str, const std::size_t size, const Session& s);

// Logs a session out when it goes out of scope, error or not. A session left
// open makes the next login force its way in, which ends any other live
// session. Never throws: a failed logout is printed to stderr.
class LogoutOnExit {
 public:
  explicit LogoutOnExit(Session* session) noexcept : session_(session) {}
  ~LogoutOnExit();
  LogoutOnExit(const LogoutOnExit&) = delete;
  LogoutOnExit& operator=(const LogoutOnExit&) = delete;
  LogoutOnExit(LogoutOnExit&&) = delete;
  LogoutOnExit& operator=(LogoutOnExit&&) = delete;

 private:
  Session* const session_;
};

// Private functions exposed for unit testing
const std::vector<OrderBook> _read_orderbooks_html(const std::string& body,
                                                   const Market& market);

std::string _location_path(const std::string& location);
Price _parse_price(const std::string& px_str);

position_t _parse_quantity(const std::string& qty_str);

Quantity _parse_order_quantity(const std::string& qty_str);

const std::string _table_html_string(const std::string& body);

const HttpRequest _single_order_request(const Single& order);

}  // namespace iem

#endif  // CPPIEM_IEM_SESSION_HPP_
