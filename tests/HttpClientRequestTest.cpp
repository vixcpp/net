/**
 *
 *  @file HttpClientRequestTest.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */

#include <cassert>
#include <string_view>

#include <vix/net/http/ClientRequest.hpp>
#include <vix/net/http/ClientResponse.hpp>
#include <vix/net/http/Method.hpp>

int main()
{
  {
    vix::net::http::ClientRequest request;

    assert(request.method == vix::net::http::Method::Get);
    assert(request.url.empty());
    assert(request.body.empty());
    assert(request.headers.empty());
    assert(request.timeout_ms == 0);
    assert(!request.valid());
  }

  {
    vix::net::http::ClientRequest request;

    request
        .set_method(vix::net::http::Method::Post)
        .set_url("http://127.0.0.1:11434/api/generate")
        .set_header("Content-Type", "application/json")
        .set_body("{\"model\":\"llama3\"}")
        .set_timeout_ms(30000);

    assert(request.valid());
    assert(request.method == vix::net::http::Method::Post);
    assert(request.url == "http://127.0.0.1:11434/api/generate");
    assert(request.body == "{\"model\":\"llama3\"}");
    assert(request.timeout_ms == 30000);
    assert(request.headers.at("Content-Type") == "application/json");
  }

  {
    using vix::net::http::Method;
    using vix::net::http::to_string;

    assert(to_string(Method::Get) == "GET");
    assert(to_string(Method::Post) == "POST");
    assert(to_string(Method::Put) == "PUT");
    assert(to_string(Method::Patch) == "PATCH");
    assert(to_string(Method::Delete) == "DELETE");
    assert(to_string(Method::Head) == "HEAD");
    assert(to_string(Method::Options) == "OPTIONS");
  }

  {
    vix::net::http::ClientResponse response;

    assert(response.status_code == 0);
    assert(response.body.empty());
    assert(response.error.empty());
    assert(!response.success());
    assert(!response.has_error());
    assert(!response.has_status_code());

    response.status_code = 200;
    response.body = "OK";
    response.headers["Content-Type"] = "text/plain";

    assert(response.success());
    assert(response.has_status_code());
    assert(response.header("Content-Type") == "text/plain");
    assert(response.header("Missing").empty());
  }

  {
    vix::net::http::ClientResponse response;

    response.status_code = 404;
    response.error = "not found";

    assert(!response.success());
    assert(response.has_error());
    assert(response.has_status_code());
  }

  return 0;
}
