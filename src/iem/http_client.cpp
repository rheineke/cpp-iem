// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
#include "iem/http_client.hpp"

#include <cctype>
#include <cstdio>
#include <stdexcept>

#include "httplib.h"

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

// Errors before any byte of the request could reach the server.
bool never_sent(const httplib::Error error) {
  switch (error) {
    case httplib::Error::Connection:
    case httplib::Error::BindIPAddress:
    case httplib::Error::ConnectionTimeout:
    case httplib::Error::ProxyConnection:
    case httplib::Error::SSLConnection:
    case httplib::Error::SSLLoadingCerts:
    case httplib::Error::SSLServerVerification:
    case httplib::Error::SSLServerHostnameVerification:
      return true;
    default:
      return false;
  }
}

class HttplibHttpClient final : public HttpClient {
 public:
  explicit HttplibHttpClient(const std::string& base_url) : client_(base_url) {
    // Session paths and queries are already percent-encoded
    client_.set_path_encode(false);
    client_.set_connection_timeout(10);
    client_.set_read_timeout(30);
    // Redirects are not followed: login reads the Location header itself
    client_.set_follow_location(false);
  }

  HttpResponse send(const HttpRequest& request) final {
    httplib::Request req;
    req.method = (request.method == HttpMethod::POST) ? "POST" : "GET";
    req.path = request.path;
    for (const auto& h : request.headers) {
      req.headers.emplace(h.first, h.second);
    }
    req.body = request.body;

    const auto result = client_.send(req);
    if (!result) {
      // The path may be one the server chose (a redirect) and carry a session
      // id: keep it out of the message.
      const auto what = req.method + " " + redact_session(request.path) +
                        " failed: " + httplib::to_string(result.error());
      if (never_sent(result.error())) {
        throw HttpNotSent(what);
      }
      throw HttpOutcomeUnknown(what);
    }
    HttpResponse response;
    response.status = result->status;
    for (const auto& h : result->headers) {
      response.headers.emplace_back(h.first, h.second);
    }
    response.body = result->body;
    return response;
  }

 private:
  httplib::Client client_;
};

}  // namespace

std::string redact_session(const std::string& path) {
  // "/x;jsessionid=ABC?y=1" -> "/x?y=1"
  const auto start = path.find(";jsessionid=");
  if (start == std::string::npos) {
    return path;
  }
  const auto end = path.find_first_of("?#", start);
  return path.substr(0, start) +
         (end == std::string::npos ? "" : path.substr(end));
}

std::unique_ptr<HttpClient> make_http_client(const std::string& base_url) {
  return std::make_unique<HttplibHttpClient>(base_url);
}

std::unique_ptr<HttpClient> make_iem_http_client() {
  return make_http_client("https://iem.uiowa.edu");
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
