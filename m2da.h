/*
 * =====================================================================================
 *
 *       Filename:  m2da.h
 *
 *    Description:  two dimensional array 
 *
 *        Version:  1.0
 *        Created:  21/09/2018 14:58:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D. Guan), dfguan@hit.edu.cn
 *   Organization:  Center for Bioinformatics, Harbin Institute of Technology
 *
 * =====================================================================================
 */
#ifndef M2DA_H
#define M2DA_H

#define kv_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

//usually we can represent two dimensional array with one dimensional array, if suppose the column are known
//however this is not often the case, so we develop two dimensional array
#define m2da_t(type) struct { size_t n,m; struct {size_t n, m; type *a;} *a} 
#define m2da_init(v) ((v).n = (v).m = 0, (v).a = 0)

#define m2da_ext(v, i) do { \
	if ( (i) >= (v).m ) { \
		size_t om = (v).m; \
		(v).m = kv_roundup32((i) + 1); \
		void *a = calloc((v).m, sizeof(char *)); \
		if ((v).a) { \
			memcpy(a, (v).a, sizeof(char *) * om); \
			free((v).a); \
		} \
		(v).a = a; \
	} \
} while (0)


#define m2da_push1(type, v, i) do { \
		m2da_ext(type, v, i); \
		if ( (i) >= (v).n) m2da_init((v).a[(i)]); \
	} \
} while(0)

#define m2da_push2(type, v, i, x) do { \
		if ((v).a[(i)].n >= (v).a[(i)].m) { \
			(v).a[(i)].m = (v).a[(i)].m ? (v).a[(i)].m << 1 : 2;  \
			(v).a[(i)].a = realloc(sizeof(type) * (v).a[(i)].m); \
		} \
		(v).a[(i)].a[(v).a[(i)].n++] = *(x); \
	} \
} while(0)

#define m2da_push(type, v, i, x) do { \
	m2da_push1(type, v, i);	\
	m2da_push2(type, v, i, x); \
}while (0) 


#define m2da_destory(v) do { \
	if ((v).a) { \
		int i; \
		for (i = 0;  i < (v).n; ++i) \
			if ((v).a[i].a) free((v).a[i].a); \
		free((v).a); \
	} \
} while (0);


#endif
