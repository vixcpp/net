/**
 *
 *  @file CurlClientTest.cpp
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

#include <vix/net/http/ClientRequest.hpp>
#include <vix/net/http/CurlClient.hpp>
#include <vix/net/http/Method.hpp>

int main()
{
  {
    vix::net::http::CurlClient client;
    vix::net::http::ClientRequest request;

    auto result = client.send(request);

    assert(!result);
  }

  {
    vix::net::http::CurlClient client;
    vix::net::http::ClientRequest request;

    request
        .set_method(vix::net::http::Method::Get)
        .set_url("localhost:8080");

    auto result = client.send(request);

    assert(!result);
  }

  {
    vix::net::http::CurlClient client;
    vix::net::http::ClientRequest request;

    request
        .set_method(vix::net::http::Method::Get)
        .set_url("http://127.0.0.1:1")
        .set_timeout_ms(1000);

    auto result = client.send(request);

    assert(result);

    const auto &response = result.value();

    assert(!response.success());
    assert(response.has_error() || !response.body.empty());
  }

  {
    vix::net::http::ClientRequest request;

    request
        .set_method(vix::net::http::Method::Post)
        .set_url("http://127.0.0.1:11434/api/generate")
        .set_header("Content-Type", "application/json")
        .set_body("{\"model\":\"llama3\",\"prompt\":\"hello\",\"stream\":false}")
        .set_timeout_ms(30000);

    assert(request.valid());
    assert(request.method == vix::net::http::Method::Post);
    assert(request.headers.at("Content-Type") == "application/json");
    assert(!request.body.empty());
  }

  return 0;
}
