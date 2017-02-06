#!/usr/bin/env bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    brew update
    brew upgrade
    <.travis/brew.txt xargs brew install
elif [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
    sudo add-apt-repository ppa:llvm-toolchain-precise-3.6
    sudo add-apt-repository ppa:kalakris-cmake -y
    sudo apt-get update -q
    if [[ $CXX == 'clang++' ]]; then
      sudo apt-get install -y clang-3.8
    elif [[ $CXX == 'g++' ]]; then
      sudo apt-get install -y gcc-6 g++-6
    fi
    sudo apt-get install -y libboost-all-dev openssl libssl-dev
    sudo apt-get install -y cmake libcppnetlib-dev

    sudo apt list --installed
fi
