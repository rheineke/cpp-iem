# README #

This project implements an algorithmic low latency market making strategy on
the Iowa Electronic Markets exchange <https://iemweb.biz.uiowa.edu/>

### How do I get set up? ###

Instructions for Mac OS X 10.11 (El Capitan) using HomeBrew
* Install [HomeBrew]: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
* Install brew packages: brew install openssl cmake boost cpp-netlib
* Download waf: curl "https://waf.io/waf-1.8.20" -o waf-1.8.20 && chmod 755 waf-1.8.20/waf
* Download iem: git clone https://github.com/rheineke/iem
* Build iem: cd iem && waf configure build -p -j 4

[HomeBrew]: http://brew.sh/

Alternatives: Build with CMake
* Build iem: cmake --target all -j 4 CMakeLists.txt && make
