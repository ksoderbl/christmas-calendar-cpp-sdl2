#include <stdio.h>
#include <stdlib.h>


/* returns double in interval 0.0 - 1.0 */
double randdouble(void)
{
	return (double)rand()/(double)RAND_MAX;
}
