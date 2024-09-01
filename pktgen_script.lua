package.path = package.path .. ";/root/Workspace/Pktgen-DPDK/?.lua"
require("Pktgen")

-- Not screen

-- Set random seed
math.randomseed(os.time())

local srcmac = "01:00:83:01:00:00"
local dstmac = "01:00:83:01:00:01"
local pkt_size = 128
-- Clear everything

-- =================== Generic Info ===================


pktgen.set_mac("0","src", srcmac);
pktgen.set_mac("0", "dst",dstmac);
pktgen.set_proto("0", "udp");
pktgen.set("0", "size",pkt_size);

pktgen.start("0")
