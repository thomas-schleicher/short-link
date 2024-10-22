FROM ubuntu:20.04

ENV APPLICATION_BINARY="Application"
ENV OPEN_PORT=8080

COPY ${APPLICATION_BINARY} .

RUN apt-get update && \
    ln -fs /usr/share/zoneinfo/America/New_York /etc/localtime \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

EXPOSE ${OPEN_PORT}
CMD [ "/${APPLICATION_BINARY}" ]