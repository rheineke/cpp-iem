// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <fstream>

#include "iem/logger.hpp"
#include "iem/orderbook.hpp"
#include "iem/session.hpp"

using LoginCreds = std::pair<std::string, std::string>;

LoginCreds read_login_credentials(char const* filename = "conf/login.json") {
  // Open file in binary mode
  Json::Value root;
  std::ifstream config_doc(filename, std::ifstream::binary);
  config_doc >> root;
  // Read username and password
  const auto username = root.get("username", "").asString();
  const auto password = root.get("password", "").asString();
  // Return as pair
  return {username, password};
}

int _main(int argc, char* argv[]) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  const auto login = (argc == 2) ?
                     read_login_credentials(argv[1]) : read_login_credentials();

  iem::Session session(login.first, login.second);
  std::cout << "Logging in..." << std::endl;
  session.authenticate();
  // Print session after authentication
  iem::Logger logger;
  iem::snprintf_session(logger.getBuffer(), session);
  logger.log();

  // FedPolicyB market
  const iem::Market mkt("FedPolicyB");
  const auto obs = session.orderbook(mkt);
  for (const auto& ob : obs) {
    std::cout << ob << std::endl;
  }

  // Send a test bid order
  const iem::Contract c(mkt.name(), "FRsame1216");
//  const iem::Price bid_px(0);
//  const iem::PriceTimeLimit bid_ptl(bid_px, boost::posix_time::not_a_date_time);
//  const iem::Single bid_o(c, iem::Side::BUY, 1, bid_ptl);
//  const auto response = session.place_order(bid_o);
//  std::cout << body(response) << std::endl;

  // Send a test ask order
//  const iem::Price ask_px(1000);
//  const iem::PriceTimeLimit ask_ptl(ask_px, boost::posix_time::not_a_date_time);
//  const iem::Single ask_o(c, iem::Side::SELL, 1, ask_ptl);
//  const auto ask_response = session.place_order(ask_o);

  // For each side
  std::cout << "Requesting outstanding orders" << std::endl;
  const std::vector<iem::Side> sides{iem::Side::BUY, iem::Side::SELL};
  for (const auto& side : sides) {
    // Request side outstanding orders
    const auto oos = session.outstanding_orders(c, side);
    for (const auto& oo : oos) {
      std::cout << oo << std::endl;
    }

    // Cancel most recent bid order
//    const auto cxl_o = oos[oos.size() - 1];
//    std::cout << cxl_o << std::endl;
//    const auto cxl_response = session.cancel_order(cxl_o);
  }

  // Send a test bundle order
  const iem::ContractBundle cb(mkt.name(),
                               iem::MonthYear(boost::gregorian::Dec, 16));
  const iem::Bundle b(cb, iem::Side::BUY, 1, iem::Counterparty::EXCHANGE);
  const auto bundle_response = session.place_order(b);
  std::cout << body(bundle_response) << std::endl;

  // Request trade messages
  const auto msgs = session.messages(mkt);
  for (const auto& msg : msgs) {
    std::cout << msg << std::endl;
  }

  // Request holdings
  const auto holdings_response = session.holdings(c);

  // Request portfolio
  std::cout << "Requesting portfolio messages" << std::endl;
  const auto portfolio_trader_messages = session.portfolio(mkt);
  for (const auto& msg : msgs) {
    std::cout << msg << std::endl;
  }


  // TODO(rheineke): Generate a heartbeat thread with a mutex around cookie
  // Make simple request every n minutes and call authenticate(...) with
  // forceLogin=True if request fails. Update mutexed cookie if necessary
  // std::thread hb(&heartbeat, session);
  // hb.detach();

  // Logout
  std::cout << "Logging out..." << std::endl;
  session.logout();

  auto return_value = EXIT_SUCCESS;
  // std::cout << "Starting trade" << std::endl;
  return return_value;
}

void heartbeat(const iem::Session session) {
  // Assume authenticated
  while (true) {
    // session.heartbeat();

    std::this_thread::sleep_for(std::chrono::minutes(5));
  }
}

int main(int argc, char* argv[]) {
  return _main(argc, argv);

//  iem::Market mkt("FedPolicyB");
//  std::cout << mkt.value() << std::endl;
//  return 0;
}
