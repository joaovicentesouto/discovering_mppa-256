
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
    int tag_in = 16;
    int id = 128;
    int target_cluster;
    int target_tag = 16;
    
    printf("Alloc and config Syncs: mask %04x\n", MASK);

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface_in, tag_in);
    cnoc_rx_free(interface_in, tag_in);

    int tag_out = cnoc_tx_alloc_auto(interface_out);
    
    printf("Send signal\n");

    target_cluster = 1;
    assert(cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster) == MPPA_NOC_RET_SUCCESS);
    cnoc_tx_write(interface_out, tag_out, 0x1);

    
    target_cluster = 2;
    assert(cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster) == MPPA_NOC_RET_SUCCESS);
    cnoc_tx_write(interface_out, tag_out, 0x1);

    cnoc_tx_free(interface_out, tag_out);

    printf("Join\n");

    join();

    printf("Goodbye\n");

	return 0;
};