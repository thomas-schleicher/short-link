FROM ubuntu:20.04

ENV APPLICATION_BINARY="Application"
ENV OPEN_PORT=8080

COPY ${APPLICATION_BINARY} .

RUN apt-get update && \
    apt-get install -y --no-install-recommends libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

EXPOSE ${OPEN_PORT}
CMD [ "/${APPLICATION_BINARY}" ]