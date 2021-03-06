FROM ubuntu as builder

RUN apt update -y && DEBIAN_FRONTEND=noninteractive apt install -y clang libsdl2-dev libsdl2-image-dev make libglew-dev

WORKDIR /dedup
COPY . .

RUN make CC=clang dedup_release -j4

FROM ubuntu
RUN apt update -y && DEBIAN_FRONTEND=noninteractive apt install -y libsdl2-2.0-0 libsdl2-image-2.0-0 libglew2.1
COPY --from=builder /dedup/dedup_release /usr/local/bin/dedup
