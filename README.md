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

Install dependencies on Ubuntu:
```
apt install cmake pkg-config doxygen gnuradio-dev \
    graphviz libsndfile1-dev libspdlog-dev \
    pybind11-dev python3-packaging
```

Install dependencies on Fedora:
```
dnf install cmake doxygen gnuradio-devel \
    fftw-devel gmp-devel graphviz \
    libsndfile-devel spdlog-devel \
    pybind11-devel python3-packaging \
```

Build:
```
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$(gnuradio-config-info --prefix) ..
cmake --build . -j$(nproc)
```
Or use the convenience script `build.sh`.

Install:
```
sudo make install
sudo ldconfig
```

### Docker Image
It's possible to use Igor Freire's Ubuntu-based Docker image ([link](https://hub.docker.com/r/igorfreire/gnuradio-oot-dev)) for GNU Radio OOT development. A Dockerfile is available with a recipe to build **gr-dvbs2acm** blocks in the container.

First, [install Docker](https://docs.docker.com/engine/install). After that, [install docker-compose](https://docs.docker.com/compose/install/) (e.g. [on Ubuntu](https://docs.docker.com/compose/install/linux/#install-using-the-repository): `apt install docker-compose`).

Build the image:
```
docker compose build
```

Run the image:
```
docker compose run app gnuradio-companion
```
This will launch an instance of *gnuradio-companion* inside the container with **gr-dvbs2acm** blocks ready to use. You can navigate to `/src/gr-dvbs2acm/examples` to use the provided example flowcharts.

## Authors
- [Ron Economos](https://github.com/drmpeg)
- [Ahmet Inan](https://github.com/xdsopl)
- [Igor Freire](https://github.com/igorauad)
- [AsriFox](https://github.com/AsriFox)
