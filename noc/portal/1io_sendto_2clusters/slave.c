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
    int interface_in = 0;
    int interface_out = 0;
    int tag_in = 7;
    int target_tag = 7;
    int target_cluster = 128;

    int id = __k1_get_cluster_id();
    int offset = id == 1 ? 0 : 1;
    int buffer_size = id == 1 ? 4 : 7;
    char buffer[buffer_size];

    printf("C#: Alloc and config Portals\n");

    dnoc_rx_alloc(interface_in, tag_in);
    dnoc_rx_config(interface_in, tag_in, buffer, buffer_size, 0);

    int tag_out = cnoc_tx_alloc_auto(interface_out);
    cnoc_tx_config(interface_out, tag_out, id, target_tag, target_cluster);

    printf("Signal\n");

    cnoc_tx_write(interface_out, tag_out, 1 << offset);
    
    printf("Recive Msg\n");
    
    dnoc_rx_wait(interface_in, tag_in);
    
    printf("Msg: %s\n", buffer);

    cnoc_tx_free(interface_out, tag_out);
    dnoc_rx_free(interface_in, tag_in);

    printf("Goodbye\n");

	return 0;
}
