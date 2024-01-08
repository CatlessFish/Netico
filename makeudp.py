from scapy.all import *
from scapy.layers.inet import IP, UDP

local_ip = "192.168.1.21"
local_port = 5555
pico_ip = "192.168.1.25"
pico_port = 4444

ip_pack = IP(src=local_ip, dst=pico_ip)
udp_pack = UDP(sport=local_port, dport=pico_port)
while True:
    data = input()
    send(ip_pack/udp_pack/data, verbose=0)
    print(f"Send {len(data)} Bytes")
    