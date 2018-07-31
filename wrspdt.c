/*
 * =====================================================================================
 *
 *       Filename:  write.c
 *
 *    Description:  test writing speed
 *
 *        Version:  1.0
 *        Created:  30/07/18 19:43:53
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
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#include <getopt.h>




int main(int argc, char *argv[])
{
	int c;
	size_t bs = 1000000;
	int cnt = 1000; //1k	
	while ((c = getopt(argc, argv, "m:c:h") >= 0)) {
		switch (c) {
			case 'm': 
				{ // notice this bracket here // variable are not allow to declare outside this brackets
					int arg_l = strlen(optarg);
					int l_c = toupper(optarg[arg_l - 1]);
					size_t times = 1;	
					if (isalpha(l_c)) {
						optarg[arg_l - 1] = 0;//is it changable?
						if (l_c == 'K') {
							times = 1000;	
						} else if (l_c == 'M') {
							times = 1000000;	
						} else {
							fprintf(stderr, "%c is not accepted in block size, please use K, M instead\n", l_c);
							return -1;
						}	
					} 
					bs = times * atoi(optarg); //assume our users are very nice guys // will this lead to overflow?
				}
				break;	
			case 'c':
				cnt = atoi(optarg); // again nice users
				break;
			case 'h':
	help:
				fprintf(stderr, "Usage: wrtspd [options]\n");
				fprintf(stderr, "       -m    STR    block size, could be numeric or human readable format, e.g. 1M [1M]\n");
				fprintf(stderr, "       -c    INT    count of blocks, count * bs bytes will be wrote into a file [1000]\n");
				return 0;
			default:
				fprintf(stderr, "[E::%s] undefined option\n", __func__);
				goto help;	
		}	
	}
	
	FILE *fp = fopen("spd_test.fl", "wb");
	char *blk = (char *)malloc(sizeof(char) * bs); 
	if (!blk) {
		fprintf(stderr, "[E::%s] memory allocation error, required %ld Bytes", __func__, bs);
		return -1;
	}
	int i = 0;
	clock_t ob = clock();	
	while ( i < cnt) {
		/*clock_t ib = clock();	*/
		if (fwrite(blk, sizeof(char), bs , fp) < bs) {
			fprintf(stderr, "[E::%s] write failed\n",__func__);
			return -1;
		}
		/*clock_t ie = clock();*/
		/*fprintf(stderr, "INNER: %.2lfB PER SEC\n", (double)bs / (double)(ie -ib)/ CLOCKS_PER_SEC);*/
		++i;
	}
	clock_t oe = clock();
	
	double st = (double)(oe - ob)/ CLOCKS_PER_SEC;
	double bytes_per_sec = (double)(bs * cnt) / st;
	
	char buf[1024] = {0};
	char three_lv[] = {'G','M','K', 0};
	
	int unit = 999999999;
	for ( i = 0; i < 3; ++i, unit /= 1000) {
		if ((int64_t)bytes_per_sec > unit) {
			sprintf(buf, "%.2lf %cB/S", bytes_per_sec/unit, three_lv[i]);
			break;
		}		
	}
	
	fprintf(stderr, "WRITTING SPEED: %s\n", buf); //better output human readable
	remove("spd_test.fl");
	if (blk) free(blk);
	if (fp) fclose(fp);
	return 0;
}



