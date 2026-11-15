// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_HTTP_CLIENT_HPP_
#define CPPIEM_IEM_HTTP_CLIENT_HPP_

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace iem {

// Ordered name-value pairs: HTTP headers repeat (Set-Cookie), and form fields
// keep their order on the wire.
using NameValuePairs = std::vector<std::pair<std::string, std::string>>;

enum class HttpMethod { GET, POST };

// Request to the IEM host. path includes any query string.
struct HttpRequest {
  HttpMethod method = HttpMethod::GET;
  std::string path;
  NameValuePairs headers;
  std::string body;
};

struct HttpResponse {
  int status = 0;
  NameValuePairs headers;
  std::string body;
};

// A request that certainly never reached the server (connection refused, no
// such host, TLS handshake failure): safe to send again.
class HttpNotSent : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

// A request that may have reached the server, with no answer (a timeout or a
// connection dropped mid-request): an order may have been placed, so read the
// outstanding orders before sending it again.
class HttpOutcomeUnknown : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

// The only network dependency of Session. Tests substitute a fake.
class HttpClient {
 public:
  virtual ~HttpClient() = default;
  virtual HttpResponse send(const HttpRequest& request) = 0;
};

// Client for base_url, such as "https://iem.uiowa.edu" or a local test
// server's "http://127.0.0.1:8080". Throws HttpNotSent or HttpOutcomeUnknown
// when a request gets no response.
std::unique_ptr<HttpClient> make_http_client(
    const std::string& base_url,
    std::chrono::seconds read_timeout = std::chrono::seconds(30));

// Client for https://iem.uiowa.edu
std::unique_ptr<HttpClient> make_iem_http_client();

// A path without its ";jsessionid=..." part, for messages and logs.
std::string redact_session(const std::string& path);

// Percent-encode as application/x-www-form-urlencoded (RFC 3986 unreserved
// characters are kept; everything else is %XX)
std::string url_encode(const NameValuePairs& nvs);

}  // namespace iem

#endif  // CPPIEM_IEM_HTTP_CLIENT_HPP_
