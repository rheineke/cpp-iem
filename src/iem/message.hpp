// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#ifndef CPPIEM_IEM_MESSAGE_HPP
#define CPPIEM_IEM_MESSAGE_HPP

#include "boost/date_time/posix_time/posix_time.hpp"

#include "iem/order.hpp"

namespace iem {

enum class MessageType { ORDER, ORDER_RESOLUTION };

const std::string to_string(const MessageType& msg_type);

MessageType message_type_from_string(const std::string& msg_type_str);

// Enumeration of all actions available on IEM as reported in exchanges messages
enum class Action {
  ASK_ENTERED,
  BID_ENTERED,
  SELL,
  BUY,
  FIXED_BUNDLE_PURCHASE_EXECUTED,
  FIXED_BUNDLE_SALE_EXECUTED,
  MARKET_BUNDLE_PURCHASE_EXECUTED,
  MARKET_BUNDLE_SALE_EXECUTED,
  ASK_ORDER_WITHDRAWN,
  BID_ORDER_WITHDRAWN,
};

// Returns a string representation of action
const std::string to_string(const Action& action);

// Returns an action enumeration of a valid string. Unknown strings will cause
// an exception to be thrown
Action action_from_string(const std::string& action_str);

// Message provided by IEM. Actions covered by this message include order
// placements, cancellations, and fulfillments
class TraderMessage final {
 public:
  TraderMessage(const boost::posix_time::ptime& date,
                const MessageType& type,
                const std::string& contract,
                const Action& action,
                const Quantity quantity,
                const Price& unit_price,
                const boost::posix_time::ptime& expiration_date);

  inline const boost::posix_time::ptime date() const noexcept { return date_; }
  inline const MessageType type() const noexcept { return type_; }
  inline const std::string contract() const noexcept { return contract_; }
  inline const Action action() const noexcept { return action_; }
  inline const Quantity quantity() const noexcept { return quantity_; }
  inline const Price unit_price() const noexcept { return unit_price_; }
  inline const boost::posix_time::ptime expiration_date() const noexcept {
    return expiration_date_;
  }
 private:
  boost::posix_time::ptime date_;
  MessageType type_;
  std::string contract_;
  Action action_;
  Quantity quantity_;
  Price unit_price_;
  boost::posix_time::ptime expiration_date_;
};

std::ostream& operator<<(std::ostream& os, const TraderMessage& msg);

// Returns a ptime object using the IEM format for the date/time of an event
// that results in a message
boost::posix_time::ptime date_from_string(const std::string& str);

// Returns a ptime object using the IEM for the date/time that an order will
// expire. not_a_date_time is equivalent to no expiration, i.e. Good 'Til
// Cancelled (GTC)
boost::posix_time::ptime expiration_date_from_string(const std::string& str);

// Message provided by IEM. Actions cover only position changing events.
class HoldingMessage final {
 public:
  HoldingMessage(const boost::posix_time::ptime& date,
                 const std::string& market_label,
                 const Action& action,
                 const Quantity quantity,
                 const Price& price);

  inline const boost::posix_time::ptime date() const { return date_; }
  inline const std::string market_label() const { return market_label_; }
  inline const Action action() const { return action_; }
  inline const Quantity quantity() const { return quantity_; }
  inline const Price price() const { return price_; }

 private:
  boost::posix_time::ptime date_;
  std::string market_label_;
  Action action_;
  Quantity quantity_;
  Price price_;
};

}  // namespace iem

#endif  // CPPIEM_IEM_MESSAGE_HPP
