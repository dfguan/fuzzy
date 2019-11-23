/*
 * =====================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  a toy example on how to use kpipeline_t, the pipeline function is calculating a maximum interger for every n number. 
 *
 *        Version:  1.0
 *        Created:  26/06/2018 09:11:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D. Guan), dfguan@hit.edu.cn
 *   Organization:  Center for Bioinformatics, Harbin Institute of Technology
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "kthread.h"





/**
 * @struct	pipe_t
 * @brief	data structure for pipeline
 *
 */
typedef struct {
	FILE	*fp;
	int		m_batch_size;
}pipe_t;


typedef struct {
	int *data;
	int n_data;
	int m_data;
	int max;
}aux_in_t;


void *pipeline_worker(void *shared, int step, void *in)
{
	pipe_t *p = (pipe_t *)shared;
	if (step == 0) {
		aux_in_t *t = (aux_in_t *)calloc(1, sizeof(aux_in_t));
		int v;
		while (t->n_data < p->m_batch_size ) {
			if (fscanf(p->fp, "%d", &v) != 1) break;
			if (t->n_data >= t->m_data) {
				t->m_data = t->m_data ? t->m_data << 1 : 2;
				t->data = (int *)realloc(t->data, t->m_data * sizeof(int));
			}	
			if (t->data) 
				t->data[t->n_data++] = v;	
			 else {
				free(t);
				t = NULL;
				return 0;
			 } 
		}
		if (!t->n_data) {
			free(t); 
			t= NULL;
		} //NB: free wont set t to zero
		return t;
	} else if (step == 1) {
		int i; 
		aux_in_t *t = (aux_in_t *)in;
		t->max = INT32_MIN;
		for ( i = 0; i < t->n_data; ++i)
			if (t->data[i] > t->max) t->max = t->data[i];
		return in;
	} else if (step == 2) {
		aux_in_t *t = (aux_in_t *)in;
		fprintf(stderr, "max: %d\n", t->max);	
		free(t->data);free(t);
	}	
	return 0;
}

int main(int argc, char *argv[])
{
	pipe_t p;
	p.fp = fopen(argv[1], "r");
	p.m_batch_size = 5;
	kt_pipeline(3, pipeline_worker, &p, 3);
	//release resource
	fclose(p.fp);
	return 0;
}

