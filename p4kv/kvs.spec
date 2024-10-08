

struct ethernet_t {
	bit<48> dst_addr
	bit<48> src_addr
	bit<16> ether_type
}

struct ipv4_t {
	bit<8> version_ihl
	bit<8> dscp_ecn
	bit<16> total_length
	bit<16> identification
	bit<16> flags_fragment_offset
	bit<8> ttl
	bit<8> protocol
	bit<16> header_checksum
	bit<32> src_addr
	bit<32> dst_addr
}

struct udp_t {
	bit<16> src_port
	bit<16> dst_port
	bit<16> length_
	bit<16> checksum
}

struct key_t {
	bit<8> type
	bit<16> key
	bit<32> value
}

header ethernet instanceof ethernet_t
header ipv4 instanceof ipv4_t
header udp instanceof udp_t
header key instanceof key_t

struct metadata_t {
	bit<32> pna_main_input_metadata_direction
	bit<32> pna_main_input_metadata_input_port
	bit<32> pna_main_output_metadata_output_port
	bit<48> MainControlT_tmp_mac
	bit<32> MainControlT_tmp_ip
	bit<16> MainControlT_tmp_port
}
metadata instanceof metadata_t

regarray kv_store_0 size 0xFFFF initval 0x0
regarray direction size 0x100 initval 0
apply {
	rx m.pna_main_input_metadata_input_port
	regrd m.pna_main_input_metadata_direction direction m.pna_main_input_metadata_input_port
	extract h.ethernet
	jmpeq MAINPARSERIMPL_PARSE_IPV4 h.ethernet.ether_type 0x800
	jmp MAINPARSERIMPL_ACCEPT
	MAINPARSERIMPL_PARSE_IPV4 :	extract h.ipv4
	jmpeq MAINPARSERIMPL_PARSE_UDP h.ipv4.protocol 0x11
	jmp MAINPARSERIMPL_ACCEPT
	MAINPARSERIMPL_PARSE_UDP :	extract h.udp
	jmpeq MAINPARSERIMPL_PARSE_KEY h.udp.dst_port 0x8000
	jmp MAINPARSERIMPL_ACCEPT
	MAINPARSERIMPL_PARSE_KEY :	extract h.key
	MAINPARSERIMPL_ACCEPT :	jmpneq LABEL_FALSE m.pna_main_input_metadata_direction 0x0
	jmpnv LABEL_FALSE h.ipv4
	jmpnv LABEL_FALSE h.key
	jmpneq LABEL_FALSE h.key.type 0x0
	jmpneq LABEL_FALSE_0 h.key.value 0x0
	regrd h.key.value kv_store_0 h.key.key
	mov h.key.type 0x1
	validate h.ethernet
	mov m.MainControlT_tmp_mac h.ethernet.dst_addr
	mov h.ethernet.dst_addr h.ethernet.src_addr
	mov h.ethernet.src_addr m.MainControlT_tmp_mac
	mov m.MainControlT_tmp_ip h.ipv4.dst_addr
	mov h.ipv4.dst_addr h.ipv4.src_addr
	mov h.ipv4.src_addr m.MainControlT_tmp_ip
	mov m.MainControlT_tmp_port h.udp.dst_port
	mov h.udp.dst_port h.udp.src_port
	mov h.udp.src_port m.MainControlT_tmp_port
	mov m.pna_main_output_metadata_output_port 0x0
	jmp LABEL_END
	LABEL_FALSE_0 :	regwr kv_store_0 h.key.key h.key.value
	mov h.key.type 0x2
	validate h.ethernet
	mov m.MainControlT_tmp_mac h.ethernet.dst_addr
	mov h.ethernet.dst_addr h.ethernet.src_addr
	mov h.ethernet.src_addr m.MainControlT_tmp_mac
	mov m.MainControlT_tmp_ip h.ipv4.dst_addr
	mov h.ipv4.dst_addr h.ipv4.src_addr
	mov h.ipv4.src_addr m.MainControlT_tmp_ip
	mov m.MainControlT_tmp_port h.udp.dst_port
	mov h.udp.dst_port h.udp.src_port
	mov h.udp.src_port m.MainControlT_tmp_port
	mov m.pna_main_output_metadata_output_port 0x0
	jmp LABEL_END
	LABEL_FALSE :	drop
	LABEL_END :	emit h.ethernet
	emit h.ipv4
	emit h.udp
	emit h.key
	tx m.pna_main_output_metadata_output_port
}


