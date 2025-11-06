FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    python3 \
    python3-pip \
    git \
    gcc \
    g++ \
    cmake \
    clang \
    libgmp-dev \
    libntl-dev \
    golang

WORKDIR /root

COPY . /root

RUN if [ -f requirements.txt ]; then pip3 install -r requirements.txt; fi
