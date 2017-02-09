#!/usr/bin/env bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    brew update
    brew upgrade
    <.travis/brew.txt xargs brew install
elif [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "Nothing to do here"
    # apt list --installed
fi
