# Reproducible Linux arm64 build of cpp-iem on Debian 13 (trixie). Builds, runs
# the tests, then the static analysis (`lint` stage):
#   docker build --platform linux/arm64 -f docker/debian-trixie.Dockerfile -t cpp-iem:trixie .
# Add --target test to skip the static analysis.
FROM debian:trixie@sha256:913f6706df59a68922d1dd08f78c2476560a8d367897200a6005b00e5f67c2d5 AS test

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
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    && cmake --build build -j"$(nproc)" \
    && cd build && ctest --output-on-failure

# clang-tidy (checks in .clang-tidy), then cppcheck; any finding fails the build.
# Both read build/compile_commands.json and skip vendored code: deps/ and jsoncpp
# (src/json/, src/jsoncpp.cpp).
FROM test AS lint
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get -o Acquire::Check-Valid-Until=false update \
    && apt-get install -y --no-install-recommends \
        clang-tidy-19=1:19.1.7-3+b1 \
        cppcheck=2.17.1-2 \
    && rm -rf /var/lib/apt/lists/*
RUN run-clang-tidy-19 -p build -quiet -j"$(nproc)" \
        '^/src/src/(main\.cpp|iem/)' '^/src/test/iem/' \
    && cppcheck --project=build/compile_commands.json \
        --enable=warning,performance,portability --check-level=exhaustive \
        --inline-suppr --error-exitcode=1 --quiet -j"$(nproc)" \
        --file-filter='src/iem/*' --file-filter=src/main.cpp \
        --file-filter='test/iem/*' --suppress='*:deps/*' --suppress='*:src/json/*'
