// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#ifndef CPPIEM_IEM_HTTP_CLIENT_HPP_
#define CPPIEM_IEM_HTTP_CLIENT_HPP_

#include <memory>
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

// The only network dependency of Session. Tests substitute a fake.
class HttpClient {
 public:
  virtual ~HttpClient() = default;
  virtual HttpResponse send(const HttpRequest& request) = 0;
};

// Client for https://iem.uiowa.edu
std::unique_ptr<HttpClient> make_iem_http_client();

// Percent-encode as application/x-www-form-urlencoded (RFC 3986 unreserved
// characters are kept; everything else is %XX)
std::string url_encode(const NameValuePairs& nvs);

}  // namespace iem

#endif  // CPPIEM_IEM_HTTP_CLIENT_HPP_
