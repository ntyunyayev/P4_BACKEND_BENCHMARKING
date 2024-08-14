#include "gu.h"
#include <rte_cycles.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 0;

static int job(void *arg) {
  struct port_init_ctx *ctx = (struct port_init_ctx *)arg;

  while (1) {
    printf("Enter key : ");
    scanf("%d", &pkt_size);
    pkt = rte_pktmbuf_alloc(ctx->mpool);
    if (pkt == NULL) {
      printf("Failed to allocate huge mbuf\n");
      return -1;
    }

    pkt->data_len = pkt_size;
    pkt->pkt_len = pkt_size;
    nb_tx = rte_eth_tx_burst(port, 0, &pkt, 1);
    if (unlikely(nb_tx != 1)) {
      printf("failed to send packet\n");
      rte_pktmbuf_free(pkt);
    } else {
      printf("pkt_sent => \n");
      printf("pkt_len : %d\n", pkt->pkt_len);
      printf("pkt_data : %d\n", pkt->data_len);
      printf("nb_segs : %d\n", pkt->nb_segs);
    }
  }
}

int main(int argc, char *argv[]) {
  // args
  uint16_t portid;
  uint16_t lcore_id;
  int ret;
  /* Initializion the Environment Abstraction Layer (EAL). 8< */
  ret = rte_eal_init(argc, argv);
  if (ret < 0)
    rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
  argc -= ret;
  argv += ret;

  struct port_init_ctx *ctx = gu_port_init(PORT, 1, 1, 1024, 1024, 1500);
  if (ctx == NULL) {
    printf("failed to init port\n");
    return -1;
  }

  rte_lcore_foreach_worker(lcore_id) {
    rte_eal_remote_launch(job, (void *)ctx, lcore_id);
  }
  rte_eal_mp_wait_lcore();

  /* clean up the EAL */
  rte_eal_cleanup();

  return 0;
}
