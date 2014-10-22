//
//  url_match.h
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#ifndef dat_url_match_h
#define dat_url_match_h

#include <pthread.h>
#include "dat.h"

#define URL_MI_HOST_LEN  64
#define URL_MI_PATH_LEN 192

struct path_mi {
    pthread_rwlock_t lock;

    int host_wildcast;
    int path_wildcast;
    
    struct dat *path_dat;
};

int insert_url(char *url);
int find_host_path(char *host, int host_len, char *path, int path_len);
int find_url(char *url, unsigned long len);

#endif
