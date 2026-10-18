// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/http_client.hpp"

#include <cctype>
#include <cstdio>

#include "boost/network/protocol/http/client.hpp"

namespace iem {

namespace {

std::string percent_encode(const std::string& s) {
  std::string out;
  for (const unsigned char c : s) {
    if (std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
      out += static_cast<char>(c);
    } else {
      char buf[4];
      std::snprintf(buf, sizeof(buf), "%%%02X", c);
      out += buf;
    }
  }
  return out;
}

class NetlibHttpClient final : public HttpClient {
 public:
  HttpResponse send(const HttpRequest& request) final {
    using boost::network::http::client;
    client::request netlib_request("https://iem.uiowa.edu" + request.path);
    for (const auto& h : request.headers) {
      netlib_request << boost::network::header(h.first, h.second);
    }
    if (!request.body.empty()) {
      netlib_request << boost::network::body(request.body);
    }
    const client::response netlib_response =
        (request.method == HttpMethod::POST) ? client_.post(netlib_request)
                                             : client_.get(netlib_request);
    HttpResponse response;
    response.status = status(netlib_response);
    for (const auto& h : headers(netlib_response)) {
      response.headers.emplace_back(h.first, h.second);
    }
    response.body = body(netlib_response);
    return response;
  }

 private:
  boost::network::http::client client_;
};

}  // namespace

std::unique_ptr<HttpClient> make_iem_http_client() {
  return std::make_unique<NetlibHttpClient>();
}

std::string url_encode(const NameValuePairs& nvs) {
  std::string out;
  for (const auto& nv : nvs) {
    if (!out.empty()) {
      out += '&';
    }
    out += percent_encode(nv.first) + '=' + percent_encode(nv.second);
  }
  return out;
}

}  // namespace iem
