// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x3

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("====== NoC Portal: 1 IO to 2 Clusters ======\n");
    
    int id_1 = 128;
    int id_2 = 129;
    int interface_in = 0;
    int interface_out_1 = 0;
    int interface_out_2 = 1;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster;
    
    char buff_1[128];
    char buff_2[256];

    printf("Alloc and config Portals\n");

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    int tag_out = dnoc_tx_alloc_auto(interface_out_1);

    spawn();

    printf("Wait signal\n");

    cnoc_rx_wait(interface_in, tag_in);
    
    memset(buff_1, 1, 128);
    printf("Send to cluster 1: %d\n", sizeof(buff_1));

    target_cluster = 1;
    dnoc_tx_config(interface_out_1, tag_out, id_1, target_tag, target_cluster);
    dnoc_tx_write(interface_out_1, tag_out, buff_1, 128, 0);

    memset(buff_2, 1, 256);
    printf("Send to cluster 2: %d\n", sizeof(buff_2));

    target_cluster = 2;
    dnoc_tx_config(interface_out_1, tag_out, id_1, target_tag, target_cluster);
    dnoc_tx_write(interface_out_1, tag_out, buff_2, 256, 0);

    cnoc_rx_free(interface_in, tag_in);
    dnoc_tx_free(interface_out_1, tag_out);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
