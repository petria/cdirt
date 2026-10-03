FROM ubuntu:22.04

ARG CDIRT_UNVEIL_PASS=AberMUD
ARG CDIRT_CFLAGS="-O2 -g3 -ggdb3 -fno-omit-frame-pointer -fcommon -DCDIRT_DOCKER"
ARG CDIRT_LDFLAGS="-lm -lcrypt"
ARG CDIRT_ASAN_OPTIONS=""

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    libcrypt-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /mud
COPY . /mud

RUN chmod 755 /mud /mud/bin \
    && chmod +x /mud/utils/makedep /mud/bin/configure-container.sh \
    /mud/bin/entrypoint.sh \
    && CDIRT_UNVEIL_PASS="$CDIRT_UNVEIL_PASS" \
       CDIRT_LDFLAGS="$CDIRT_LDFLAGS" \
       CDIRT_CFLAGS="$CDIRT_CFLAGS" /mud/bin/configure-container.sh \
    && cd /mud/src \
    && ASAN_OPTIONS="$CDIRT_ASAN_OPTIONS" make gen \
    && make depend \
    && make all \
    && mv /mud/bin/aberd.new /mud/bin/aberd \
    && test -x /mud/bin/aberd \
    && useradd --system --uid 10001 --home-dir /mud --no-create-home mud \
    && chown -R mud:mud /mud/data

USER mud
WORKDIR /mud/bin

EXPOSE 6715

CMD ["/mud/bin/entrypoint.sh"]
