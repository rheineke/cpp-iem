#!/usr/bin/env bash
set -e

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
  -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
  -DIEM-NETLIB_BUILD_DOCS=$BUILD_DOCS \
  -DBOOST_INCLUDEDIR="${HOME}/${CC}-boost_${BOOST_VERSION}/include" \
  -DBOOST_LIBRARYDIR="${HOME}/${CC}-boost_${BOOST_VERSION}/lib" \
  -DCMAKE_CXX_FLAGS="-std=c++1z ${CMAKE_CXX_FLAGS}" \
  ..
make -j4
make test
cd ..