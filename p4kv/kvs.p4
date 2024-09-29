#include <core.p4>
#include <dpdk/pna.p4>


#define inbound(meta) (istd.direction == PNA_Direction_t.NET_TO_HOST)
#define outbound(meta) (istd.direction == PNA_Direction_t.HOST_TO_NET)

#define IP_PROTOCOL_UDP 0x11 
#define PROTOCOL_PORT 1 << 15

typedef bit<48> ethernet_addr_t;
typedef bit<32> ip_addr_t;
typedef bit<16> udp_port_t;

//
// Packet headers.
//
header ethernet_t {
	ethernet_addr_t dst_addr;
	ethernet_addr_t src_addr;
	bit<16> ether_type;
}

#define ETHERNET_ETHERTYPE_IPV4 0x0800

header ipv4_t {
	bit<4>  version;
	bit<4>  ihl;
	bit<6>  dscp;
	bit<2>  ecn;
	bit<16> total_length;
	bit<16> identification;
	bit<3>  flags;
	bit<13> fragment_offset;
	bit<8>  ttl;
	bit<8>  protocol;
	bit<16> header_checksum;
	ip_addr_t src_addr;
	ip_addr_t dst_addr;
}
header udp_t {
    udp_port_t src_port;
    udp_port_t dst_port;
    bit<16> length_;
    bit<16> checksum;
}
header key_t {
    bit<64> key;
    bit<64> value;
}

struct headers_t {
	ethernet_t ethernet;
	ipv4_t ipv4;
    udp_t udp;
	key_t key;
}

//
// Packet metadata.
//
struct metadata_t {
}

//
// Parser.
//
parser MainParserImpl(
	packet_in pkt,
	out   headers_t hdrs,
	inout metadata_t meta,
	in    pna_main_parser_input_metadata_t istd)
{
	state start {
		transition parse_ethernet;
	}

	state parse_ethernet {
		pkt.extract(hdrs.ethernet);
		transition select(hdrs.ethernet.ether_type) {
			ETHERNET_ETHERTYPE_IPV4 : parse_ipv4;
			default : accept;
		}
	}

	state parse_ipv4 {
		pkt.extract(hdrs.ipv4);
		transition select(hdrs.ipv4.protocol) {
			IP_PROTOCOL_UDP : parse_udp;
			default : accept;
		}
	}

	state parse_udp {
		pkt.extract(hdrs.udp);
		transition select(hdrs.udp.dst_port) {
			PROTOCOL_PORT : parse_key;
			default : accept;
		}
	}
	state parse_key {
		pkt.extract(hdrs.key);
		transition accept;
	}
}

control PreControlImpl(
    in    headers_t  hdr,
    inout metadata_t meta,
    in    pna_pre_input_metadata_t  istd,
    inout pna_pre_output_metadata_t ostd)
{
    apply {
    }
}

//
// Control block.
//
control MainControlImpl(
	inout headers_t hdrs,
	inout metadata_t meta,
	in    pna_main_input_metadata_t  istd,
	inout pna_main_output_metadata_t ostd)
{

    Register<bit<64>, bit<64>>(1024) kv_store;

	action return_to_sender() {
		hdrs.ethernet.setValid();

        ethernet_addr_t tmp_mac;
        tmp_mac = hdrs.ethernet.dst_addr;
        hdrs.ethernet.dst_addr = hdrs.ethernet.src_addr;
        hdrs.ethernet.src_addr = tmp_mac;

		ip_addr_t tmp_ip;
        tmp_ip = hdrs.ipv4.dst_addr;
        hdrs.ipv4.dst_addr = hdrs.ipv4.src_addr;
        hdrs.ipv4.src_addr = tmp_ip;

        udp_port_t tmp_port;
        tmp_port = hdrs.udp.dst_port;
        hdrs.udp.dst_port = hdrs.udp.src_port;
        hdrs.udp.src_port = tmp_port;

		send_to_port((PortId_t)0);
	}

	apply {
		if (inbound(meta) && hdrs.ipv4.isValid() && hdrs.key.isValid()) {
			if (hdrs.key.value == 0) {
                kv_store.read(hdrs.key.key);
                return_to_sender();
            } else {
                kv_store.write(hdrs.key.key, hdrs.key.value);
                drop_packet();
            }
		} else {
			drop_packet();
		}
	}
}

//
// Deparser.
//
control MainDeparserImpl(
	packet_out pkt,
	in    headers_t hdrs,
	in    metadata_t meta,
	in    pna_main_output_metadata_t ostd)
{
	apply {
		pkt.emit(hdrs.ethernet);
		pkt.emit(hdrs.ipv4);
		pkt.emit(hdrs.udp);
		pkt.emit(hdrs.key);
	}
}

//
// Package.
//
PNA_NIC(MainParserImpl(), PreControlImpl(), MainControlImpl(), MainDeparserImpl()) main;
