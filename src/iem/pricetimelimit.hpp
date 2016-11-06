// Copyright 2015 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_PRICETIMELIMIT_HPP_
#define CPPIEM_SRC_IEM_PRICETIMELIMIT_HPP_

#include "boost/date_time/posix_time/posix_time.hpp"

#include "iem/price.hpp"

namespace iem {

class PriceTimeLimit final {
 public:
  PriceTimeLimit(const Price& price,
                 const boost::posix_time::ptime& expiration);
  PriceTimeLimit();

  inline const Price price() const { return price_; }

  const boost::posix_time::ptime expiration() const;

  bool ioc() const noexcept;

 private:
  bool is_ioc(const Price& price, const boost::posix_time::ptime& expiration);

  Price price_;
  boost::posix_time::ptime expiration_;
  bool ioc_;
};

boost::posix_time::ptime read_expiration_ptime(const std::string& market_name,
                                               const std::string& asset_name);

// Returns IEM formatted expiration date string. Provided as top level function
// so that all expirations can be printed consistently.
const std::string to_string(const boost::posix_time::ptime& expiration_ptime);

std::ostream& operator<<(std::ostream& os, const PriceTimeLimit& ptl);

} // namespace iem

#endif
