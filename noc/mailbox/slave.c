// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#define MASK 0xfaca

static int mailbox_tx;

void init(int rx_id, int source_cluster, int target_cluster)
{
    unsigned tx_id = 0;
    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = rx_id;

    assert(mppa_noc_cnoc_tx_alloc_auto(0, &tx_id, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    mppa_noc_cnoc_tx_configure(0, tx_id, config, header);

    mailbox_tx = tx_id;
}

void send_mailbox(uint64_t mask)
{
    mppa_noc_cnoc_tx_push_eot(0, mailbox_tx, mask);
}

void end()
{
    mppa_noc_cnoc_tx_free(0, mailbox_tx);
}

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();

    printf("[IODDR0] Cluster %d: Start sync\n", id);

    init(16, id, 128);

    printf("[IODDR0] Cluster %d: Send mailbox: %jx\n", id, (uint64_t) MASK);
    
    send_mailbox(MASK);
    
    printf("[IODDR0] Cluster %d: End Sync\n", id);

    end();

	printf("[IODDR0] Cluster %d: Goodbye\n", id);

	return 0;
}
