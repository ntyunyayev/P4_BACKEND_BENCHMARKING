#!/usr/bin/env bash
p4c-dpdk --arch pna p4kv/kvs.p4 -o p4kv/kvs.spec || exit 1
export RTE_INSTALL_DIR="/root/Workspace/dpdk-23.03"
EXAMPLE_DPDK_PATH="/root/Workspace/dpdk-23.03/build_$(hostname)/examples"
${EXAMPLE_DPDK_PATH}/dpdk-pipeline -l 0-1 -- -s kvs.cli
