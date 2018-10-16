// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
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
    printf("====== NoC: Portal 2 ======\n");
    
    int id = 128;
    int interface_in = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster;
    
    char buff_1[4];
    char buff_2[7];

    printf("Alloc and config Portals\n");

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    int tag_out = dnoc_tx_alloc_auto(interface_out);

    spawn();

    printf("Wait signal\n");

    cnoc_rx_wait(interface_in, tag_in);
    
    sprintf(buff_1, "C_1\0");
    printf("Send to cluster 1: %s\n", buff_1);

    target_cluster = 1;
    dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    dnoc_tx_write(interface_out, tag_out, buff_1, 4, 0);

    sprintf(buff_2, "C____2\0");
    printf("Send to cluster 2: %s\n", buff_2);

    target_cluster = 2;
    dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    dnoc_tx_write(interface_out, tag_out, buff_2, 7, 0);

    cnoc_rx_free(interface_in, tag_in);
    dnoc_tx_free(interface_out, tag_out);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
