# Reproducible Linux arm64 build of cpp-iem on Debian 13 (trixie).
#   docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile -t cpp-iem:trixie .
FROM debian:trixie@sha256:913f6706df59a68922d1dd08f78c2476560a8d367897200a6005b00e5f67c2d5

# Install from a fixed snapshot.debian.org date so the pinned versions stay available
ARG SNAPSHOT=20261017T000000Z
ARG DEBIAN_FRONTEND=noninteractive
RUN printf '%s\n' \
        'Types: deb' \
        "URIs: http://snapshot.debian.org/archive/debian/${SNAPSHOT}" \
        'Suites: trixie trixie-updates' \
        'Components: main' \
        'Signed-By: /usr/share/keyrings/debian-archive-keyring.gpg' \
        '' \
        'Types: deb' \
        "URIs: http://snapshot.debian.org/archive/debian-security/${SNAPSHOT}" \
        'Suites: trixie-security' \
        'Components: main' \
        'Signed-By: /usr/share/keyrings/debian-archive-keyring.gpg' \
        > /etc/apt/sources.list.d/debian.sources \
    && apt-get -o Acquire::Check-Valid-Until=false update \
    && apt-get install -y --no-install-recommends \
        ca-certificates=20250419 \
        cmake=3.31.6-2 \
        g++=4:14.2.0-1 \
        git=1:2.47.3-0+deb13u1 \
        libboost-all-dev=1.83.0.2+b2 \
        libssl-dev=3.5.7-1~deb13u3 \
        make=4.4.1-2 \
    && rm -rf /var/lib/apt/lists/*

# cpp-netlib 0.13.0-final, pinned by commit. The patch adds includes that GCC 14 and
# Boost 1.83 no longer pull in transitively.
ARG CPPNETLIB_COMMIT=ca95f04d140acf619892ee02a82e930dd91ff7d4
COPY docker/patches/ /opt/patches/
RUN git clone https://github.com/cpp-netlib/cpp-netlib.git /opt/cpp-netlib-src \
    && cd /opt/cpp-netlib-src \
    && git checkout --quiet ${CPPNETLIB_COMMIT} \
    && git submodule update --init --recursive \
    && git apply /opt/patches/cpp-netlib-0.13.0-boost-1.83.patch \
    && cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCPP-NETLIB_BUILD_TESTS=OFF \
        -DCPP-NETLIB_BUILD_EXAMPLES=OFF \
        -DCPP-NETLIB_ENABLE_HTTPS=ON \
    && cmake --build build -j"$(nproc)" --target install \
    && cd / && rm -rf /opt/cpp-netlib-src

WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j"$(nproc)" \
    && cd build && ctest --output-on-failure
