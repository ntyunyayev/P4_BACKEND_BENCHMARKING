#
# Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES, ALL RIGHTS RESERVED.
#
# This software product is a proprietary product of NVIDIA CORPORATION &
# AFFILIATES (the "Company") and all right, title, and interest in and to the
# software product, including all associated intellectual property rights, are
# and shall remain exclusively with the Company.
#
# This software product is governed by the End User License Agreement
# provided with the software product.
#

project(
  'DPDK_PORT_FORWARD',
  'C',
  'CPP',
  license: 'Proprietary',
  default_options: ['buildtype=release'],
)

# Comment this line to restore warnings of experimental DOCA features
add_project_arguments(
  [
    '-ggdb',
    '-g',
  ],
  language: ['c', 'cpp'],
)

dependencies = []

# DPDK dependency
dependencies += dependency('libdpdk')

srcs = ['clients/dpdk_client/main.c', 'dpdk_utils/gu.c']
inc_dirs = include_directories('dpdk_utils/')
executable(
  'dpdk_client',
  srcs,
  dependencies: dependencies,
  include_directories: inc_dirs,
  install: false,
)
