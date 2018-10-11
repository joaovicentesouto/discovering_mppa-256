// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

int init_sync(int tag_rx, int source_cluster, int target_cluster);
void sync(int tag_tx, uint64_t mask);
void end_sync(int tag_tx);

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();

        printf("Start sync\n");

    int tag_tx = init_sync(16, id, 128);

    if (id == 1) //! cluster 1
    {
            printf("Signal 0x%u\n", MASK_0);
        sync(tag_tx, MASK_0);
    } 
    else //! cluster 2
    {
            printf("Signal 0x%u - 0x%u\n", MASK_1, MASK_2);
        sync(tag_tx, MASK_1);
        sync(tag_tx, MASK_2);
    }

    end_sync(tag_tx);

	    printf("Goodbye\n");

	return 0;
}

int init_sync(int tag_rx, int source_cluster, int target_cluster)
{
    unsigned tag_tx = 0;
    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = tag_rx;

    assert(mppa_noc_cnoc_tx_alloc_auto(0, &tag_tx, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    mppa_noc_cnoc_tx_configure(0, tag_tx, config, header);

    return tag_tx;
}

void sync(int tag_tx, uint64_t mask)
{
    mppa_noc_cnoc_tx_push_eot(0, tag_tx, mask);
}

void end_sync(int tag_tx)
{
    mppa_noc_cnoc_tx_free(0, tag_tx);
}