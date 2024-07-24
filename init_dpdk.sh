echo 2048 | sudo tee /proc/sys/vm/nr_hugepages
modprobe vfio-pci
echo 1 > /sys/module/vfio/parameters/enable_unsafe_noiommu_mode
sudo python3 ~/Workspace/dpdk-23.03/install_$(hostname)/bin/dpdk-devbind.py -b vfio-pci 0000:00:04.0
sudo python3 ~/Workspace/dpdk-23.03/install_$(hostname)/bin/dpdk-devbind.py -b vfio-pci 0000:00:08.0
