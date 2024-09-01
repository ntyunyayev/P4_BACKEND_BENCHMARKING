#include <rte_cycles.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gu.h"

struct kvs_hdr {
    int64_t key;
    int64_t val;
};

const uint16_t port = 0;
const uint8_t kvs_protocol_id = 24;
const struct rte_ether_addr src_mac = {{0x02, 0x00, 0x83, 0x01, 0x00, 0x00}};
const struct rte_ether_addr dst_mac = {{0x02, 0x00, 0x83, 0x01, 0x00, 0x01}};

void construct_request(struct rte_mbuf* pkt, int64_t key, int64_t val) {
    // Ether hdr processing
    printf("======Constructing packet=========\n");
    struct rte_ether_hdr* eth_hdr = rte_pktmbuf_mtod(pkt, struct rte_ether_hdr*);
    eth_hdr->ether_type = rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4);
    rte_ether_addr_copy(&src_mac, &eth_hdr->src_addr);
    rte_ether_addr_copy(&dst_mac, &eth_hdr->dst_addr);
    // IP processing
    struct rte_ipv4_hdr* ipv4_hdr = (struct rte_ipv4_hdr*)(eth_hdr + 1);
    ipv4_hdr->next_proto_id = kvs_protocol_id;
    ipv4_hdr->time_to_live = 64;
    const char ip_src[128] = "192.168.0.1";
    const char ip_dst[128] = "192.168.0.2";

    if (inet_pton(AF_INET, ip_src, &ipv4_hdr->src_addr) != 1) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (inet_pton(AF_INET, ip_dst, &ipv4_hdr->dst_addr) != 1) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    // KVS protocol processing
    struct kvs_hdr* kvs_hdr = (struct kvs_hdr*)(ipv4_hdr + 1);
    kvs_hdr->key = key;
    kvs_hdr->val = val;
    int pkt_size = 128;
    printf("size : %d\n", pkt_size);
    pkt->data_len = pkt_size;
    pkt->pkt_len = pkt_size;
    gu_print_mac_addresses(pkt);
    printf("=================================\n");
}

static int job(void* arg) {
    struct rte_mbuf* pkt;
    struct port_init_ctx* ctx = (struct port_init_ctx*)arg;
    int key;
    int val;
    uint16_t nb_tx;
    struct rte_mempool* pool = ctx->mpool;
    while (1) {
        printf("Enter a key and a value : ");
        while (scanf("%d %d", &key, &val) != 2) {
            printf("Invalid format, expect : 'KEY VALUE\n'");
        };
        pkt = rte_pktmbuf_alloc(pool);
        if (pkt == NULL) {
            printf("Failed to allocate huge mbuf\n");
            return -1;
        }
        construct_request(pkt, key, val);
        nb_tx = rte_eth_tx_burst(port, 0, &pkt, 1);
        if (unlikely(nb_tx != 1)) {
            printf("failed to send packet\n");
            rte_pktmbuf_free(pkt);
        } else {
            printf("Pkt sent\n");
        }
    }
}

int receive_pkts(void) {
    struct rte_mbuf* bufs[32];
    uint16_t nb_rx;
    while (1) {
        nb_rx = rte_eth_rx_burst(port, 0, bufs, 32);
        if (nb_rx == 0) {
            continue;
        }
        for (int i = 0; i < nb_rx; i++) {
            if (bufs[i]->pkt_len > 64) {
                printf("pkt_len : %d\n", bufs[i]->pkt_len);
                printf("data_len : %d\n", bufs[i]->data_len);
            }
            rte_pktmbuf_free(bufs[i]);
        }
    }
}

int main(int argc, char* argv[]) {
    // args
    printf("Starting client\n");
    uint16_t lcore_id;
    int ret;
    /* Initializion the Environment Abstraction Layer (EAL). 8< */
    ret = rte_eal_init(argc, argv);
    if (ret < 0) rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    argc -= ret;
    argv += ret;
    printf("EAL: Initialization complete\n");
    /* >8 End of initialization the Environment Abstraction Layer (EAL). */
    /* Allocates mempool to hold the mbufs. 8< */
    const uint16_t NB_QUEUES = 1;
    struct port_init_ctx* ctx =
        gu_port_init(port, NB_QUEUES, NB_QUEUES, GU_DEFAULT_RING_SIZE, GU_DEFAULT_RING_SIZE, GU_DEFAULT_MTU);
    if (ctx == NULL) {
        rte_exit(EXIT_FAILURE, "Cannot init port %" PRIu16 "\n", port);
    }

    RTE_LCORE_FOREACH_WORKER(lcore_id) { rte_eal_remote_launch(job, (void*)ctx, lcore_id); }

    receive_pkts();
    rte_eal_mp_wait_lcore();

    /* clean up the EAL */
    rte_eal_cleanup();

    return 0;
}
