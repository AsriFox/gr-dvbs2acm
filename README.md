# gr-dvbs2acm

## Overview
**gr-dvbs2acm** is a GNU Radio OOT module including transmitter and receiver blocks for software-defined radio and (primarily) message passing simulation.

This repository consists of two parts: transmitter and receiver. The transmitter is a fork of drmpeg's [gr-dvbs2](https://github.com/drmpeg/gr-dvbs2) project. The receiver is a fork of igorauad's [gr-dvbs2rx](https://github.com/igorauad/gr-dvbs2rx) project. In particular, this fork adds VCM (Variable Coding and Modulation) support to receiver blocks with the same stream tags interface as in the transmitter blocks.

This project is licensed under GPLv3 and was developed primarily for non-commercial research, experimentation, and education. Please feel free to contribute or get in touch to request features or report any problems.

## Installation
The only available installation method is compiling from source. You will need to clone this repository first:
```
git clone --recursive https://github.com/AsriFox/gr-dvbs2acm.git
cd gr-dvbs2acm/
```

### From source

#### Install dependencies
Ubuntu:
```
apt install cmake pkg-config doxygen gnuradio-dev \
    graphviz libsndfile1-dev libspdlog-dev \
    pybind11-dev python3-packaging
```

Fedora:
```
dnf install cmake doxygen gnuradio-devel \
    fftw-devel gmp-devel graphviz \
    libsndfile-devel spdlog-devel \
    pybind11-devel python3-packaging \
```

#### Build and install
Build:
```
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$(gnuradio-config-info --prefix) ..
cmake --build . -j$(nproc)
```

Install:
```
sudo make install
sudo ldconfig
```

Or use the convenience script `install.sh`.

### Docker Image
It's possible to use Igor Freire's Ubuntu-based Docker image ([link](https://hub.docker.com/r/igorfreire/gnuradio-oot-dev)) for GNU Radio OOT development. A Dockerfile is available with a recipe to build **gr-dvbs2acm** blocks in the container. Run:
```
docker build -t gr-dvbs2acm .
```

## Authors
- [Ron Economos](https://github.com/drmpeg)
- [Ahmet Inan](https://github.com/xdsopl)
- [Igor Freire](https://github.com/igorauad)
- [AsriFox](https://github.com/AsriFox)
