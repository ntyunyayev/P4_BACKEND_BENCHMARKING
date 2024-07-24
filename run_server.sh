sudo LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./root/Workspace/dpdk-23.03/install_$(hostname)/bin/dpdk-testpmd -l 0-1  -a 0000:00:04.0  -- --portlist="0,0" --eth-peer=0,02:00:83:01:00:00
