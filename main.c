#include <locale.h>
#include <stdio.h>

#include <rte_eal.h>
#include <rte_debug.h>
#include <rte_ring.h>

#include "main.h"

struct rte_ring *ring;
char *ring_name = "ring-0";
const int ring_size = RING_SIZE;
const int bulk_size = BULK_SIZE;
uint64_t nb_iteration = NB_ITERATION;
uint64_t nb_overfeed = NB_OVERFEED;

int main(int argc, char **argv)
{
	int ret, nb_producers;
	unsigned socket_io;

	/* initialize EAL first */
	ret = rte_eal_init(argc, argv);

	argc -= ret;
	argv += ret;

	if (argc != 2) {
		rte_panic
		    ("Please specify number of producer/consumers\n test-rte_ring <eal_args> -- <num>\n");
	}

	setlocale(LC_NUMERIC, "");

	nb_producers = atoi(argv[1]);
	printf("[MASTER] Number of Producers/Consumers: %d\n", nb_producers);

	socket_io = rte_lcore_to_socket_id(rte_get_master_lcore());

	ring = rte_ring_create(ring_name,
			       ring_size,
			       socket_io, RING_F_SP_ENQ | RING_F_SC_DEQ);

	if (ring == NULL) {
		rte_panic("Cannot create ring");
	}

	driver_start_all(nb_producers);

	rte_eal_mp_wait_lcore();
}
