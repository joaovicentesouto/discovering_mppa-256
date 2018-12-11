// #include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <utask.h>
#include <mppa_power.h>
#include <mppa_noc.h>
#include <mppa_routing.h>

#include <spawn.h>
#include <noc.h>

#define MASK ~0x1

// int global_buffer_size = 14;
// char global_buffer[14] = "HELLO";

// void noc_callback(
//     unsigned interface_id,
//     mppa_noc_interrupt_line_t line,
//     unsigned resource_id,
//     void *args)
// {
//     char * buffer = (char *) args;

//     memcpy(global_buffer, buffer, global_buffer_size);

//     printf("Handler:\nInterface: %u\nLine: %u\nResource: %u\nMessage: %s\n",
//         interface_id, line, resource_id, buffer);

//     global_buffer_size++;

//     // __builtin_k1_wpurge();
//     // __builtin_k1_fence();
//     __builtin_k1_dinval();
// }

int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
{
    printf("====== NoC Handlers: 1 IO to 1 Cluster ======\n");
    
    int interface_in = 0;
    int tag_in = 7;

    int buffer_size = 14;
    char buffer[buffer_size];

    // fprintf(buffer, "AAAAAAABBBBBB");
    
    printf("C#: Alloc and config Portals\n");

    dnoc_rx_alloc(interface_in, tag_in);
    dnoc_rx_config(interface_in, tag_in, buffer, buffer_size, 0);

    //! ======= Config Handler ========
    // mppa_noc_interrupt_line_enable(interface_in, MPPA_NOC_INTERRUPT_LINE_DNOC_RX);
    // mppa_noc_interrupt_line_enable(interface_in, MPPA_NOC_INTERRUPT_LINE_DNOC_RX);
    
    // mppa_noc_register_interrupt_handler(
    //     interface_in,
    //     MPPA_NOC_INTERRUPT_LINE_DNOC_RX,
    //     tag_in,
    //     noc_callback,
    //     (void *) buffer
    // );
    
    printf("Recive Msg\n");

    spawn();

    // while(global_buffer_size == 14) {
    //     __builtin_k1_dinval();
    // }

    sleep(1);
    
    unsigned a = dnoc_rx_wait(interface_in, tag_in, 0);
    
    printf("1 !!!!!!Msg: %s _ R: %u\n", buffer, a);

    a = dnoc_rx_wait(interface_in, tag_in, 0);
    
    printf("2 !!!!!!Msg: %s _ R: %u\n", buffer, a);

    // a = dnoc_rx_wait(interface_in, tag_in, 0);
    
    // printf("3 !!!!!!Msg: %s _ R: %u\n", buffer, a);

    // a = dnoc_rx_wait(interface_in, tag_in, 0);
    
    // printf("4 !!!!!!Msg: %s _ R: %u\n", buffer, a);

    dnoc_rx_free(interface_in, tag_in);


    printf("Done\n");

    join();

    printf("Goodbye\n");

	return 0;
};

// int main(__attribute__((unused)) int argc,__attribute__((unused)) const char **argv)
// {
//     printf("G BUF: %s\n", global_buffer);

//     utask_t t;
// 	utask_create(&t, NULL, (void*)main2, NULL);

//     utask_join(t, NULL);

//     printf("G BUF: %s\n", global_buffer);
// };