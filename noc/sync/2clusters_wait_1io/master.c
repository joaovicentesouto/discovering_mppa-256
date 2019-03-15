// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x3

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Sync: 2 Clusters wait 1 IO ======\n");

    int tag_in = 16;
    int interface_in = 0;
    int tag_out_1 = 0;
    int tag_out_2 = 0;
    int interface_out_1 = 0;
    int interface_out_2 = 1;
    int id = 128;
    int target_cluster;
    int target_tag = 24;
    
    printf("Alloc and config Syncs: mask %04x\n", MASK);

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface_in, tag_in);
    cnoc_rx_free(interface_in, tag_in);

    printf("Alloc 1\n");
    cnoc_tx_alloc(interface_out_1, tag_out_1+0);
    cnoc_tx_alloc(interface_out_1, tag_out_1+1);
    cnoc_tx_alloc(interface_out_1, tag_out_1+2);
    cnoc_tx_alloc(interface_out_1, tag_out_1+3);
    cnoc_tx_alloc(interface_out_1, tag_out_1+4);

    printf("Alloc 2\n");
    cnoc_tx_alloc(interface_out_2, tag_out_2+0);
    cnoc_tx_alloc(interface_out_2, tag_out_2+1);
    cnoc_tx_alloc(interface_out_2, tag_out_2+2);
    cnoc_tx_alloc(interface_out_2, tag_out_2+3); 
    cnoc_tx_alloc(interface_out_2, tag_out_2+4);    

    target_cluster = 1;
    cnoc_tx_config(interface_out_1, tag_out_1+0, 128, target_tag+0, target_cluster);
    cnoc_tx_config(interface_out_1, tag_out_1+1, 128, target_tag+1, target_cluster);
    cnoc_tx_config(interface_out_1, tag_out_1+2, 128, target_tag+2, target_cluster);
    cnoc_tx_config(interface_out_1, tag_out_1+3, 128, target_tag+3, target_cluster);
    cnoc_tx_config(interface_out_1, tag_out_1+4, 128, target_tag+4, target_cluster);

    printf("Send signal 0\n");
    cnoc_tx_write(interface_out_1, tag_out_1+0, 0x1);
    printf("Send signal 1\n");
    cnoc_tx_write(interface_out_1, tag_out_1+1, 0x1);
    printf("Send signal 2\n");
    cnoc_tx_write(interface_out_1, tag_out_1+2, 0x1);
    printf("Send signal 3\n");
    cnoc_tx_write(interface_out_1, tag_out_1+3, 0x1);
    printf("Send signal 4\n");
    cnoc_tx_write(interface_out_1, tag_out_1+4, 0x1);

    target_cluster = 2;
    cnoc_tx_config(interface_out_2, tag_out_2+0, 129, target_tag+0, target_cluster);
    cnoc_tx_config(interface_out_2, tag_out_2+1, 129, target_tag+1, target_cluster);
    cnoc_tx_config(interface_out_2, tag_out_2+2, 129, target_tag+2, target_cluster);
    cnoc_tx_config(interface_out_2, tag_out_2+3, 129, target_tag+3, target_cluster);
    cnoc_tx_config(interface_out_2, tag_out_2+4, 129, target_tag+4, target_cluster);

    printf("-Send signal 0\n");
    cnoc_tx_write(interface_out_2, tag_out_2+0, 0x1);
    printf("-Send signal 1\n");
    cnoc_tx_write(interface_out_2, tag_out_2+1, 0x1);
    printf("-Send signal 2\n");
    cnoc_tx_write(interface_out_2, tag_out_2+2, 0x1);
    printf("-Send signal 3\n");
    cnoc_tx_write(interface_out_2, tag_out_2+3, 0x1);
    printf("-Send signal 4\n");
    cnoc_tx_write(interface_out_2, tag_out_2+4, 0x1);

    printf("Join\n");

    join();

    cnoc_tx_free(interface_out_1, tag_out_1+0);
    cnoc_tx_free(interface_out_1, tag_out_1+1);
    cnoc_tx_free(interface_out_1, tag_out_1+2);
    cnoc_tx_free(interface_out_1, tag_out_1+3);
    cnoc_tx_free(interface_out_1, tag_out_1+4);
    cnoc_tx_free(interface_out_2, tag_out_2+0);
    cnoc_tx_free(interface_out_2, tag_out_2+1);
    cnoc_tx_free(interface_out_2, tag_out_2+2);
    cnoc_tx_free(interface_out_2, tag_out_2+3);
    cnoc_tx_free(interface_out_2, tag_out_2+4);


    printf("Goodbye\n");

	return 0;
};