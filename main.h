// main.h

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

#define RING_SIZE 1024
#define MAX_BULK_SIZE 64
#define NB_ITERATION 100000000
#define NB_OVERFEED 50

extern char * ring_name;
extern const int ring_size;
extern const int max_bulk_size;
extern struct rte_ring * ring;
extern uint64_t nb_iteration;
extern uint64_t nb_overfeed; // over-feeded number by conumer to avoid starve

extern uint64_t enqueue_tries;

void driver_sp_sc();
void driver_sp_sc_bulk();
void driver_mp_mc();

int app_sp_thread(__attribute__((unused))void *dummy);
int app_sc_thread(__attribute__((unused))void *dummy);
int app_mp_thread(__attribute__((unused))void *dummy);
int app_mc_thread(__attribute__((unused))void *dummy);
int app_sp_bulk_thread(__attribute__((unused))void *dummy);
int app_sc_bulk_thread(__attribute__((unused))void *dummy);

#endif
