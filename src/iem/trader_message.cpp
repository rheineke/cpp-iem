// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#include "trader_message.hpp"

namespace iem {

constexpr auto order_cstr("Order");
constexpr auto order_resolution_cstr("Order Resolution");

const std::string to_string(const MessageType& msg_type) {
  switch (msg_type) {
    case MessageType::ORDER:
      return order_cstr;
    case MessageType::ORDER_RESOLUTION:
      return order_resolution_cstr;
  }
}

MessageType message_type_from_string(const std::string& msg_type_str) {
  if (msg_type_str == order_cstr) {
    return MessageType::ORDER;
  } else if (msg_type_str == order_resolution_cstr) {
    return MessageType::ORDER_RESOLUTION;
  } else {
    throw std::invalid_argument("Unknown string: " + msg_type_str);
  }
}

constexpr auto ask_entered("Ask entered");
constexpr auto bid_entered("Bid entered");
constexpr auto fixed_bundle_purchase_executed("Fixed bundle purchase executed");
constexpr auto fixed_bundle_sale_executed("Fixed bundle sale executed");

inline const std::string to_string(const Action& action) {
  switch (action) {
    case Action::ASK_ENTERED:
      return ask_entered;
    case Action::BID_ENTERED:
      return bid_entered;
    case Action::FIXED_BUNDLE_PURCHASE_EXECUTED:
      return fixed_bundle_purchase_executed;
    case Action::FIXED_BUNDLE_SALE_EXECUTED:
      return fixed_bundle_sale_executed;
  }
}

Action action_from_string(const std::string& action_str) {
  if (action_str == ask_entered) {
    return Action::ASK_ENTERED;
  } else if (action_str == bid_entered) {
    return Action::BID_ENTERED;
  } else if (action_str == fixed_bundle_purchase_executed) {
    return Action::FIXED_BUNDLE_PURCHASE_EXECUTED;
  } else if (action_str == fixed_bundle_sale_executed) {
    return Action::FIXED_BUNDLE_SALE_EXECUTED;
  } else {
    throw std::invalid_argument("Unknown string: " + action_str);
  }
}

TraderMessage::TraderMessage(const boost::posix_time::ptime& date,
                             const MessageType& type,
                             const std::string& contract,
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
  std::istringstream iss(str);
  // Almost ISO extended
  constexpr auto fmt("%Y-%m-%d %H:%M:%S%F");
  // Locale object owns the facet instance so no memory leak here
  auto* p_dt_facet(new boost::posix_time::time_input_facet(fmt));
  iss.imbue(std::locale(iss.getloc(), p_dt_facet));

  boost::posix_time::ptime abs_time;
  iss >> abs_time;

  return abs_time;
}

boost::posix_time::ptime expiration_date_from_string(const std::string& str) {
  if (str == "") {  // No expiration
    return boost::posix_time::not_a_date_time;
  }

  std::istringstream iss(str);
  constexpr auto fmt("%b %d, %Y %H:%M:%S");
  // Locale object owns the facet instance so no memory leak here
  auto* p_dt_facet(new boost::posix_time::time_input_facet(fmt));

  iss.imbue(std::locale(iss.getloc(), p_dt_facet));

  boost::posix_time::ptime expiry_dt;
  iss >> expiry_dt;

  return expiry_dt;
}

}  // namespace iem