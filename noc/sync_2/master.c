
// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x3

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC: Sync 2 ======\n");

    int interface_in = 0;
    int interface_out = 0;
    int tag_in = 0;
    int id = __k1_get_cluster_id();
    int target_cluster;
    int target_tag = 16;
    
    printf("Alloc and config Syncs\n");

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    int tag_out = cnoc_tx_alloc_auto(interface_out);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface_in, tag_in);
    
    printf("Send signal\n");

    target_cluster = 1;
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface_out, tag_out, ~0);
    
    target_cluster = 2;
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface_out, tag_out, ~0);

    cnoc_rx_free(interface_in, tag_in);
    cnoc_tx_free(interface_out, tag_out);

    printf("Join\n");

    join();

    printf("Goodbye\n");

	return 0;
};