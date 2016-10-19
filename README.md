# README #

This project implements an algorithmic low latency market making strategy on
the Iowa Electronic Markets exchange <https://iem.uiowa.edu/iem/>

### How do I get set up? ###

Instructions for macOS 10.12 (Sierra) using HomeBrew
* Install [HomeBrew]: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
* Install brew packages: brew install wget openssl cmake boost cpp-netlib
* Download waf: wget https://waf.io/waf-1.9.5 && chmod 755 waf-1.9.5 && alias waf=$PWD/waf-1.9.5
* Download iem: git clone https://github.com/rheineke/iem
* Build iem: cd iem && waf configure build -p -j 4

[HomeBrew]: http://brew.sh/

Alternatives: Build with CMake
* Build iem: cmake --target all -j 4 CMakeLists.txt && make
