// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x1
#define MSG 0xfaca

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Mailbox: 1 IO to 2 Clusters ======\n");
    
    int id_1 = 128;
    int id_2 = 129;
    int tag_in = 7;
    int interface_in = 0;
    int tag_out_1;
    int tag_out_2;
    int interface_out_1 = 0;
    int interface_out_2 = 1;
    int target_tag = 16;
    int target_cluster;

    printf("C#: Alloc and config Mailbox\n");

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    tag_out_1 = cnoc_tx_alloc_auto(interface_out_1);
    tag_out_2 = cnoc_tx_alloc_auto(interface_out_2);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface_in, tag_in);

    printf("Send mailbox: %jx\n", (uint64_t) MSG);

    target_cluster = 1;    
    cnoc_tx_config(interface_out_1, tag_out_1, id_1, target_tag, target_cluster);
    cnoc_tx_write(interface_out_1, tag_out_1, MSG);

    target_cluster = 2;    
    cnoc_tx_config(interface_out_2, tag_out_2, id_2, target_tag, target_cluster);
    cnoc_tx_write(interface_out_2, tag_out_2, MSG);

    printf("Done\n");

    cnoc_rx_free(interface_in, tag_in);
    cnoc_tx_free(interface_out_1, tag_out_1);
    cnoc_tx_free(interface_out_2, tag_out_2);

    join();

    printf("Goodbye\n");

	return 0;
};