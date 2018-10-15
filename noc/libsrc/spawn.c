// #include <mppaipc.h>
#include <stdint.h>
#include <mppa/osconfig.h>
#include <stdio.h>
#include <assert.h>

#include <mppa_power.h>

//! Spawn section
#define NUM_CLUSTERS 16
static mppa_power_pid_t pids[NUM_CLUSTERS];

#ifdef _MAILBOX_
	int first_cluster = 0;
	int last_cluster = 1;
#else
	int first_cluster = 1;
	int last_cluster = 3;
#endif

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = first_cluster; i < last_cluster; i++)
	{	
		sprintf(arg0, "%d", i);
		args[0] = arg0;
		pids[i] = mppa_power_base_spawn(i, "slave", (const char **)args, NULL, MPPA_POWER_SHUFFLING_ENABLED);
		assert(pids[i] != -1);
	}
}

void join(void)
{
    int i, ret;
	for (i = first_cluster; i < last_cluster; i++)
		mppa_power_base_waitpid(i, &ret, 0);
}