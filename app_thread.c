#include <inttypes.h>
#include <stdio.h>

#include <rte_cycles.h>
#include <rte_ring.h>

#include "main.h"

int app_sp_thread(void *data)
{
    unsigned lcore;
    uint64_t j;
    void *dummy_obj;

    /* wrapper of parameter list */
#if LITERAL_BULK_SIZE
    unsigned n = 1;
#else
    unsigned n = bulk_size;
#endif
    struct rte_ring *r = ring;
    void **obj_table = &dummy_obj;
    uint32_t prod_head, cons_tail;
    uint32_t prod_next, free_entries;
    unsigned i;
    uint32_t mask = r->prod.mask;
    /* end of vars for rte_ring.h code */

    lcore = rte_lcore_id();
    printf("[Producer %d] Start\n", lcore);

    for (j = 0; j < nb_iteration; j++) {

        while (1) {

            /* begin of rte_ring.h code */
            prod_head = r->prod.head;
            cons_tail = r->cons.tail;
            free_entries = mask + cons_tail - prod_head;

            if (unlikely(n > free_entries)) {
                continue;
            }

            prod_next = prod_head + n;
            r->prod.head = prod_next;

            /* write entries in ring */
            ENQUEUE_PTRS();
            rte_compiler_barrier();

            r->prod.tail = prod_next;
            /* end of rte_ring.h code */

            break;
        }
    }

    printf("[Producer %d] Finish\n", lcore);
}

int app_sc_thread(void *data)
{
    void *dummy_obj;
    uint64_t j, start_cycle, end_cycle, hz;
    double time;

    /* wrapper of parameter list */
    struct rte_ring *r = ring;
    void **obj_table = &dummy_obj;
#if LITERAL_BULK_SIZE
    unsigned n = 1;
#else
    unsigned n = bulk_size;
#endif
    uint32_t cons_head, prod_tail;
    uint32_t cons_next, entries;
    unsigned i;
    uint32_t mask = r->prod.mask;
    /* end of vars for rte_ring.h code */

    printf("[Consumer %d] Start\n", rte_lcore_id());

    hz = rte_get_tsc_hz();
    start_cycle = rte_get_tsc_cycles();
    for (j = 0; j < nb_iteration; j++) {

        while (1) {

            /* begin of rte_ring.h code */
            cons_head = r->cons.head;
            prod_tail = r->prod.tail;
            entries = prod_tail - cons_head;

            if (n > entries) {
                continue;
            }

            cons_next = cons_head + n;
            r->cons.head = cons_next;

            DEQUEUE_PTRS();
            rte_compiler_barrier();

            r->cons.tail = cons_next;
            /* end of rte_ring.h code */

            break;
        }
    }
    end_cycle = rte_get_tsc_cycles();
    time = (end_cycle - start_cycle) / (double)hz;

    printf("[Consumer %d] Finish\n", rte_lcore_id());
    printf(" - Count     : %'" PRId64 " (objs)\n", nb_iteration);
    printf(" - Time      : %.3f (s)\n", time);
    printf(" - Throughput: %'d (obj/s)\n", (int)(nb_iteration / time));
}

int app_mp_thread(void *data)
{
    unsigned lcore;
    uint64_t j;
    void *dummy_obj;
    uint64_t enqueue_tries = 0;

    /* vars for rte_ring.h code */
#if LITERAL_BULK_SIZE
    unsigned n = 1;
#else
    unsigned n = bulk_size;
#endif
    struct rte_ring *r = ring;
    void **obj_table = &dummy_obj;
    volatile uint32_t prod_head, cons_tail;
    uint32_t prod_next, free_entries;
    int success;
    unsigned i, rep = 0;
    uint32_t mask = r->prod.mask;
    /* end of vars for rte_ring.h code */

    lcore = rte_lcore_id();
    printf("[Producer %d] Start\n", lcore);

    for (j = 0; j < nb_iteration; j++) {
        while (1) {

 retry:
            /* begin of rte_ring.h code */
            do {
                enqueue_tries++;
                prod_head = r->prod.head;
                cons_tail = r->cons.tail;
                free_entries = (mask + cons_tail - prod_head);

                if (unlikely(n > free_entries)) {
                    goto retry;
                }

                prod_next = prod_head + n;
                success =
                    rte_atomic32_cmpset(&r->prod.head, prod_head, prod_next);
            } while (unlikely(success == 0));

            ENQUEUE_PTRS();
            rte_compiler_barrier();

            while (unlikely(r->prod.tail != prod_head)) {
                rte_pause();

                if (RTE_RING_PAUSE_REP_COUNT &&
                    ++rep == RTE_RING_PAUSE_REP_COUNT) {
                    rep = 0;
                    sched_yield();
                }
            }
            r->prod.tail = prod_next;
            /* end of rte_ring.h code */

            break;
        }
    }
    printf("[Producer %d] Finish\n", lcore);
    printf(" - CAS Tries  : %'" PRId64 "\n", enqueue_tries);
}

int app_mc_thread(void *data)
{
    uint64_t j, start_cycle, end_cycle, hz;
    double time;
    void *dummy_obj;
    uint64_t dequeue_tries = 0;

    /* vars for rte_ring.h code */
    struct rte_ring *r = ring;
    void **obj_table = &dummy_obj;
#if LITERAL_BULK_SIZE
    unsigned n = 1;
#else
    unsigned n = bulk_size;
#endif
    //uint32_t cons_head, prod_tail;
    //uint32_t cons_next, entries;
    volatile uint32_t cons_head, prod_tail;
    uint32_t cons_next, entries;
    int success;
    unsigned i, rep = 0;
    uint32_t mask = r->prod.mask;
    /* end of vars for rte_ring.h code */

    printf("[Consumer %d] Start\n", rte_lcore_id());

    hz = rte_get_tsc_hz();
    start_cycle = rte_get_tsc_cycles();
    for (j = 0; j < nb_iteration; j++) {
        while (1) {

 retry:
            /* begin of rte_ring.h code */
            do {
                dequeue_tries++;
                cons_head = r->cons.head;
                prod_tail = r->prod.tail;
                entries = (prod_tail - cons_head);

                if (n > entries) {
                    goto retry;
                }

                cons_next = cons_head + n;
                success =
                    rte_atomic32_cmpset(&r->cons.head, cons_head, cons_next);
            } while (unlikely(success == 0));

            DEQUEUE_PTRS();
            rte_compiler_barrier();

            while (unlikely(r->cons.tail != cons_head)) {
                rte_pause();
                if (RTE_RING_PAUSE_REP_COUNT &&
                    ++rep == RTE_RING_PAUSE_REP_COUNT) {
                    rep = 0;
                    sched_yield();
                }
            }
            r->cons.tail = cons_next;
            /* end of rte_ring.h code */

            break;
        }
    }
    end_cycle = rte_get_tsc_cycles();
    time = (end_cycle - start_cycle) / (double)hz;

    printf("[Consumer %d] Finish\n", rte_lcore_id());
    printf(" - Count     : %'" PRId64 " (objs)\n", nb_iteration);
    printf(" - Time      : %.3f (s)\n", time);
    printf(" - CAS Tries : %'" PRId64 "\n", dequeue_tries);
    printf(" - Throughput: %'d (obj/s)\n", (int)(nb_iteration / time));
}
