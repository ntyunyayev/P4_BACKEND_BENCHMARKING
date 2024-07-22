sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ~/Workspace/Pktgen-DPDK/build_$(hostname)/app/pktgen -l 2-3 -a 0000:00:08.0 --no-huge -m 2048 --file-prefix=server -- -m 1.0 -P 
