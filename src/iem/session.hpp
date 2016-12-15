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

class Session {
 public:
  Session(const std::string& username, const std::string& password);
  Session(const Session&) = delete;
  Session operator=(const Session&) = delete;

  // Don't throw exception on destruction!
//  ~Session() {
//    if (this->session() != "") {
//      this->logout();
//    }
//  }

  inline const std::string username() const { return username_; }
  inline const std::string password() const { return password_; }
  inline const std::string cookie() const { return cookie_; }

  const ClientResponse authenticate();

  const ClientResponse logout();

  const std::vector<OrderBook> orderbook(const Market& market);

  const std::vector<HoldingMessage> holdings(const Contract& contract);

  const SingleOrders outstanding_orders(const Contract& contract,
                                        const Side& side);

  const ClientResponse place_order(const Single& order);

  const ClientResponse place_order(const Bundle& order);

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

const std::vector<OrderBook> _read_orderbooks_html(const std::string &body);

int snprintf_session(char* const str, const Session& s);

using UniqueSession = std::unique_ptr<const Session,
                                      void (*)(const Session* const)>;

Price _parse_price(const std::string &px_str);

position_t _parse_quantity(const std::string &qty_str);

const ClientRequest _single_order_request(const Single &order);

}  // namespace iem

#endif  // CPPIEM_IEM_SESSION_HPP_
