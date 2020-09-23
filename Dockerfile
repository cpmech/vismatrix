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
  cmake \
  && apt-get clean && rm -rf /var/lib/apt/lists/*

# build vismatrix
WORKDIR /app
COPY . .
