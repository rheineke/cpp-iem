// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <fstream>

#include "iem/logger.hpp"
#include "iem/orderbook.hpp"
#include "iem/session.hpp"

using LoginCreds = std::pair<std::string, std::string>;

LoginCreds _read_login_credentials(char const* filename = "conf/login.json") {
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

LoginCreds read_login_credentials(int argc, char* argv[]) {
  return (argc == 2) ?
         _read_login_credentials(argv[1]) : _read_login_credentials();
}

int _main(int argc, char* argv[]) {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  const auto login = read_login_credentials(argc, argv);

  iem::Session session(login.first, login.second);
  std::cout << "Logging in...\n";
  session.authenticate();
  // Print session after authentication
  iem::Logger logger;
  iem::snprintf_session(logger.getBuffer(), session);
  logger.log();

  // FedPolicyB market
  const iem::Market mkt("FedPolicyB");
  const auto obs = session.orderbook(mkt);
  for (const auto& ob : obs) {
    std::cout << ob << '\n';
  }

  // Send a test bid order
  const iem::Contract c(mkt.name(), "FRsame1216");
//  const iem::Price bid_px(0);
//  const iem::PriceTimeLimit bid_ptl(bid_px, boost::posix_time::not_a_date_time);
//  const iem::Single bid_o(c, iem::Side::BUY, 1, bid_ptl);
//  const auto response = session.place_order(bid_o);
//  std::cout << body(response) << '\n';

  // Send a test ask order
//  const iem::Price ask_px(1000);
//  const iem::PriceTimeLimit ask_ptl(ask_px, boost::posix_time::not_a_date_time);
//  const iem::Single ask_o(c, iem::Side::SELL, 1, ask_ptl);
//  const auto ask_response = session.place_order(ask_o);

  // For each side
  std::cout << "Requesting outstanding orders\n";
  const std::vector<iem::Side> sides{iem::Side::BUY, iem::Side::SELL};
  for (const auto& side : sides) {
    // Request side outstanding orders
    const auto oos = session.outstanding_orders(c, side);
    for (const auto& oo : oos) {
      std::cout << oo << '\n';
    }

    // Cancel most recent bid order
//    const auto cxl_o = oos[oos.size() - 1];
//    std::cout << cxl_o << '\n';
//    const auto cxl_response = session.cancel_order(cxl_o);
  }

  // Send a test bundle order
  const iem::ContractBundle cb(mkt.name(),
                               iem::MonthYear(boost::gregorian::Dec, 16));
  const iem::Bundle b(cb, iem::Side::BUY, 1, iem::Counterparty::EXCHANGE);
  const auto bundle_response = session.place_order(b);
  std::cout << body(bundle_response) << '\n';

  // Request trade messages
  const auto msgs = session.messages(mkt);
  for (const auto& msg : msgs) {
    std::cout << msg << '\n';
  }

  // Request holdings
  const auto holdings_response = session.holdings(c);

  // Request portfolio
  std::cout << "Requesting portfolio messages\n";
  const auto portfolio_trader_messages = session.portfolio(mkt);
  for (const auto& msg : msgs) {
    std::cout << msg << '\n';
  }


  // TODO(rheineke): Generate a heartbeat thread with a mutex around cookie
  // Make simple request every n minutes and call authenticate(...) with
  // forceLogin=True if request fails. Update mutexed cookie if necessary
  // std::thread hb(&heartbeat, session);
  // hb.detach();

  // Logout
  std::cout << "Logging out...\n";
  session.logout();

  const auto return_value = EXIT_SUCCESS;
  // std::cout << "Starting trade" << '\n';
  return return_value;
}

int _main2(int argc, char* argv[]) {
  const auto login = read_login_credentials(argc, argv);

  iem::Session session(login.first, login.second);

  std::cout << "Logging in...\n";

  // session.authenticate();
  std::thread t(&iem::Session::authenticate, session);
  t.join();

  // Print session after authentication
  iem::Logger logger;
  iem::snprintf_session(logger.getBuffer(), session);
  logger.log();

  // Find all active markets and retrieve their markets concurrently
//  const iem::Market mkt("FedPolicyB");
//  std::thread t(&iem::Session::orderbook, session, mkt);
//
//  const std::vector<iem::Market> markets = {
//      iem::Market("FedPolicyB"),
//      iem::Market("Congress18"),
//      iem::Market("House18"),
//      iem::Market("Senate18"),
//  };
//  const auto obs = session.orderbook(mkt);
//  for (const auto& ob : obs) {
//    std::cout << ob << '\n';
//  }

  // Logout after logging in
  std::cout << session.cookie() << std::endl;
  std::cout << "Logging out..." << std::endl;
  const auto snd_logout_response = session.logout();
  std::cout << status(snd_logout_response) << '\n';
  std::cout << body(snd_logout_response) << '\n';

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
  return _main2(argc, argv);

//  iem::Market mkt("FedPolicyB");
//  std::cout << mkt.value() << std::endl;
//  return 0;
}
