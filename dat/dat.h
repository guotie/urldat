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

#define DAT_ATTR_WILDCAST   1

typedef unsigned long DatAttr;

struct dat_node {
    int base;
    int check;
    void *data;
    DatAttr attr;
};

struct dat {
    char name[DAT_NAME_LEN];

    struct dat_node *nodes;
    int array_len;
    int idle_count;
    
    int nocase;

    // methods
    int (*insert) (struct dat *, unsigned char *, unsigned long, unsigned long, void *);
    int (*rev_insert) (struct dat *, unsigned char *, unsigned long, unsigned long, void *);
    int (*remove) (struct dat *, unsigned char *, unsigned long, void (*free_fn)(void *));
    int (*rev_remove) (struct dat *, unsigned char *, unsigned long, void (*free_fn)(void *));
    void * (*match) (struct dat *, unsigned char *, unsigned long, int, int *);
    void * (*rev_match) (struct dat *, unsigned char *, unsigned long, int, int *);
};

struct dat * create_dat(int array_len, int nocase);
void destroy_dat(struct dat *, void (*free_fn)(void *));
int build_dat(struct dat *d, unsigned char *pats[MAX_PAT_LEN], int pat_count);
void dump_dat(struct dat *);

#endif
