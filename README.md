# README #

This project implements an algorithmic low latency market making
strategy on the [Iowa Electronic Markets] exchange.

### How do I get set up? ###

The supported build is Linux arm64 in Docker (below). A native build needs CMake 3.25+, a C++20
compiler, Boost 1.83+, OpenSSL 3 and zlib; on macOS (untested): `brew install cmake boost openssl@3`.
```shell
git clone --recursive https://github.com/rheineke/cpp-iem.git
cd cpp-iem
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

[Iowa Electronic Markets]: https://iem.uiowa.edu/iem/

## IEM login
`CPP-IEM` reads the login from `IEM_USERNAME` and `IEM_PASSWORD`, and `conf/markets.json` from
the working directory, so run it from the repo root. Keep the password in a secret store, not a
file. With the macOS Keychain:
```shell
security add-generic-password -s cpp-iem -a "<username>" -w   # prompts for the password
IEM_USERNAME="<username>" \
IEM_PASSWORD="$(security find-generic-password -s cpp-iem -w)" build/CPP-IEM
```
Or with the 1Password CLI, from an item called IEM:
```shell
IEM_USERNAME=op://Private/IEM/username IEM_PASSWORD=op://Private/IEM/password op run -- build/CPP-IEM
```

## Pre-commit hooks
Whitespace and file checks, codespell, gitleaks, clang-format (Google style) and
cmake-format/cmake-lint. Vendored code (`deps/`) is excluded. CI runs them on every pull request;
since the gitleaks hook checks only staged changes, CI also scans the whole git history.
```shell
pip install pre-commit
pre-commit install
pre-commit run --all-files
```

## Linux arm64 build (Docker)
A reproducible build on a digest-pinned Debian 13 (trixie) image. Packages come from a fixed
snapshot.debian.org date with pinned versions (Boost 1.83, OpenSSL 3, zlib, CMake, GCC 14).
The HTTP client ([cpp-httplib](https://github.com/yhirose/cpp-httplib)), the JSON parser
([nlohmann/json](https://github.com/nlohmann/json)) and googletest are git submodules pinned to
release tags. The build runs the tests, then the static analysis: clang-tidy (checks in
`.clang-tidy`) and cppcheck, where any finding fails the build. CI runs it on every pull request.
```shell
git submodule update --init --recursive
docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile -t cpp-iem:trixie .
# Tests only
docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile --target test .
```
