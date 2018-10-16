// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int interface = 0;
    int target_cluster = 128;
    int target_tag = 7;
    int id = __k1_get_cluster_id();
    // int size = id == 1 ? 4 : 7;
    // int offset = id == 1 ? 0 : 4;
    // char buffer[size];
    
    int tag = dnoc_tx_alloc_auto(interface);
    printf("C#: Alloc and config portal: %u\n", tag);


    if (id == 1)
    {
        char buffer[4];
        sprintf(buffer, "C1& ");
        printf("Send data: %s\n", buffer);

        dnoc_tx_config(interface, tag, id, target_tag, target_cluster);
        dnoc_tx_write(interface, tag, buffer, 4, 0);
    }
    else
    {
        char buffer[7];
        sprintf(buffer, " Clus2");
        printf("Send data: %s\n", buffer);

        dnoc_tx_config(interface, tag, id, target_tag, target_cluster);
        dnoc_tx_write(interface, tag, buffer, 7, 4);
    }

    dnoc_rx_free(interface, tag);

    printf("Goodbye\n");

	return 0;
}
