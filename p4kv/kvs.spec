

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

struct key_t {
	bit<64> key
	bit<64> value
}

header ethernet instanceof ethernet_t
header ipv4 instanceof ipv4_t
header key instanceof key_t

struct metadata_t {
	bit<32> pna_main_input_metadata_direction
	bit<32> pna_main_input_metadata_input_port
	bit<32> pna_main_output_metadata_output_port
	bit<48> MainControlT_tmp_mac
	bit<32> MainControlT_tmp_ip
}
metadata instanceof metadata_t

regarray kv_store_0 size 0x400 initval 0
regarray direction size 0x100 initval 0
apply {
	rx m.pna_main_input_metadata_input_port
	regrd m.pna_main_input_metadata_direction direction m.pna_main_input_metadata_input_port
	extract h.ethernet
	jmpeq MAINPARSERIMPL_PARSE_IPV4 h.ethernet.ether_type 0x800
	jmp MAINPARSERIMPL_ACCEPT
	MAINPARSERIMPL_PARSE_IPV4 :	extract h.ipv4
	jmpeq MAINPARSERIMPL_PARSE_KEY h.ipv4.protocol 0x18
	jmp MAINPARSERIMPL_ACCEPT
	MAINPARSERIMPL_PARSE_KEY :	extract h.key
	MAINPARSERIMPL_ACCEPT :	jmpneq LABEL_FALSE m.pna_main_input_metadata_direction 0x0
	jmpnv LABEL_FALSE h.ipv4
	jmpnv LABEL_FALSE h.key
	jmpneq LABEL_FALSE_0 h.key.value 0x0
	validate h.ethernet
	mov m.MainControlT_tmp_mac h.ethernet.dst_addr
	mov h.ethernet.dst_addr h.ethernet.src_addr
	mov h.ethernet.src_addr m.MainControlT_tmp_mac
	mov m.MainControlT_tmp_ip h.ipv4.dst_addr
	mov h.ipv4.dst_addr h.ipv4.src_addr
	mov h.ipv4.src_addr m.MainControlT_tmp_ip
	mov m.pna_main_output_metadata_output_port 0x0
	jmp LABEL_END
	LABEL_FALSE_0 :	regwr kv_store_0 h.key.key h.key.value
	drop
	jmp LABEL_END
	LABEL_FALSE :	drop
	LABEL_END :	emit h.ethernet
	emit h.ipv4
	emit h.key
	tx m.pna_main_output_metadata_output_port
}


