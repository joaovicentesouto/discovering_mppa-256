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

void init(int tag_rx, int source_cluster, int target_cluster);
void send_mailbox(uint64_t mask);
void end();

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();

        printf("Start sync\n");

    init(16, id, 128);

        printf("Send mailbox: %jx\n", (uint64_t) MASK);
    
    send_mailbox(MASK);
    end();

	    printf("Goodbye\n");

	return 0;
}

void init(int tag_rx, int source_cluster, int target_cluster)
{
    unsigned tag_tx = 0;
    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = tag_rx;

    assert(mppa_noc_cnoc_tx_alloc_auto(0, &tag_tx, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    mppa_noc_cnoc_tx_configure(0, tag_tx, config, header);

    mailbox_tx = tag_tx;
}

void send_mailbox(uint64_t mask)
{
    mppa_noc_cnoc_tx_push_eot(0, mailbox_tx, mask);
}

void end()
{
    mppa_noc_cnoc_tx_free(0, mailbox_tx);
}