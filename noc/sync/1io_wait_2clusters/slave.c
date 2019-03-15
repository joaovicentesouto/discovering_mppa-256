// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <pthread.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <noc.h>

#define MASK_0 0xF
#define MASK_1 0x10
#define MASK_2 0x20

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int tag = 0;
    int interface = 0;
    int target_cluster = 128;
    int target_tag = 16;
    int id = __k1_get_cluster_id();

    printf("C#: Alloc and config Sync\n");

    cnoc_tx_alloc(interface, tag+0);
    cnoc_tx_alloc(interface, tag+1);
    cnoc_tx_alloc(interface, tag+2);
    cnoc_tx_alloc(interface, tag+3);
    cnoc_tx_alloc(interface, tag+4);

    cnoc_tx_config(interface, tag+0, id, target_tag+0, target_cluster);
    cnoc_tx_config(interface, tag+1, id, target_tag+1, target_cluster);
    cnoc_tx_config(interface, tag+2, id, target_tag+2, target_cluster);
    cnoc_tx_config(interface, tag+3, id, target_tag+3, target_cluster);
    cnoc_tx_config(interface, tag+4, id, target_tag+4, target_cluster);

    if (id == 1) //! cluster 1
    {
        printf("Send signal 0x%x\n", MASK_0);
        cnoc_tx_write(interface, tag+0, MASK_0);
        cnoc_tx_write(interface, tag+1, MASK_0);
        cnoc_tx_write(interface, tag+2, MASK_0);
        cnoc_tx_write(interface, tag+3, MASK_0);
        cnoc_tx_write(interface, tag+4, MASK_0);
    }
    else //! cluster 2
    {
        printf("Send signal 0x%x - 0x%x\n", MASK_1, MASK_2);
        cnoc_tx_write(interface, tag+0, MASK_1);
        cnoc_tx_write(interface, tag+0, MASK_2);
        cnoc_tx_write(interface, tag+1, MASK_1);
        cnoc_tx_write(interface, tag+1, MASK_2);
        cnoc_tx_write(interface, tag+2, MASK_1);
        cnoc_tx_write(interface, tag+2, MASK_2);
        cnoc_tx_write(interface, tag+3, MASK_1);
        cnoc_tx_write(interface, tag+3, MASK_2);
        cnoc_tx_write(interface, tag+4, MASK_1);
        cnoc_tx_write(interface, tag+4, MASK_2);
    }


    cnoc_tx_free(interface, tag+0);
    cnoc_tx_free(interface, tag+1);
    cnoc_tx_free(interface, tag+2);
    cnoc_tx_free(interface, tag+3);
    cnoc_tx_free(interface, tag+4);

    printf("Goodbye\n");

	return 0;
}
