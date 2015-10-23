#include <locale.h>
#include <stdio.h>
#include <unistd.h>

#include <rte_eal.h>
#include <rte_debug.h>
#include <rte_ring.h>

#include "main.h"

struct rte_ring *ring;
char *ring_name = "ring-0";
const int ring_size = RING_SIZE;
const int max_bulk_size = MAX_BULK_SIZE;
uint64_t nb_iteration = NB_ITERATION;

unsigned nb_producers;
unsigned bulk_size = 1;
unsigned work_cycles = 0;

int main(int argc, char **argv)
{
    int c;
    int ret;
    int sp_sc;
    unsigned socket_io;

    /* initialize EAL first */
    ret = rte_eal_init(argc, argv);

    argc -= ret;
    argv += ret;

    sp_sc = 1;
    bulk_size = 1;
    while ((c = getopt(argc, argv, "sm:b:w:")) != -1) {
        switch (c) {
        case 's':
            sp_sc = 1;
            break;
        case 'm':
            sp_sc = 0;
            nb_producers = atoi(optarg);
            break;
        case 'b':
            bulk_size = atoi(optarg);
            break;
        case 'w':
            work_cycles = atoi(optarg);
            break;
        case '?':
            break;
        }
    }

    setlocale(LC_NUMERIC, "");

    socket_io = rte_lcore_to_socket_id(rte_get_master_lcore());

    ring = rte_ring_create(ring_name,
                           ring_size, socket_io, RING_F_SP_ENQ | RING_F_SC_DEQ);

    if (ring == NULL) {
        rte_panic("Cannot create ring");
    }

    if (sp_sc) {
        printf("[MASTER] Single Producer/Consumer\n");
        printf("[MASTER] Bulk size: %d\n", bulk_size);
        driver_sp_sc();
    } else {
        printf("[MASTER] Number of Producers/Consumers: %d\n", nb_producers);
        printf("[MASTER] Bulk size: %d\n", bulk_size);
        driver_mp_mc();
    }
    rte_eal_mp_wait_lcore();
}
