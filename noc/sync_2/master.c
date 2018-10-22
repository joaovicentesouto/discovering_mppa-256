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
    int interface_out_1 = 0;
    int interface_out_2 = 1;
    int tag_in = 16;
    int id = 128;
    int target_cluster;
    int target_tag = 0;
    
    printf("Alloc and config Syncs: mask %04x\n", MASK);

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait 1\n");

    cnoc_rx_wait(interface_in, tag_in);

    printf("Wait 2\n");
    cnoc_rx_free(interface_in, tag_in);

    printf("Wait 3\n");
    int tag_out_1 = cnoc_tx_alloc_auto(interface_out_1);

    printf("Wait 4\n");
    int tag_out_2 = cnoc_tx_alloc_auto(interface_out_2);
    
    printf("Send signal\n");

    target_cluster = 1;
    assert(cnoc_tx_config(interface_out_1, tag_out_1, 128, target_tag, target_cluster) == MPPA_NOC_RET_SUCCESS);
    cnoc_tx_write(interface_out_1, tag_out_1, 0x1);

    
    target_cluster = 2;
    assert(cnoc_tx_config(interface_out_2, tag_out_2, 129, target_tag, target_cluster) == MPPA_NOC_RET_SUCCESS);
    cnoc_tx_write(interface_out_2, tag_out_2, 0x1);

    printf("Join\n");

    join();

    cnoc_tx_free(interface_out_1, tag_out_1);
    cnoc_tx_free(interface_out_2, tag_out_2);


    printf("Goodbye\n");

	return 0;
};