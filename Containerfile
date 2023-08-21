FROM docker.io/alpine:3 AS builder

RUN apk add alpine-sdk \
    confuse \
    confuse-dev \
    check \
    check-dev \
    linux-headers

WORKDIR /src
COPY . /src
RUN make realclean test && make realclean all

FROM docker.io/alpine:3

RUN apk add confuse
COPY --from=builder /src/picocom /usr/local/bin/picocom
ENTRYPOINT ["/usr/local/bin/picocom"]
