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

#define MASK ~0x1

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("====== NoC Handlers: 1 IO to 1 Cluster ======\n");
    
    int id = 128;
    int interface = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster = 0;
    
    char buffer[] = "Dummy message\0";

    printf("Alloc and config Portals\n");

    cnoc_rx_alloc(interface, tag_in);
    cnoc_rx_config(interface, tag_in, MPPA_NOC_CNOC_RX_BARRIER, MASK);
    
    spawn();
    
    printf("Wait signal\n");

    cnoc_rx_wait(interface, tag_in);
    cnoc_rx_free(interface, tag_in);

    printf("Prepare to send\n");

    int tag_out = dnoc_tx_alloc_auto(interface_out);
    
    dnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);
    dnoc_tx_write(interface_out, tag_out, buffer, 14, 0);

    dnoc_tx_free(interface_out, tag_out);

    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};
