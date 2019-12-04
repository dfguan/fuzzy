/*
 * =====================================================================================
 *
 *       Filename:  gmm.c
 *
 *    Description:  using EM algorithm to figure out gaussion mixture models
 *
 *        Version:  1.0
 *        Created:  03/12/2019 18:52:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D.G.), dfguan9@gmail.com
 *   Organization:  Harbin Institute of Technology
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <getopt.h>

typedef double latent;

typedef struct {
	double alpha, mu, delta;
} gmm_unit_t;


typedef struct {
	latent *gamma; 
	int k;
	double lkhd;
	gmm_unit_t *gms;
}gmm_t;

typedef struct {
	double v;
	double freq;
}freq_t;

typedef struct {
	uint32_t n, m; // number of clusters 
	uint32_t nsamps;
	freq_t *freqs;
} freqs_t;

typedef struct {
	char *histo;
	int  max_iter;
	double max_diff;
	int k, min_v, max_v;
}opt_t;


#define PI 3.1415926

freqs_t * freqs_init()
{
	return calloc(1, sizeof(freqs_t));
}

int freq_push(freqs_t *frqs, freq_t *frq)
{
	if (frqs->n >= frqs->m) {
		frqs->m = frqs->m ? frqs-> m << 1 : 16;
		frqs->freqs = realloc(frqs->freqs, sizeof(freq_t) * frqs->m);
	}
	frqs->freqs[frqs->n++] = *frq;
	return 0;
}

int freq_destroy(freqs_t *frqs)
{
	if (frqs) {
		if (frqs->freqs) free(frqs->freqs);
		free(frqs);	
	}	
	return 0;
}

int freq_clean(freqs_t *frqs)
{
	//start from zero whether there is a point with lowest coverage
		


}

double cal_prob(int x, double mu, double delta)
{
	double s = 2*pow(delta, 2);
	/*fprintf(stderr, "what is these: %d %lf %lf %lf\t%lf\n", x, mu, s, sqrt(PI*s), exp(-pow(x-mu, 2)/s));*/
	return 1.0/sqrt(PI*s) * exp(-pow(x-mu, 2)/s);
}

double cal_mean(uint32_t *ary, int n)
{
	double sum = 0;
	uint32_t nsample = 0;
	int i;
	for ( i = 0; i < n; ++i ) 
		sum += ary[i] * n, nsample += ary[i];
	
	return sum/nsample;	
}

int cal_gamma(gmm_t *gmt, int ng, freq_t *freqs)
{
	int i;
	int k = gmt->k;
	double *kp = malloc(sizeof(double) * k * ng);

	for ( i = 0; i < ng; ++i) {
		double *kpc = kp + i * k;
		int j;
		for ( j = 0; j < k; ++j) {
			double alpha_k = gmt->gms[j].alpha;
			double mu = gmt->gms[j].mu;
			double delta = gmt->gms[j].delta;
			/*fprintf(stderr, "density:%f %f\n", freqs[i].v, cal_prob(freqs[i].v, mu, delta));*/
			kpc[j] = alpha_k * cal_prob(freqs[i].v, mu, delta);
		} 
	}
	for ( i = 0; i < ng; ++i ) {
		double *kpc = kp + i * k;	
		double *gammas = gmt->gamma + i * k;
		int j;
		double sum = 0;
		for ( j = 0; j < k; ++j ) sum += kpc[j];
		for ( j = 0; j < k; ++j ) {
			gammas[j] = kpc[j]/sum;
			/*fprintf(stderr, "%d %d %lf\n", i, j, gammas[j]);*/
		}	
	}

	return 0;
}

double update_mu_delta_alpha(gmm_t *gmt, int ng, freq_t *freqs, uint32_t nsamps)
{	
	int i;
	double max_diff = 0;
	int k = gmt->k;
		
	for ( i = 0; i < k; ++i) {
		int j;
		gmm_unit_t *gmu = &gmt->gms[i];
		double *gamms = gmt->gamma;
		double old_mu = gmt->gms[i].mu;
		double old_delta = gmt->gms[i].delta;
		double old_alpha = gmt->gms[i].alpha;
		double new_mu, new_delta, new_alpha;
		double sum1 = 0, sum2 = 0, sum3 = 0; 

		for ( j = 0; j < ng; ++j ) sum1 += freqs[j].v * freqs[j].freq * gamms[j * k + i], sum2 += pow(freqs[j].v - old_mu, 2) * freqs[j].freq * gamms[j * k + i],sum3 += gamms[j * k + i] * freqs[j].freq;						
		
		new_mu = gmu->mu= sum1 / sum3;
		new_delta = gmu->delta = sqrt(sum2 / sum3);
		new_alpha = gmu->alpha =  sum3 / nsamps; 	

		double diff_mu = abs(new_mu - old_mu);
		double diff_delta = abs(new_delta - old_delta);
		/*double diff_alpha = abs(new_alpha - old_alpha);	*/

		if (diff_mu > max_diff) max_diff = diff_mu;
	   if (diff_delta > max_diff) max_diff = diff_delta;
		/*if (diff_alpha > max_diff) max_diff = diff_alpha;	   */
	}

	return max_diff;	
}

int cal_lkhd(gmm_t *gmt, uint32_t n, freq_t *freqs)
{
	gmm_unit_t *gmu = gmt->gms; 
	int k = gmt->k;
	double lkhd = 0;
	int i;
	for (i = 0; i < k; ++i) {
		int j;
		for ( j = 0; j < n; ++j ) {
			double prob = cal_prob(freqs[j].v, gmu[i].mu, gmu[i].delta);	
			lkhd += freqs[j].freq * log(prob);
		}
	}	
	gmt->lkhd = lkhd;
	return 0;
}




//ng number of groups input is a histogram
gmm_t *init_gmms(int k, double delta, double mu, uint32_t ng, freq_t *freqs)
{
	gmm_t * gmt = (gmm_t*)calloc(1,sizeof(gmm_t))	;
	gmt->k = k;
	gmt->gms = malloc(sizeof(gmm_unit_t) * k);
	gmt->gamma = malloc(sizeof(double) * k * ng);
	int i;
	gmm_unit_t *gmtu = gmt->gms;
	srand(time(NULL));
	uint32_t *ridxs = malloc(sizeof(uint32_t) * k);
	//generate k random numbers as means
		
	//won't be many just n
	while (i < k) {
		uint32_t r = rand()%ng;
		int z;
		for ( z = 0; z < i; ++z ) 
			if (ridxs[z] == r) break;
		if (z == i) ridxs[i++] = r;	
	}	

	for ( i = 0; i < k; ++i) {
		gmtu[i].alpha = 1 / (double) k;
		gmtu[i].mu = freqs[ridxs[i]].v;
		gmtu[i].delta = delta;			
	}
	free(ridxs);
	cal_lkhd(gmt, ng, freqs);
	return gmt;
}

int free_gmms(gmm_t *gmt)
{
	if (gmt) {
		if (gmt->gamma) free(gmt->gamma);
		if (gmt->gms) free(gmt->gms);
		free(gmt);
	}
}

/*cal_model_fit()*/
/*{*/
	

/*}*/

int print_models(gmm_t *gmt)
{
	int i; 
	for ( i = 0; i < gmt->k; ++i ) 
		fprintf(stdout, "[M::%s] Model %d: (alpha, mu, delta): %f %f %f\n", __func__, i, gmt->gms[i].alpha, gmt->gms[i].mu, gmt->gms[i].delta);			
	return 0;	
}

int run_all(opt_t *opts)
{

	//io
	double v,freq;
	
	char *histo = opts->histo;

	FILE *fp = fopen(histo, "r");
	
	freqs_t *frqs = freqs_init();
	freq_t u;
	while (fscanf(fp, "%lf\t%lf\n", &v, &freq) != EOF) {
		if (v > opts->max_v || v < opts->min_v) continue;
		u.v = v, u.freq = freq;
		freq_push(frqs, &u);			
	}
	fclose(fp);	
	int i;
	double sum = 0, nsamp = 0, sum_sum = 0;
	for ( i = 0 ; i < frqs->n; ++i ) nsamp += frqs->freqs[i].freq, sum += frqs->freqs[i].freq * frqs->freqs[i].v, sum_sum += frqs->freqs[i].freq *pow(frqs->freqs[i].v, 2); 
	frqs->nsamps = nsamp;
	//init gmms
	
	gmm_t *gmt = init_gmms(opts->k, sqrt(sum_sum / nsamp - pow(sum/nsamp, 2)), sum / nsamp, frqs->n, frqs->freqs);	
	for ( i = 0; i < opts->max_iter; ++ i ) {
		print_models(gmt);
		cal_gamma(gmt, frqs->n, frqs->freqs);	
		double max_diff = update_mu_delta_alpha(gmt, frqs->n, frqs->freqs, frqs->nsamps);		
		double old_lkhd = gmt->lkhd;
		cal_lkhd(gmt, frqs->n, frqs->freqs);
		fprintf(stdout, "[M::%s] %d Iteration, likelyhood difference: %lf\n", __func__, i + 1, fabs(old_lkhd - gmt->lkhd));
		if (fabs(old_lkhd - gmt->lkhd) < opts->max_diff) break;	
	}
	/*cal_model_fit(gmt, frqs->n, frqs->freqs);*/
	print_models(gmt);
}

int help() 
{

	return 0;
}

//max_diff to terminate the iterations

int main(int argc, char *argv[])
{
	int c;
	opt_t opts = (opt_t){0, 10000, 1e-7, 2, 3, 499};	
	//optind points at argv[1]
	char *program;
   	(program = strrchr(argv[0], '/')) ? ++program : (program = argv[0]);
	
	while (~(c=getopt(argc, argv, "k:i:d:v:V:h"))) {
		switch (c) {
			case 'k': 
				opts.k = atoi(optarg);
				break;
			case 'i':
				opts.max_iter = atoi(optarg);
				break;
			case 'V':
				opts.max_v = atoi(optarg);
				break;
			case 'v':
				opts.min_v = atoi(optarg);
				break;
			case 'd':
				opts.max_diff = atof(optarg);
				break;
			default:
				if (c != 'h') fprintf(stderr, "[E::%s] undefined option %c\n", __func__, c);
help:	
				fprintf(stderr, "\nUsage: %s [<options>] <HISTO> ...\n", program);
				fprintf(stderr, "Options:\n");
				fprintf(stderr, "         -i    INT      maximum iteration [10000]\n");	
				fprintf(stderr, "         -d    FLOAT    maximum difference to terminate the iterations [1e-7]\n");
				fprintf(stderr, "         -v    FLOAT    minimum model value [3]\n");
				fprintf(stderr, "         -V    FLOAT    maximum model value [499]\n");
				fprintf(stderr, "         -k    INT      number of gaussion models [2]\n");
				fprintf(stderr, "         -h             help\n");
				return 1;	
		}		
	}
	if (optind + 1 > argc) {
		fprintf(stderr,"[E::%s] require a histogram file!\n", __func__); goto help;
	}
	opts.histo = argv[optind];  
	fprintf(stderr, "Program starts\n");	
	run_all(&opts);
	fprintf(stderr, "Program ends\n");	
	return 0;	
}


