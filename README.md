# vix::net

Low-level networking primitives for Vix.cpp.

`vix::net` provides small, reusable network utilities used by Vix modules that need connectivity checks or outbound HTTP communication without depending directly on platform details or external process logic.

## Features

- Connectivity probing with cached online/offline state
- Outbound HTTP client abstraction
- Curl-backed HTTP client backend
- Structured error handling through `vix::error`
- Process-backed implementation through `vix::process`
- Clean separation between server-side HTTP and client-side HTTP

## Modules

```txt
include/vix/net/
  NetworkProbe.hpp
  net.hpp

include/vix/net/http/
  Method.hpp
  HeaderMap.hpp
  ClientRequest.hpp
  ClientResponse.hpp
  ClientError.hpp
  Client.hpp
  CurlClient.hpp
  http.hpp
```

## HTTP client

The HTTP client layer lives under:

```cpp
#include <vix/net/http/http.hpp>
```

Namespace:

```
vix::net::http
```

### Basic usage

```cpp
#include <iostream>

#include <vix/net/http/http.hpp>

int main()
{
  vix::net::http::CurlClient client;

  vix::net::http::ClientRequest request;

  request
      .set_method(vix::net::http::Method::Get)
      .set_url("https://example.com")
      .set_timeout_ms(5000);

  auto result = client.send(request);

  if (!result)
  {
    std::cerr << result.error().message() << '\n';
    return 1;
  }

  const auto &response = result.value();

  std::cout << "Status: " << response.status_code << '\n';
  std::cout << response.body << '\n';

  return response.success() ? 0 : 1;
}
```

### POST JSON

```cpp
#include <iostream>

#include <vix/net/http/http.hpp>

int main()
{
  vix::net::http::CurlClient client;

  vix::net::http::ClientRequest request;

  request
      .set_method(vix::net::http::Method::Post)
      .set_url("http://127.0.0.1:11434/api/generate")
      .set_header("Content-Type", "application/json")
      .set_body(R"({"model":"llama3","prompt":"Hello","stream":false})")
      .set_timeout_ms(30000);

  auto result = client.send(request);

  if (!result)
  {
    std::cerr << result.error().message() << '\n';
    return 1;
  }

  const auto &response = result.value();

  if (!response.success())
  {
    std::cerr << response.error << '\n';
    return 1;
  }

  std::cout << response.body << '\n';

  return 0;
}
```

## NetworkProbe

`NetworkProbe` provides a lightweight connectivity probe with caching and rate limiting.

```cpp
#include <iostream>

#include <vix/net/NetworkProbe.hpp>

int main()
{
  vix::net::NetworkProbe probe(
      vix::net::NetworkProbe::Config{},
      []()
      {
        return true;
      });

  const bool online = probe.isOnline(1000);

  std::cout << (online ? "online" : "offline") << '\n';

  return 0;
}
```

## Design

`vix::net` is responsible for outbound network utilities.

```
vix::net
  connectivity probing
  outbound HTTP client abstraction
  HTTP client backends
```

Server-side HTTP remains in `vix::http`, `vix::server`, `vix::router`, and `vix::session`.

```
vix::http
  server request
  server response
  request handler
  response wrapper

vix::server
  HTTP server

vix::router
  route matching and dispatch

vix::session
  connection/session transport
```

This keeps the responsibilities clear:

```
core/http = server-side HTTP framework
net/http  = outbound HTTP client
process   = external process execution
```

## Dependencies

Public dependencies:

- `vix::error`
- `vix::process`

`CurlClient` uses `vix::process` internally to execute the curl command-line tool. This keeps curl hidden behind the `vix::net::http::Client` abstraction.

Future backends can be added without changing users of the client interface:

```
CurlClient
NativeClient
AsioClient
TlsClient
```

## CMake

### Standalone

```bash
vix build
```

### With tests

```bash
vix build -- -DVIX_NET_BUILD_TESTS=ON
vix tests
```

### Consumer usage

```cmake
find_package(vix_net CONFIG REQUIRED)

target_link_libraries(my_app PRIVATE vix::net)
```

## Build options

| Option | Purpose |
|--------|---------|
| `VIX_NET_BUILD_TESTS` | Build net module tests |
| `VIX_NET_FETCH_ERROR` | Auto-fetch `vix::error` if missing |
| `VIX_NET_FETCH_PROCESS` | Auto-fetch `vix::process` if missing |
| `VIX_NET_FETCH_TESTS` | Auto-fetch `vix::tests` if missing |

## License

MIT
