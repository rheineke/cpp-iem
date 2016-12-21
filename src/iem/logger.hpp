// Copyright 2014 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_LOGGER_HPP_
#define CPPIEM_IEM_LOGGER_HPP_

#include <iostream>

namespace iem {

class Logger final {
 public:
  static constexpr std::size_t kBufferSize = 256;

  constexpr char* getBuffer() const { return buffer_; }

  void log() const { std::cout << buffer_ << '\n'; }

 private:
  static char buffer_[kBufferSize];
};

}  // namespace iem

#endif  // CPPIEM_IEM_LOGGER_HPP_
