# README #

This project implements an algorithmic low latency market making
strategy on the [Iowa Electronic Markets] exchange.

### How do I get set up? ###

Instructions for macOS 10.12 (Sierra) using [HomeBrew]:
* Install [HomeBrew]: `/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
* Install brew packages: `brew install wget openssl cmake boost homebrew/boneyard/cpp-netlib`
* Download cpp-iem: `git clone --recursive -j4 https://github.com/rheineke/cpp-iem.git`
* Build cpp-iem: `cd cpp-iem & CMAKE_C_COMPILER=clang CMAKE_CXX_COMPILER=clang++ ./build-cmake.sh`

* Alternatively, build cpp-iem with waf by replacing last command:
* Download waf: `wget https://waf.io/waf-1.9.5 && chmod 755 waf-1.9.5 && alias waf=$PWD/waf-1.9.5`
* Build cpp-iem: `cd cpp-iem & ./build-waf.sh`

[HomeBrew]: http://brew.sh/
[Iowa Electronic Markets]: https://iem.uiowa.edu/iem/
[waf]: https://waf.io/

## Pre-commit hooks
Whitespace and file checks, gitleaks, clang-format (Google style) and cmake-format/cmake-lint.
Vendored code (`deps/`, jsoncpp) is excluded.
```shell
pip install pre-commit
pre-commit install
pre-commit run --all-files
```

## Linux arm64 build (Docker)
A reproducible build on a digest-pinned Debian 13 (trixie) image. Packages come from a fixed
snapshot.debian.org date with pinned versions (Boost 1.83, OpenSSL 3, CMake, GCC 14), and
cpp-netlib 0.13.0 is built from a pinned commit with `docker/patches/` applied for Boost 1.83.
The build runs the tests, and CI runs it on every pull request.
```shell
git submodule update --init --recursive
docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile -t cpp-iem:trixie .
```
