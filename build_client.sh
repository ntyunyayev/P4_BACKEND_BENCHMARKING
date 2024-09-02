rsync -a /root/Workspace/P4_BACKEND_BENCHMARKING root@130.104.229.11:/root/Workspace/P4_BACKEND_BENCHMARKING
meson build --wipe && ninja -C build
