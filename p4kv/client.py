import scapy.all as scapy
import argparse

class KVS(scapy.Packet):
    name = "key-value store"
    fields_desc=[
        scapy.NBytesField("key", 0, 64),
        scapy.NBytesField("value", 0, 64),
    ]


parser = argparse.ArgumentParser()
parser.add_argument("src_mac", help="MAC address of the source")
parser.add_argument("dst_mac", help="MAC address of the destination")
parser.add_argument("dst", help="IP address of the destination")
parser.add_argument("key", help="Key to store")
parser.add_argument("value", help="Value to store")
args = parser.parse_args()

mac_src = args.src_mac
mac_dst = args.dst_mac
ip_dst = args.dst
key = int(args.key)
value = int(args.value)

scapy.bind_layers(scapy.IP, KVS, proto=24)

# packet = scapy.Ether(src=mac_src, dst=mac_dst) / scapy.IP(dst=ip_dst) / KVS(key=key, value=value)
packet = scapy.IP(dst=ip_dst) / KVS(key=key, value=value)
print(packet.show())
scapy.send(packet)

# wait for response
response = scapy.sniff(count=1, filter=f"src {ip_dst} and proto 24", timeout=30)
print(response[0].show())
