/*
 * =====================================================================================
 *
 *       Filename:  pickup_beans.c
 *
 *    Description:  how many times does it take to pick all beans at least once? 
 *
 *        Version:  1.0
 *        Created:  09/05/2018 19:20:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D. Guan), dfguan@hit.edu.cn
 *   Organization:  Center for Bioinformatics, Harbin Institute of Technology
 *
 * =====================================================================================
 */

//n beans in a bowl, pick m beans with replacement each time, how many times to pick at least one of each bean? don't know the distribution so write a program to simulate the process. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

int pick_m_beans(uint8_t *b, int32_t n, int32_t m, uint8_t *c) 
{
	int32_t i = 0;
	memset(c, 0, n);
	for ( i = 0; i < m; ++i) {
		int r = rand() % n;
		if (c[r]) --i; else c[r] = 1;
	} 
	for ( i = 0; i < n; ++i ) b[i] = c[i] | b[i];	
	return 0;
}

int32_t picked_beans(uint8_t *b, int32_t n_beans)
{
	int32_t s = 0;
	int32_t i = 0;
	for ( i = 0; i < n_beans; ++i) if (b[i]) ++s;
	return s;

}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	int32_t n_beans = atoi(argv[1]);
	int32_t pick_m_beans_each_time = atoi(argv[2]);
	
	int32_t iter = atoi(argv[3]);

	uint8_t *beans = (uint8_t *)calloc(n_beans<<1, sizeof(uint8_t));
	int32_t i = 0;
	/*fprintf(stderr, "%d\t%d\t%d\n",n_beans, pick_m_beans_each_time, iter);*/
	for (i = 0; i < iter; ++i) {
		memset(beans, 0, n_beans);
		int32_t t = 0;
		while ( picked_beans(beans, n_beans) != n_beans) {
			++t;
			pick_m_beans(beans, n_beans, pick_m_beans_each_time, beans + n_beans);
		}
		fprintf(stderr,"%d iteration: %d\n",i, t);	
	}
	if (beans) free(beans);	
	return 0;

}
