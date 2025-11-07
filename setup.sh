#!/bin/bash

# Function to update and install packages for apt (Debian/Ubuntu)
install_with_apt() {
  sudo apt-get update && sudo apt-get install -y \
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
}

# Function to update and install packages for yum (CentOS/RHEL)
install_with_yum() {
  sudo yum update -y && sudo yum install -y \
    gcc-c++ \
    python3 \
    python3-pip \
    git \
    cmake \
    clang \
    gmp-devel \
    ntl-devel \
    golang
}

# Function to update and install packages for dnf (Fedora)
install_with_dnf() {
  sudo dnf update -y && sudo dnf install -y \
    gcc-c++ \
    python3 \
    python3-pip \
    git \
    cmake \
    clang \
    gmp-devel \
    ntl-devel \
    golang
}

# Function to update and install packages for pacman (Arch Linux)
install_with_pacman() {
  sudo pacman -Syu --noconfirm && sudo pacman -S --noconfirm \
    base-devel \
    python \
    python-pip \
    git \
    gcc \
    clang \
    cmake \
    gmp \
    ntl \
    go
}

# Detect package manager and install accordingly
if command -v apt-get &> /dev/null; then
  echo "Using apt package manager."
  install_with_apt
elif command -v yum &> /dev/null; then
  echo "Using yum package manager."
  install_with_yum
elif command -v dnf &> /dev/null; then
  echo "Using dnf package manager."
  install_with_dnf
elif command -v pacman &> /dev/null; then
  echo "Using pacman package manager."
  install_with_pacman
else
  echo "No known package manager found. Please install the packages manually."
  exit 1
fi

pip3 install -r requirements.txt