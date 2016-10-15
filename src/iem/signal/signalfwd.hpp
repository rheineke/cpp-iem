// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_iem_signal_signalfwd_HPP_
#define iem_src_iem_signal_signalfwd_HPP_

#include "iem/signal/againstsignal.hpp"
#include "iem/signal/arbitragesignal.hpp"
#include "iem/signal/constantsignal.hpp"
#include "iem/signal/constraintsignal.hpp"
#include "iem/signal/lastfillsignal.hpp"
#include "iem/signal/logonvaluesignal.hpp"
#include "iem/signal/netpositionsignal.hpp"
#include "iem/signal/numcontractsatpricesignal.hpp"
#include "iem/signal/numordersignal.hpp"
#include "iem/signal/numorderbookupdatesignal.hpp"
#include "iem/signal/numordersatpricesignal.hpp"
#include "iem/signal/nummarketorderssignal.hpp"
#include "iem/signal/prioritysignal.hpp"
#include "iem/signal/positionsignal.hpp"
#include "iem/signal/ticksinsidebestsignal.hpp"
#include "iem/signal/ticksoutsidesignal.hpp"
#include "iem/signal/toppricesignal.hpp"

namespace {

using iem::Signal;
using iem::signal::AgainstSignal;
using iem::signal::ArbitrageSignal;
using iem::signal::ConstantSignal;
using iem::signal::ConstraintSignal;
using iem::signal::LastFillSignal;
using iem::signal::LogOnValueSignal;
using iem::signal::MarketPriceSumSignal;
using iem::signal::NetPositionSignal;
using iem::signal::NumContractsAtPriceSignal;
using iem::signal::NumMarketOrdersSignal;
using iem::signal::NumOrderSignal;
using iem::signal::NumOrderBookUpdateSignal;
using iem::signal::NumOrdersAtPriceSignal;
using iem::signal::PositionSignal;
using iem::signal::PrioritySignal;
using iem::signal::TicksInsideBestSignal;
using iem::signal::TicksOutsideSignal;
using iem::signal::TopPriceSignal;

}  // namespace

#endif
