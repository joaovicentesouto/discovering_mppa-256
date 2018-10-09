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

int init_sync(int rx_id, int source_cluster, int target_cluster)
{
    unsigned tx_id = 0;
    mppa_cnoc_config_t config = { 0 };
    mppa_cnoc_header_t header = { 0 };

    mppa_routing_get_cnoc_unicast_route(source_cluster, target_cluster, &config, &header);
    header._.tag = rx_id;

    assert(mppa_noc_cnoc_tx_alloc_auto(0, &tx_id, MPPA_NOC_BLOCKING) == MPPA_NOC_RET_SUCCESS);
    mppa_noc_cnoc_tx_configure(0, tx_id, config, header);

    return tx_id;
}

void sync(int tx_id, uint64_t mask)
{
    mppa_noc_cnoc_tx_push_eot(0, tx_id, mask);
}

void end_sync(int tx_id)
{
    mppa_noc_cnoc_tx_free(0, tx_id);
}

int main(__attribute__((unused)) int argc, const char **argv)
{
    // int id = __k1_get_cluster_id();
    int id = atoi(argv[0]);

    printf("[IODDR0] Cluster %d: Start sync\n", id);

    int tx_id = init_sync(16, id, 128);

    if (id == 1) //! cluster 1
    {
        printf("[IODDR0] Cluster %d: Wait %d\n", id, MASK_0);
        sync(tx_id, MASK_0);
    } 
    else //! cluster 2
    {
        printf("[IODDR0] Cluster %d: Wait %d - %d\n", id, MASK_1, MASK_2);
        sync(tx_id, MASK_1);
        sync(tx_id, MASK_2);
    }
    
    printf("[IODDR0] Cluster %d: Sync\n", id);

    end_sync(tx_id);

    printf("[IODDR0] Cluster %d: End Sync\n", id);
	printf("[IODDR0] Cluster %d: Goodbye\n", id);

	return 0;
}
