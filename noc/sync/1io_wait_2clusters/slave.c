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

static pthread_mutex_t lock0 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    int interface = 0;
    int target_cluster = 128;
    int target_tag = 16;
    int id = __k1_get_cluster_id();

    printf("C#: Alloc and config Sync\n");

    pthread_mutex_lock(&lock0);

    printf("%d, ** LOCK 0 ** \n", id);
    
    pthread_mutex_lock(&lock1);
    
    printf("%d, ** LOCK 1 ** \n", id);

    pthread_mutex_lock(&lock2);

    printf("%d, ** LOCK 2 ** \n", id);

    pthread_mutex_lock(&lock3);
    
    printf("%d, ** LOCK 3 ** \n", id);

    int tag = cnoc_tx_alloc_auto(interface);
    cnoc_tx_config(interface, tag, id, target_tag, target_cluster);

    if (id == 1) //! cluster 1
    {
        printf("Send signal 0x%x\n", MASK_0);
        cnoc_tx_write(interface, tag, MASK_0);
    } 
    else //! cluster 2
    {
        printf("Send signal 0x%x - 0x%x\n", MASK_1, MASK_2);
        cnoc_tx_write(interface, tag, MASK_1);
        cnoc_tx_write(interface, tag, MASK_2);
    }

    cnoc_tx_free(interface, tag);

    printf("Goodbye\n");

	return 0;
}
