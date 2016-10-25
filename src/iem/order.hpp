// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_SRC_IEM_ORDER_HPP_
#define CPPIEM_SRC_IEM_ORDER_HPP_

#include <string>
#include <vector>

#include "boost/pool/pool_alloc.hpp"

#include "iem/contract.hpp"
#include "iem/pricetimelimit.hpp"
#include "iem/side.hpp"

namespace iem {

using RequestCode = std::string;
using OrderId = uint_fast32_t;
using Quantity = uint_fast8_t;

enum class Counterparty { exchange, participant };

std::ostream& operator<<(std::ostream& os, const Counterparty& cp);

class Order {
 public:
  Side side() const noexcept;
  Quantity quantity() const noexcept;
  inline const PriceTimeLimit price_time_limit() const {
    return price_time_limit_;
  }
  Counterparty counterparty() const;

  friend std::ostream& operator<<(std::ostream& os, const Order& o);

 protected:
  Order(const Side& side,
        const Quantity quantity,
        const PriceTimeLimit& price_time_limit,
        const Counterparty& counterparty);

 private:
  Side side_;
  Quantity quantity_;
  PriceTimeLimit price_time_limit_;
  Counterparty counterparty_;

  virtual void print(std::ostream* const p_os) const = 0;
};

std::ostream& operator<<(std::ostream& os, const Order& o);

class Single final : public Order {
 public:
  const Contract contract() const;
  void print(std::ostream* const p_os) const final;

  Single(const Contract& contract,
         const Side& side,
         const Quantity quantity,
         const PriceTimeLimit& price_time_limit);
 private:
  Contract contract_;
};

class Bundle final : public Order {
 public:
  Bundle(const ContractBundle& contract_bundle,
         const Side& side,
         const Quantity quantity,
         const Counterparty& counterparty);

  inline const ContractBundle contract_bundle() const {
    return contract_bundle_;
  }
  void print(std::ostream* const p_os) const final;

 private:
  ContractBundle contract_bundle_;
};

// using Orders = std::vector<Single, boost::pool_allocator<Single> >;
using Orders = std::vector<Single>;

}  // namespace iem

#endif
