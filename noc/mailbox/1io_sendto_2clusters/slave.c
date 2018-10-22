// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK 0xfaca

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();
    int tag_in = 16;
    int interface_in = 0;
    int tag_out;
    int interface_out = 0;
    int target_tag = 7;
    int target_cluster = 128;

    printf("C#: Alloc and config Sync 9999\n");

    tag_out = cnoc_tx_alloc_auto(interface_out);
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);

    cnoc_rx_alloc(interface_in, tag_in);
    cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_MAILBOX, 0);

    printf("Send signal 0x%x\n", 1);
    cnoc_tx_write(interface_out, tag_out, 0x1);

    printf("Recive mailbox\n");

    cnoc_rx_wait(interface_in, tag_in);

    uint64_t mailbox = cnoc_rx_read(interface_in, tag_in);
    printf("Msg: %jx\n", mailbox);

    cnoc_rx_free(interface_in, tag_in);
    cnoc_tx_free(interface_out, tag_out);

    printf("Goodbye\n");

	return 0;
}