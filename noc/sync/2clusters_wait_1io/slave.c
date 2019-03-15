// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <unistd.h>

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

    cnoc_rx_alloc(interface, tag_in+0);
    cnoc_rx_alloc(interface, tag_in+1);
    cnoc_rx_alloc(interface, tag_in+2);
    cnoc_rx_alloc(interface, tag_in+3);
    cnoc_rx_alloc(interface, tag_in+4);
    cnoc_rx_config(interface, tag_in+0, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag_in+1, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag_in+2, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag_in+3, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    cnoc_rx_config(interface, tag_in+4, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    int tag_out = cnoc_tx_alloc_auto(interface);

    printf("Send signal %x\n", id == 1 ? 0x1 : 0x2);

    cnoc_tx_config(interface, tag_out, id, target_tag, target_cluster);
    cnoc_tx_write(interface, tag_out, id == 1 ? 0x1 : 0x2);
    cnoc_tx_free(interface, tag_out);

    // uint64_t ret = cnoc_rx_read(interface, tag_in);
    // printf("Ret %jx\n", ret);

    // sleep(5);
    printf("Wait 0\n");
    cnoc_rx_wait(interface, tag_in+0);
    printf("Wait 1\n");
    cnoc_rx_wait(interface, tag_in+1);
    printf("Wait 2\n");
    cnoc_rx_wait(interface, tag_in+2);
    printf("Wait 3\n");
    cnoc_rx_wait(interface, tag_in+3);
    printf("Wait 4\n");
    cnoc_rx_wait(interface, tag_in+4);
    printf("Finish\n");

    cnoc_rx_free(interface, tag_in+0);
    cnoc_rx_free(interface, tag_in+1);
    cnoc_rx_free(interface, tag_in+2);
    cnoc_rx_free(interface, tag_in+3);
    cnoc_rx_free(interface, tag_in+4);

	printf("Goodbye\n");

	return 0;
}