FROM DEBIAN:bookworm-slim@sha256:88200866dfff7ea7f5cbcb6ec7c8a701889efe6fe859fe64d6990e4b07ea4171

ENV DEBIAN_FRONTEND=noninteractive

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

#toolchain  depenencies:
RUN apt-get update \
  && apt-get install -y build-essential \
      gcc \
      g++ \
      gdb \
      clang \
      make \
      ninja-build \
      cmake \
      autoconf \
      automake \
      libtool \
      valgrind \
      locales-all \
      dos2unix \
      rsync \
      tar \
      python \
      python-dev \
      #project dependecies: \
      bash \
      mpv \
      #dev tools \
  && apt-get clean

ENTRYPOINT ["top", "-b"]