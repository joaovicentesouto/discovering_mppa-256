// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK 0xfaca

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int id = __k1_get_cluster_id();
    int interface = 1;
    int tag;
    int target_tag = 16;
    int target_cluster = 128;

    printf("C#: Alloc and config Mailbox\n");

    // cnoc_tx_alloc(interface, tag);
    tag = cnoc_tx_alloc_auto(interface);
    cnoc_tx_config(interface, tag, id, target_tag, target_cluster);

    printf("Send mailbox: %jx to %d\n", (uint64_t) MASK, target_cluster);
    
    cnoc_tx_write(interface, tag, MASK);

    target_tag = 7;
    // target_cluster = 129;
    cnoc_tx_config(interface, tag, id, target_tag, target_cluster);

    printf("Send mailbox: %jx to %d\n", (uint64_t) MASK, target_cluster);
    
    cnoc_tx_write(interface, tag, MASK);

    cnoc_tx_free(interface, tag);

    printf("Goodbye\n");

	return 0;
}
