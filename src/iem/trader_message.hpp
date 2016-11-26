// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#ifndef CPP_IEM_TRADER_MESSAGE_HPP
#define CPP_IEM_TRADER_MESSAGE_HPP

#include "boost/date_time/posix_time/posix_time.hpp"

#include "iem/order.hpp"

namespace iem {

enum class MessageType { ORDER, RESOLUTION };

inline const std::string to_string(const MessageType& msg_type) {
  switch (msg_type) {
    case MessageType::ORDER:
      return "order";
    case MessageType::RESOLUTION:
      return "resolution";
  }
}

enum class Action {
  ASK_ENTERED,
  BID_ENTERED,
  FIXED_BUNDLE_PURCHASE_EXECUTED
};

inline const std::string to_string(const Action& action) {
  switch (action) {
    case Action::ASK_ENTERED:
      return "ask entered";
    case Action::BID_ENTERED:
      return "bid entered";
    case Action::FIXED_BUNDLE_PURCHASE_EXECUTED:
      return "fixed bundle purchase executed";
  }
}

class TraderMessage final {
 public:
  TraderMessage(const boost::posix_time::ptime& date,
                const MessageType& type,
                const Contract& contract,
                const Action& action,
                const Quantity quantity,
                const Price& unit_price,
                const boost::posix_time::ptime& expiration_date);

  inline const boost::posix_time::ptime date() const { return date_; }
  inline const MessageType type() const { return type_; }
  inline const Contract contract() const { return contract_; }
  inline const Action action() const { return action_; }
  inline const Quantity quantity() const { return quantity_; }
  inline const Price unit_price() const { return unit_price_; }
  inline const boost::posix_time::ptime expiration_date() const {
    return expiration_date_;
  }
 private:
  boost::posix_time::ptime date_;
  MessageType type_;
  Contract contract_;
  Action action_;
  Quantity quantity_;
  Price unit_price_;
  boost::posix_time::ptime expiration_date_;
};

std::ostream& operator<<(std::ostream& os, const TraderMessage& msg);

boost::posix_time::ptime date_from_string(const std::string& str);

}  // namespace iem

#endif //CPP_IEM_TRADER_MESSAGE_HPP
