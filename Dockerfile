FROM ubuntu:20.04

# disable tzdata questions
ENV DEBIAN_FRONTEND=noninteractive

# use bash
SHELL ["/bin/bash", "-c"]

# dependencies
RUN apt-get update -y && apt-get install -y --no-install-recommends \
  freeglut3-dev \
  libxmu-dev \
  libxi-dev \
  libz-dev \
  g++ \
  make \
  cmake \
  cmake-curses-gui \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# build vismatrix
WORKDIR /app
COPY . .
RUN cd src && cmake . && make
