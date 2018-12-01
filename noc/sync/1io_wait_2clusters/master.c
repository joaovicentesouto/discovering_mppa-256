
// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>

#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x3F

static pthread_mutex_t lock0 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;

int main(__attribute__((unused)) int argc, __attribute__((unused)) const char **argv)
{
    printf("====== NoC Sync: 1 IO wait 2 Clusters ======\n");
    
    int interface = 0;
    int tag = 16;

    pthread_mutex_lock(&lock0);

    printf(" ** LOCK 0 ** \n");
    
    pthread_mutex_lock(&lock1);
    
    printf(" ** LOCK 1 ** \n");

    pthread_mutex_lock(&lock2);

    printf(" ** LOCK 2 ** \n");

    pthread_mutex_lock(&lock3);
    
    printf(" ** LOCK 3 ** \n");

    cnoc_rx_alloc(interface, tag);
    cnoc_rx_config(interface, tag, MPPA_NOC_CNOC_RX_BARRIER, MASK);

    spawn();

    printf("Wait\n");

    cnoc_rx_wait(interface, tag);
    
    printf("Done\n");

    cnoc_rx_free(interface, tag);

    printf("Join\n");

    join();

    printf("Goodbye\n");

	return 0;
};
