FROM ubuntu:20.04

ENV APPLICATION_BINARY="Application"
ENV OPEN_PORT=8080

RUN apt-get update && \
    ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libboost-all-dev \
        git \
        libsqlite3-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    make

EXPOSE ${OPEN_PORT}

CMD ["./build/Application"]
