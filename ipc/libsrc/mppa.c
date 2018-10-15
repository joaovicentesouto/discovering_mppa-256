
#include <mppaipc.h>
#include <mppa/osconfig.h>
#include <stdio.h>

#define NUM_CLUSTERS 16
static mppa_pid_t pids[NUM_CLUSTERS];

void spawn(void)
{
    int i;
	char arg0[4];
	char *args[2];

	/* Spawn slaves. */
	args[1] = NULL;
	for (i = 1; i < 3; i++)
	{	
		sprintf(arg0, "%d", i);
		args[0] = arg0;
		pids[i] = mppa_spawn(i, NULL, "slave", (const char **)args, NULL);
		// assert(pids[i] != -1);
	}
}

void join(void)
{
    int i;
	for (i = 1; i < 3; i++)
		mppa_waitpid(pids[i], NULL, 0);
}