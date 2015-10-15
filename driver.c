#include <rte_eal.h>
#include <rte_lcore.h>

#include "main.h"

void driver_sp_sc()
{
    int i, lcore;

    lcore = rte_get_master_lcore();
    lcore = rte_get_next_lcore(lcore, 1, 0);
    if (lcore < RTE_MAX_LCORE) {
        rte_eal_remote_launch(app_sc_thread, NULL, lcore);
    }

    rte_delay_us(2);

    lcore = rte_get_next_lcore(lcore, 1, 0);
    if (lcore < RTE_MAX_LCORE) {
        rte_eal_remote_launch(app_sp_thread, NULL, lcore);
    }
}

void driver_mp_mc()
{
    int i, lcore;
    lcore = rte_get_master_lcore();

    for (i = 0; i < nb_producers; i++) {
        lcore = rte_get_next_lcore(lcore, 1, 0);
        if (lcore < RTE_MAX_LCORE) {
            rte_eal_remote_launch(app_mp_thread, NULL, lcore);
        }
    }

    rte_delay_us(2);

    for (i = 0; i < nb_producers; i++) {
        lcore = rte_get_next_lcore(lcore, 1, 0);
        if (lcore < RTE_MAX_LCORE) {
            rte_eal_remote_launch(app_mc_thread, NULL, lcore);
        }
    }
}

void driver_sp_sc_bulk()
{
    int i, lcore;

    lcore = rte_get_master_lcore();
    lcore = rte_get_next_lcore(lcore, 1, 0);
    if (lcore < RTE_MAX_LCORE) {
        rte_eal_remote_launch(app_sc_bulk_thread, NULL, lcore);
    }

    rte_delay_us(2);

    lcore = rte_get_next_lcore(lcore, 1, 0);
    if (lcore < RTE_MAX_LCORE) {
        rte_eal_remote_launch(app_sp_bulk_thread, NULL, lcore);
    }
}

void driver_mp_mc_bulk()
{
    int i, lcore;
    lcore = rte_get_master_lcore();

    for (i = 0; i < nb_producers; i++) {
        lcore = rte_get_next_lcore(lcore, 1, 0);
        if (lcore < RTE_MAX_LCORE) {
            rte_eal_remote_launch(app_mp_bulk_thread, NULL, lcore);
        }
    }

    rte_delay_us(2);

    for (i = 0; i < nb_producers; i++) {
        lcore = rte_get_next_lcore(lcore, 1, 0);
        if (lcore < RTE_MAX_LCORE) {
            rte_eal_remote_launch(app_mc_bulk_thread, NULL, lcore);
        }
    }
}
