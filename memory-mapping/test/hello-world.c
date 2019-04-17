#include "stdio.h"
#include "stdlib.h"

#include <vbsp.h>

int main(void)
{
	int pid = __k1_get_cpu_id();

	for (int i = 0; i < 10; i++)
		printf("Hello World: %d\n", pid);

	return 0;
}
