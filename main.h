#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

#include <rte_cycles.h>

#define RING_SIZE 1024
#define MAX_BULK_SIZE 64
#define NB_ITERATION 100 * 1000 * 1000

#define LITERAL_BULK_SIZE 0

extern char *ring_name;
extern const int ring_size;
extern const int max_bulk_size;
extern struct rte_ring *ring;
extern uint64_t nb_iteration;

extern unsigned nb_producers;
extern unsigned bulk_size;
extern unsigned work_cycles;

void driver_sp_sc();
void driver_sp_sc_bulk();
void driver_mp_mc();

int app_sp_thread( __attribute__ ((unused))
                  void *dummy);
int app_sc_thread( __attribute__ ((unused))
                  void *dummy);
int app_sc_thread_linux_timer( __attribute__ ((unused))
                  void *dummy);
int app_mp_thread( __attribute__ ((unused))
                  void *dummy);
int app_mc_thread( __attribute__ ((unused))
                  void *dummy);


#endif
