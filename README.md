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
