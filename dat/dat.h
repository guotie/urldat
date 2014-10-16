//
//  dat.h
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#ifndef dat_dat_h
#define dat_dat_h

#include <pthread.h>

#define DAT_NAME_LEN        32
#define MAX_PAT_LEN         256

struct dat_node {
    int base;
    int check;
    void *data;
};

struct dat {
    char name[DAT_NAME_LEN];

    struct dat_node *nodes;
    int array_len;
    int idle_count;
    
    int nocase;

    // methods
    int (*build)(struct dat *d, char *pats[MAX_PAT_LEN], int pat_count);
    int (*insert) (struct dat *d, char *pat, int patlen);
    int (*match) (struct dat *d, char *target, int targetlen, int option);
};

#endif
