#include <inttypes.h>
#include <stdio.h>

#include <rte_cycles.h>
#include <rte_ring.h>

#include "main.h"

int app_sp_thread(void *dummy)
{
	unsigned lcore;
	int ret;
	void *dummy_obj;
	uint64_t i, start_cycle, end_cycle, hz;
	double time;

	lcore = rte_lcore_id();
	printf("[Producer %d] Start\n", lcore);

	hz = rte_get_tsc_hz();
	start_cycle = rte_get_tsc_cycles();
	for (i = 0; i < nb_iteration; i++) {
		/* Since we assume rte_ring_sc_dequeue is faster, we need to check if
		 * this is true all the time, e.g. rte_panic if otherwise */
		if (unlikely(rte_ring_sp_enqueue(ring, dummy_obj))) {
			rte_panic("[Producer %d] Blocked @ obj #%'" PRId64 "\n",
				  lcore, i);
		}
	}
	end_cycle = rte_get_tsc_cycles();
	time = (end_cycle - start_cycle) / (double)hz;

	printf("[Producer %d] Finish\n", lcore);

	printf("Count     : %'" PRId64 " (objs)\n", nb_iteration);
	printf("Time      : %.3f (s)\n", time);
	printf("Throughput: %'d (obj/s)\n", (int)(nb_iteration / time));
}

int app_sc_thread(void *dummy)
{
	int ret;
	uint64_t i;
	void *dummy_obj;

	printf("[Consumer %d] Start\n", rte_lcore_id());

	for (i = 0; i < nb_iteration; i++) {
		while (unlikely(rte_ring_sc_dequeue(ring, &dummy_obj))) ;

		//if (unlikely(rte_ring_sc_dequeue(ring, &dummy_obj)))
		//    rte_panic("[Consumer %d] Consumer starved at obj #%'"PRId64"\n", rte_lcore_id(), i);
	}

	printf("[Consumer %d] Finish\n", rte_lcore_id());
}

int app_sp_thread_bulk(void *dummy)
{
	unsigned lcore;
	int ret;
	void *dummy_objs[bulk_size];
	uint64_t i, start_cycle, end_cycle, hz;
	double time;

	lcore = rte_lcore_id();
	printf("[Producer %d] Start\n", lcore);

	hz = rte_get_tsc_hz();
	start_cycle = rte_get_tsc_cycles();
	for (i = 0; i < nb_iteration; i++) {
		/* Since we assume rte_ring_sc_dequeue is faster, we need to check if
		 * this is true all the time, e.g. rte_panic if otherwise */
		if (unlikely
		    (rte_ring_sp_enqueue_bulk(ring, dummy_objs, bulk_size))) {
			rte_panic("[Producer %d] Blocked @ obj #%'" PRId64 "\n",
				  lcore, i);
		}
	}
	end_cycle = rte_get_tsc_cycles();
	time = (end_cycle - start_cycle) / (double)hz;

	printf("[Producer %d] Finish\n", lcore);

	printf("Count     : %'" PRId64 " (objs)\n", nb_iteration);
	printf("Time      : %.3f (s)\n", time);
	printf("Throughput: %'d (obj/s)\n", (int)(nb_iteration / time));
}

int app_sc_thread_bulk(void *dummy)
{
	int ret;
	uint64_t i;
	void *dummy_objs[bulk_size];

	printf("[Consumer %d] Start\n", rte_lcore_id());

	for (i = 0; i < nb_iteration; i++) {
		while (unlikely
		       (rte_ring_sc_dequeue_bulk
			(ring, dummy_objs, bulk_size))) ;

		//if (unlikely(rte_ring_sc_dequeue(ring, &dummy_obj)))
		//    rte_panic("[Consumer %d] Consumer starved at obj #%'"PRId64"\n", rte_lcore_id(), i);
	}

	printf("[Consumer %d] Finish\n", rte_lcore_id());
}

int app_mp_thread(void *dummy)
{
	unsigned lcore;
	int ret;
	uint64_t i;
	void *dummy_obj;

	lcore = rte_lcore_id();
	printf("[Producer %d] Start\n", lcore);

	for (i = 0; i < nb_iteration; i++) {
		/* Since we assume rte_ring_sc_dequeue is faster, we need to check if
		 * this is true all the time, e.g. rte_panic if otherwise */
		while (unlikely(rte_ring_mp_enqueue(ring, dummy_obj))) ;
	}
	printf("[Producer %d] Finish\n", lcore);
}

int app_mc_thread(void *dummy)
{
	int ret;
	uint64_t i, start_cycle, end_cycle, hz;
	double time;
	void *dummy_obj;

	printf("[Consumer %d] Start\n", rte_lcore_id());

	hz = rte_get_tsc_hz();
	start_cycle = rte_get_tsc_cycles();
	for (i = 0; i < nb_iteration; i++) {
		while (unlikely(rte_ring_mc_dequeue(ring, &dummy_obj))) ;
	}
	end_cycle = rte_get_tsc_cycles();
	time = (end_cycle - start_cycle) / (double)hz;

	printf("[Consumer %d] Finish\n", rte_lcore_id());

	printf("Count     : %'" PRId64 " (objs)\n", nb_iteration);
	printf("Time      : %.3f (s)\n", time);
	printf("Throughput: %'d (obj/s)\n", (int)(nb_iteration / time));
}
