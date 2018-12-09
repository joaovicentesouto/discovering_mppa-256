#ifndef NOC_H
#define NOC_H

#ifndef _NOC_
    #error "Strange inclusion."
#endif

#include <stdint.h>
#include <mppa_noc.h>

//! Control NoC

extern void cnoc_rx_alloc(int, int);
extern void cnoc_rx_free(int, int);
extern void cnoc_tx_alloc(int, int);
extern void cnoc_tx_free(int, int);
extern unsigned cnoc_tx_alloc_auto(int);

extern void cnoc_rx_config(int, int, mppa_noc_cnoc_rx_mode_t, uint64_t);
extern void cnoc_tx_config(int, int, int, int, int);

extern void cnoc_rx_wait(int, int);
extern uint64_t cnoc_rx_read(int, int);
extern void cnoc_tx_write(int, int, uint64_t);

//! Data NoC

extern void dnoc_rx_alloc(int, int);
extern void dnoc_rx_free(int, int);
extern void dnoc_tx_alloc(int, int);
extern void dnoc_tx_free(int, int);
extern unsigned dnoc_tx_alloc_auto(int);

extern void dnoc_rx_config(int, int, char *, int, int);
extern void dnoc_rx_wait(int, int, int);

extern void dnoc_tx_config(int, int, int, int, int);
extern void dnoc_tx_write(int, int, char *, int, int);

//! UC NoC
extern void dnoc_tx_uc_config_and_write(int, int, int, int, int, int, void *);
extern void dnoc_uc_alloc(int, int);
extern int dnoc_uc_alloc_auto(int);
extern void dnoc_uc_free(int, int);

#endif /* SPAWN_H */
