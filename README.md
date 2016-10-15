# README #

This project implements an algorithmic low latency market making strategy on
the Iowa Electronic Markets exchange <https://iemweb.biz.uiowa.edu/>

### How do I get set up? ###

Instructions for Mac OS X 10.11 (El Capitan) using HomeBrew
* Install [HomeBrew]:[]: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
* Install brew packages: brew install openssl cmake boost cpp-netlib
* Download waf: curl "https://waf.io/waf-1.8.20" -o waf-1.8.20 && chmod 755 waf-1.8.20/waf
* Download iem: hg clone https://bitbucket.org/rheineke/iem
* Build iem: cd iem && waf configure build -p -j 4

[HomeBrew]: http://brew.sh/

Alternatives: Build with CMake
* Build iem: cmake --target all -j 4 CMakeLists.txt && make

### Other considerations ###

# SSL issues can be hard to diagnose but will be a consideration with the IEM
# website. The following may fail depending on your version of OpenSSL:
openssl s_client -debug -connect iemweb.biz.uiowa.edu:80
# while this should succeed regardless of version:
openssl s_client -tls1 -connect iemweb.biz.uiowa.edu:80
