// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x1
#define MSG 0xfaca

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC: Mailbox 2 ======\n");
    
    int id = 128;
    int interface_in = 0, interface_out = 0;
    int tag_in = 7, tag_out;
    int target_tag = 16;
    int target_cluster;

    printf("C#: Alloc and config Mailbox\n");

    assert(cnoc_rx_alloc(interface_in, tag_in) == 0);
    assert(cnoc_rx_config(interface_in, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK) == 0);

    tag_out = cnoc_tx_alloc_auto(interface_out);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface_in, tag_in);

    printf("Send mailbox: %jx\n", (uint64_t) MSG);

    target_cluster = 1;    
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface_out, tag_out, MSG);

    target_cluster = 2;    
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface_out, tag_out, MSG);

    printf("Done\n");

    cnoc_rx_free(interface_out, tag_in);
    cnoc_tx_free(interface_out, tag_in);

    join();

    printf("Goodbye\n");

	return 0;
};