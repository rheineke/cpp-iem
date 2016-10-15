// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#include "iem/logger.hpp"

namespace iem {

constexpr std::size_t Logger::kBufferSize;

char Logger::buffer_[Logger::kBufferSize] = "";

}  // namespace iem
