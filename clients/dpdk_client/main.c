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

const uint16_t port = 0;

int construct_packet(struct rte_mbuf *pkt, int key){

}

static int job(void *arg) {
  struct rte_mbuf *pkt;
  struct port_init_ctx *ctx = (struct port_init_ctx *)arg;
  uint16_t key;
  uint16_t nb_tx;
  struct rte_mempool *pool = ctx->mpool;
  while (1) {
    printf("Enter key to send : ");
    scanf("%d", &key);
    pkt = rte_pktmbuf_alloc(pool);
    if (pkt == NULL) {
      printf("Failed to allocate huge mbuf\n");
      return -1;
    }
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
  port = atoi(argv[1]);
  printf("Choosing port %d\n", port);
  /* >8 End of initialization the Environment Abstraction Layer (EAL). */
  /* Allocates mempool to hold the mbufs. 8< */

  const uint16_t NB_QUEUES = 1;
  struct port_init_ctx *ctx =
      gu_port_init(port, NB_QUEUES, NB_QUEUES, GU_DEFAULT_RING_SIZE,
                   GU_DEFAULT_RING_SIZE, GU_DEFAULT_MTU);
  if (ctx == NULL) {
    rte_exit(EXIT_FAILURE, "Cannot init port %" PRIu16 "\n", port);
  }

  RTE_LCORE_FOREACH_WORKER(lcore_id) {
    rte_eal_remote_launch(job, (void *)ctx, lcore_id);
  }

  rte_eal_mp_wait_lcore();

  /* clean up the EAL */
  rte_eal_cleanup();

  return 0;
}
