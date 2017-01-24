#!/usr/bin/env bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    brew update
    brew upgrade
    <.travis/brew.txt xargs brew install
else
    apt-get update && apt-get upgrade
    apt-get install -y cmake
fi
