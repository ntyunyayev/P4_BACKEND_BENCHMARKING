#!/usr/bin/env bash

export RTE_INSTALL_DIR="/root/Workspace/dpdk-23.03"
sudo RTE_INSTALL_DIR=$RTE_INSTALL_DIR ${RTE_INSTALL_DIR}/build/examples/dpdk-pipeline -l 0-1 -a 0000:00:04.0 -- -s l2fwd.cli
