#!/usr/bin/env bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    brew update
    brew upgrade
    <.travis/brew.txt xargs brew install
elif [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    add-apt-repository ppa:ubuntu-toolchain-r/test -y
    add-apt-repository ppa:llvm-toolchain-precise-3.6
    add-apt-repository ppa:kalakris-cmake -y
    apt-get update -q
    if [[ $CXX == 'clang++' ]]; then
      apt-get install -y clang-3.8
    elif [[ $CXX == 'g++' ]]; then
      apt-get install -y gcc-6 g++-6
    fi
    apt-get install -y libboost-all-dev openssl libssl-dev
    apt-get install -y cmake libcppnetlib-dev

    apt list --installed
fi
