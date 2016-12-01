// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>

#ifndef CPP_IEM_MESSAGE_HPP
#define CPP_IEM_MESSAGE_HPP

#include "boost/date_time/posix_time/posix_time.hpp"

#include "iem/order.hpp"

namespace iem {

enum class MessageType { ORDER, ORDER_RESOLUTION };

const std::string to_string(const MessageType& msg_type);

MessageType message_type_from_string(const std::string& msg_type_str);

enum class Action {
  ASK_ENTERED,
  BID_ENTERED,
  FIXED_BUNDLE_PURCHASE_EXECUTED,
  FIXED_BUNDLE_SALE_EXECUTED,
  ASK_ORDER_WITHDRAWN,
  BID_ORDER_WITHDRAWN,
};

const std::string to_string(const Action& action);

Action action_from_string(const std::string& action_str);

class TraderMessage final {
 public:
  TraderMessage(const boost::posix_time::ptime& date,
                const MessageType& type,
                const std::string& contract,
                const Action& action,
                const Quantity quantity,
                const Price& unit_price,
                const boost::posix_time::ptime& expiration_date);

  inline const boost::posix_time::ptime date() const { return date_; }
  inline const MessageType type() const { return type_; }
  inline const std::string contract() const { return contract_; }
  inline const Action action() const { return action_; }
  inline const Quantity quantity() const { return quantity_; }
  inline const Price unit_price() const { return unit_price_; }
  inline const boost::posix_time::ptime expiration_date() const {
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

boost::posix_time::ptime date_from_string(const std::string& str);

boost::posix_time::ptime expiration_date_from_string(const std::string& str);

class HoldingMessage final {
 public:
  HoldingMessage(const boost::posix_time::ptime& date,
                 const Market& market,
                 const Action& action,
                 const Quantity quantity,
                 const Price& price);

  inline const boost::posix_time::ptime date() const { return date_; }
  inline const Market market() const { return market_; }
  inline const Action action() const { return action_; }
  inline const Quantity quantity() const { return quantity_; }
  inline const Price price() const { return price_; }

 private:
  boost::posix_time::ptime date_;
  Market market_;
  Action action_;
  Quantity quantity_;
  Price price_;
};

}  // namespace iem

#endif //CPP_IEM_MESSAGE_HPP
