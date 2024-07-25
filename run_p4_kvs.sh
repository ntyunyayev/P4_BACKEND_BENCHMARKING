#!/usr/bin/env bash

EXAMPLE_DPDK_PATH="/root/Workspace/dpdk-23.03/build_$(hostname)/examples"
sudo ${EXAMPLE_DPDK_PATH}/dpdk-pipeline -l 0-1 -- -s kvs.cli
