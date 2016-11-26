// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "trader_message.hpp"

namespace iem {

TraderMessage::TraderMessage(const boost::posix_time::ptime& date,
                             const MessageType& type,
                             const Contract& contract,
                             const Action& action,
                             const Quantity quantity,
                             const Price& unit_price,
                             const boost::posix_time::ptime& expiration_date):
    date_(date),
    type_(type),
    contract_(contract),
    action_(action),
    quantity_(quantity),
    unit_price_(unit_price),
    expiration_date_(expiration_date) {

}

std::ostream& operator<<(std::ostream& os, const TraderMessage& msg) {
  return os << "{\"name\": \"traderMessage\""
            << ", \"date\": " << msg.date()
            << ", \"type\": " << to_string(msg.type())
            << ", \"contract\": " << msg.contract()
            << ", \"action\": " << to_string(msg.action())
            << ", \"quantity\": " << msg.quantity()
            << ", \"price\": " << msg.unit_price()
            << ", \"expiration_date\": " << msg.expiration_date()
            << '}';
}

boost::posix_time::ptime date_from_string(const std::string& str) {
  auto p_tif = new boost::posix_time::time_input_facet;
  p_tif->format("%Y-%m-%d %H:%M:%S%F");

  std::istringstream iss(str);
  iss.imbue(std::locale(iss.getloc(), p_tif));

  boost::posix_time::ptime abs_time;
  iss >> abs_time;

  return abs_time;
}

}  // namespace iem