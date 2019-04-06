// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <pthread.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK 0x1

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = 0;
    int interface = 0;
    int tag = 16;

    cnoc_rx_alloc(interface, tag);
    cnoc_rx_config(interface, tag, MPPA_NOC_CNOC_RX_BARRIER, ~MASK);

    printf("CC: Alloc and config Sync\n");

    int tag_out = cnoc_tx_alloc_auto(interface);
    cnoc_tx_config(interface, tag_out, id, tag, id);

    printf("CC: Send signal 0x%x\n", MASK);
    cnoc_tx_write(interface, tag_out, MASK);

    printf("CC: Wait\n");

    cnoc_rx_wait(interface, tag);
    
    printf("CC: Done\n");

    cnoc_tx_free(interface, tag_out);
    cnoc_rx_free(interface, tag);

    printf("CC: Goodbye\n");

	return 0;
}
