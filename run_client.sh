sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ~/Workspace/Pktgen-DPDK/build_$(hostname)/app/pktgen -l 0-1 -a 0000:00:04.0 -- -m [1].0 -P -f pktgen_script.lua
