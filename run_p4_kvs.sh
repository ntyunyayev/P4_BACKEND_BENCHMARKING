#!/usr/bin/env bash
export RTE_INSTALL_DIR="/root/Workspace/dpdk-23.03"
EXAMPLE_DPDK_PATH="/root/Workspace/dpdk-23.03/build_$(hostname)/examples"
${EXAMPLE_DPDK_PATH}/dpdk-pipeline -l 0-1 -- -s kvs.cli
