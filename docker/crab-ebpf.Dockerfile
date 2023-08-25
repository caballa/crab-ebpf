########################################################
# To build the image:
#
# docker build -t crab-ebpf -f crab-ebpf.Dockerfile .
#
# To load the image:
#
# docker run -v `pwd`:/host -it crab-ebpf
########################################################

ARG BASE_IMAGE=jammy-llvm14
FROM seahorn/buildpack-deps-seahorn:$BASE_IMAGE

# Download and install crab-ebpf
RUN cd / && rm -rf /crab-ebpf && \
    git clone --recurse-submodules https://github.com/caballa/crab-ebpf crab-ebpf
WORKDIR /crab-ebpf
RUN make crab_install
RUN make
ENV LD_LIBRARY_PATH "/crab-ebpf/external/crab/install/elina/lib"

# Run crab-ebpf tests

WORKDIR /crab-ebpf/scripts
RUN ./runtests.sh

