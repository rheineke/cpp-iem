// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef iem_src_debug_HPP_
#define iem_src_debug_HPP_

#include "iem/logger.hpp"

namespace debug {

template<typename T>
void log(const Logger& logger, const T* t, const char* const msg) {
  snprintf(logger.getBuffer(), logger.kBufferSize, "[%ld] %s %ld", t, msg,
           sizeof(*t));
  logger.log();
}

}

#endif
