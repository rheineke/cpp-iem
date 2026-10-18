// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_TEST_IEM_FAKE_HTTP_CLIENT_HPP_
#define CPPIEM_TEST_IEM_FAKE_HTTP_CLIENT_HPP_

#include <deque>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "iem/http_client.hpp"

namespace iem {

// Canned responses, served in order, and the requests Session sent. Owned by
// the test so it outlives the Session that owns the client.
struct FakeServer {
  std::deque<HttpResponse> responses;
  std::vector<HttpRequest> requests;
};

class FakeHttpClient final : public HttpClient {
 public:
  explicit FakeHttpClient(FakeServer* server) : server_(server) {}

  HttpResponse send(const HttpRequest& request) final {
    server_->requests.push_back(request);
    if (server_->responses.empty()) {
      throw std::logic_error("FakeHttpClient: unexpected request to " +
                             request.path);
    }
    auto response = server_->responses.front();
    server_->responses.pop_front();
    return response;
  }

 private:
  FakeServer* const server_;
};

inline std::unique_ptr<HttpClient> make_fake_client(FakeServer* server) {
  return std::make_unique<FakeHttpClient>(server);
}

inline HttpResponse ok(const std::string& body) {
  return HttpResponse{200, {}, body};
}

// Value of the first header called name, or "" if there is none
inline std::string header(const NameValuePairs& headers,
                          const std::string& name) {
  for (const auto& h : headers) {
    if (h.first == name) {
      return h.second;
    }
  }
  return "";
}

}  // namespace iem

#endif  // CPPIEM_TEST_IEM_FAKE_HTTP_CLIENT_HPP_
