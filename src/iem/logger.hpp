// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef IEM_SRC_IEM_LOGGER_HPP_
#define IEM_SRC_IEM_LOGGER_HPP_

#include <iostream>

namespace iem {

class Logger final {
 public:
  static constexpr std::size_t kBufferSize = 256;

  constexpr char* getBuffer() const { return buffer_; }

  void log() const { std::cout << buffer_ << std::endl; }

 private:
  static char buffer_[kBufferSize];
};

}  // namespace iem

#endif
