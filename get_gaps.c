/*
 * =====================================================================================
 *
 *       Filename:  get_gaps.c
 *
 *    Description:  get gaps from sequences
 *
 *        Version:  1.0
 *        Created:  13/11/2018 20:04:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D. Guan), dfguan@hit.edu.cn
 *   Organization:  Center for Bioinformatics, Harbin Institute of Technology
 *
 * =====================================================================================
 */


#include <zlib.h>
#include <stdio.h>
#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

int main() {
    gzFile fp;
    kseq_t *seq;
    fp = gzdopen(fileno(stdin), "r");
    seq = kseq_init(fp);
    while (kseq_read(seq) >= 0)  {
        int l = seq->seq.l; 
        char *s = seq->seq.s;
        int i;
        for ( i = 0; i < l; ++i) {
            if (s[i] == 'n' || s[i] == 'N') {
                int j;
                for ( j = i + 1; j < l && (s[j] == 'n' || s[j] == 'N'); ++j);
                fprintf(stdout, "%s\t%d\t%d\n",seq->name.s, i+1, j);
                i = j;
            }   
        } 
    }
    kseq_destroy(seq);
    gzclose(fp);
    return 0;
}
