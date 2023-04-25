ARG cmake_args
FROM igorfreire/gnuradio-oot-dev:3.10.5.1-ubuntu-jammy
# Dependencies
RUN apt-get install -y libsndfile1-dev python3-packaging
# Build and install
ADD . /src/gr-dvbs2acm/
RUN cd /src/gr-dvbs2acm/ && \
    git clone https://github.com/google/cpu_features.git && \
    mkdir build && cd build && \
    cmake $cmake_args .. && \
    cmake --build . -j$(nproc) && \
    cmake --install . && \
    ldconfig && \
    cd ../../ && rm -r gr-dvbs2rx/