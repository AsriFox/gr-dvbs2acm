ARG cmake_args
FROM igorfreire/gnuradio-oot-dev:3.10.5.1-ubuntu-jammy
# Dependencies
RUN apt-get install -y libsndfile1-dev python3-packaging
# Build and install
ADD . /src/gr-dvbs2acm/
RUN cd /src/gr-dvbs2acm/ && \
    rm -r build/* && \
    sh install.sh
CMD ["gnuradio-companion"]