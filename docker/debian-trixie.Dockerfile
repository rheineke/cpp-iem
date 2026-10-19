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
        libboost-all-dev=1.83.0.2+b2 \
        libssl-dev=3.5.7-1~deb13u3 \
        make=4.4.1-2 \
        zlib1g-dev=1:1.3.dfsg+really1.3.1-1+b1 \
    && rm -rf /var/lib/apt/lists/*

# cpp-httplib and googletest are git submodules under deps/, copied in with the source
WORKDIR /src
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j"$(nproc)" \
    && cd build && ctest --output-on-failure
