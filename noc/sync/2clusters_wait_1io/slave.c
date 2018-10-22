// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK ~0x1
int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int interface = 0;
    int tag_in = 24;
    int id = __k1_get_cluster_id();
    int target_cluster = 128;
    int target_tag = 16;
    
    printf("C#: Alloc and config Syncs: %d\n", tag_in);

    cnoc_rx_alloc(interface, tag_in);
    cnoc_rx_config(interface, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    int tag_out = cnoc_tx_alloc_auto(interface);

    printf("Send signal %x\n", id == 1 ? 0x1 : 0x2);

    cnoc_tx_config(interface, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface, tag_out, id == 1 ? 0x1 : 0x2);

    printf("Wait\n");

    uint64_t ret = cnoc_rx_read(interface, tag_in);
    printf("Ret %jx\n", ret);
    cnoc_rx_wait(interface, tag_in);

    cnoc_rx_free(interface, tag_in);
    cnoc_tx_free(interface, tag_out);

	printf("Goodbye\n");

	return 0;
}