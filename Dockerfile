FROM ubuntu:18.04 as builder

RUN apt update -y && apt install -y clang libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev make

WORKDIR /dedup
COPY . .

RUN make CC=clang dedup_release -j4

FROM ubuntu:18.04
RUN apt update -y && apt install -y libsdl2-2.0.0 libsdl2-image-2.0.0 libsdl2-ttf-2.0-0
COPY --from=builder /dedup/dedup_release /usr/local/bin/dedup
COPY --from=builder /dedup/font.ttf /usr/local/share/

