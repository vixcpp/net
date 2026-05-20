# Vix.cpp Net Module

Low-level networking utilities for Vix.cpp.

The Net module provides small, reusable networking primitives used by Vix modules that need connectivity checks or outbound HTTP communication.

## Documentation

Full documentation will be available here:

https://docs.vixcpp.com/modules/net/

API reference:

https://docs.vixcpp.com/modules/net/api-reference

## What Net provides

- Connectivity probing
- Cached online/offline state
- Outbound HTTP client abstraction
- Curl-backed HTTP client backend
- Structured error handling through `vix::error`
- Process-backed execution through `vix::process`
- Clean separation between server-side HTTP and client-side HTTP

## Public headers

```cpp
#include <vix/net/NetworkProbe.hpp>
#include <vix/net/http/http.hpp>
```

## HTTP client example

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

## POST JSON example

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

## Network probe example

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

## Architecture

```text
vix::net
  -> connectivity probing
  -> outbound HTTP client abstraction
  -> HTTP client backends
```

Server-side HTTP stays in Core:

```text
vix::http
  -> server request and response

vix::server
  -> HTTP server

vix::router
  -> route matching and dispatch

vix::session
  -> connection and transport lifecycle
```

This keeps responsibilities clear:

```text
core/http = server-side HTTP framework
net/http  = outbound HTTP client
process   = external process execution
```

## Dependencies

- `vix::error`
- `vix::process`

`CurlClient` uses `vix::process` internally to execute the `curl` command-line tool behind the `vix::net::http::Client` abstraction.

Future backends can be added without changing user code:

```text
CurlClient
NativeClient
AsioClient
TlsClient
```

## Build

Contributors should use the Vix CLI to build this module.

Vix wraps the C++ build workflow with project detection, presets, Ninja builds, clean logs, caching, and focused diagnostics. This keeps the contributor workflow consistent and helps avoid hidden C++ build issues.

### Build the project

```bash
git clone https://github.com/vixcpp/vix.git
cd vix
vix build
```

### Build with Net tests

```bash
vix build -- -DVIX_NET_BUILD_TESTS=ON
```

### Build all targets

Use this before running the full test suite, install workflows, or release checks:

```bash
vix build --build-target all
```

### Clean rebuild

Use this when the local CMake cache or build directory may be stale:

```bash
vix build --clean
```

### Release build

```bash
vix build --preset release
```

## Tests

Build all targets first, then run tests:

```bash
vix build --build-target all
vix tests
```

Before opening a pull request, use:

```bash
vix fmt --check
vix build --build-target all
vix tests
```

## CMake consumer usage

```cmake
find_package(vix_net CONFIG REQUIRED)

target_link_libraries(my_app PRIVATE vix::net)
```

## Build options

| Option | Purpose |
| --- | --- |
| `VIX_NET_BUILD_TESTS` | Build Net module tests |
| `VIX_NET_FETCH_ERROR` | Auto-fetch `vix::error` if missing |
| `VIX_NET_FETCH_PROCESS` | Auto-fetch `vix::process` if missing |
| `VIX_NET_FETCH_TESTS` | Auto-fetch `vix::tests` if missing |

## Useful links

- Net documentation: https://docs.vixcpp.com/modules/net/
- Net API reference: https://docs.vixcpp.com/modules/net/api-reference
- Build command: https://docs.vixcpp.com/cli/build
- Tests command: https://docs.vixcpp.com/cli/tests
- Documentation: https://docs.vixcpp.com/
- Engineering notes: https://blog.vixcpp.com/
- Registry: https://registry.vixcpp.com/
- GitHub: https://github.com/vixcpp/vix

## License

MIT License.

See [`LICENSE`](../../LICENSE) for details.
