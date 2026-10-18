# Reproducible Linux arm64 build of cpp-iem.
#   docker build --platform linux/arm64 -f docker/linux-arm64.Dockerfile -t cpp-iem:linux-arm64 .
# Ubuntu 18.04 is the newest release whose Boost (1.65) and OpenSSL 1.0.2 still work
# with cpp-netlib 0.12 and its bundled asio.
FROM ubuntu:18.04@sha256:152dc042452c496007f07ca9127571cb9c29697f42acbfad72324b2bb2e43c98

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates \
        cmake=3.10.2-1ubuntu2.18.04.2 \
        g++=4:7.4.0-1ubuntu2.3 \
        git \
        libboost-all-dev=1.65.1.0ubuntu1 \
        libssl1.0-dev=1.0.2n-1ubuntu5.13 \
        make \
    && rm -rf /var/lib/apt/lists/*

# cpp-netlib 0.12.0-final, pinned by commit
ARG CPPNETLIB_COMMIT=89f97682ba7ba68291b1ca9be21303d9c9d1c052
RUN git clone https://github.com/cpp-netlib/cpp-netlib.git /opt/cpp-netlib-src \
    && cd /opt/cpp-netlib-src \
    && git checkout --quiet ${CPPNETLIB_COMMIT} \
    && git submodule update --init --recursive \
    && mkdir build && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCPP-NETLIB_BUILD_TESTS=OFF \
        -DCPP-NETLIB_BUILD_EXAMPLES=OFF \
        -DCPP-NETLIB_ENABLE_HTTPS=ON \
    && make -j"$(nproc)" install \
    # Installed headers include the bundled standalone asio, which install skips
    && cp -r ../deps/asio/asio/include/asio ../deps/asio/asio/include/asio.hpp /usr/local/include/ \
    && cd / && rm -rf /opt/cpp-netlib-src

WORKDIR /src
COPY . .
RUN mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j"$(nproc)" \
    && ctest --output-on-failure
