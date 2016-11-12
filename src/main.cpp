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
  const auto obs = session.market_orderbook(51);
  for (const auto& ob : obs) {
    std::cout << ob << std::endl;
  }

  // Send a test order
  const iem::Contract c("FedPolicyB", "FRsame1216");
  const iem::Price px(1);
  const iem::PriceTimeLimit ptl(px, boost::posix_time::not_a_date_time);
  const iem::Single o(c, iem::Side::BUY, 1, ptl);
  const auto response = session.place_order(o);
  // std::cout << body(response) << std::endl;

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
