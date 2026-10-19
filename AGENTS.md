# AGENTS.md

Instructions for coding agents working in cpp-iem, a C++ client for the Iowa Electronic Markets
(IEM). `CLAUDE.md` is a symlink to this file.

## Communication
- Keep summaries, PR descriptions and commit messages short: ideally 60 words or less, and
  never more than 120 words or 3 paragraphs.

## Build and test
- The supported build is Linux arm64 in Docker. It runs the tests, as CI does:
  ```shell
  git submodule update --init --recursive
  docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile -t cpp-iem:trixie .
  ```
- The image is reproducible: base image pinned by digest and apt from a fixed snapshot.debian.org
  date. Keep it that way.
- To run a test binary by hand, use `build/test/iem` as the working directory. Tests read
  `conf/markets.json`, relative to it.
- The macOS build (Homebrew paths in `CMakeLists.txt`) is unmaintained.

## Code
- C++17.
- HTTP goes through `iem::HttpClient` (`src/iem/http_client.hpp`), implemented with cpp-httplib,
  a git submodule pinned to a release tag in `deps/cpp-httplib`. Session tests use
  `test/iem/fake_http_client.hpp`; `http_client_test` runs the real client against a localhost
  server.
- Google style via clang-format; CMake via cmake-format. Run `pre-commit run --all-files`.
- Vendored code is left as upstream wrote it: `deps/` (googletest, cpp-httplib) and `src/json/`,
  `src/jsoncpp.cpp` (jsoncpp).
- Tests use googletest, in `test/iem/<name>_test.cpp`; add new ones to `test/iem/CMakeLists.txt`.

## Safety
- Never commit credentials. `conf/` is git-ignored except `conf/markets.json`; the IEM login
  lives in `conf/login.json`. The gitleaks pre-commit hook scans each commit.
- Tests must not touch the live IEM site.
