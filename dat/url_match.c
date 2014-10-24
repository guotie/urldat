//
//  url_match.c
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "url_match.h"


#ifndef MIN_HOST_LEN
#define MIN_HOST_LEN 4
#endif

#ifndef MAX_HOST_LEN
#define MAX_HOST_LEN 64
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 192
#endif

void *alloc_mi() {
    struct path_mi *pm;
    pm = malloc(sizeof(struct path_mi));
    if (pm == NULL)
        return NULL;
    
    pthread_rwlock_init(&(pm->lock), NULL);
    pm->path_dat = NULL;
    pm->host_wildcast = 0;
    pm->path_wildcast = 0;

    return pm;
}

void free_mi(void *ptr) {
    struct path_mi *mi = ptr;
    pthread_rwlock_destroy(&mi->lock);
    destroy_dat(mi->path_dat, NULL);
    free(mi);
}

// split_url
// 将url转换为struct url_mi
// host: host只有第一个字符可以为*, 这时，host_wildcast为真
// path: path只有最后一个字符可以为*,这时，path_wildcast为真
//
// @return:
//    -1: 空字符串
//    -2: *号后必须是.
//    -3: *号只能出现在host的第一个字符
//    -4: host太长或太短
//    -5: path太长
int split_url(char *url, unsigned long len, char **host, int *host_len, int *host_wc, char **path, int *path_len, int *path_wc) {
    char *ptr = url, *_host = NULL, *_path = NULL;
    long _host_len = 0, _path_len = 0;
    int _host_wildcard = 0, _path_wildcard = 0;
    
    /* skip space */
    while (*ptr && (*ptr == ' ' || *ptr == '\t'))
        ptr++;
    
    if (*ptr)
        _host = ptr;
    else
        return -1;
    
    if (!strncmp(_host, "http://", 7))
        _host += 7;
    
    if (*_host == '.')
        _host_wildcard = DAT_ATTR_WILDCAST;
    else if (*_host == '*') {
        _host_wildcard = DAT_ATTR_WILDCAST;
        _host++;
        
        if (*_host != '.')    /* meaningless *a.com */
            return -2;
    }
    
    ptr = _host;
    while (*ptr && *ptr != '/' && *ptr != ' ' && *ptr != '\t' && *ptr != '\r' && *ptr != '\n') {
        // 除首字符外，不允许host的其他字符出现*
        if (*ptr == '*')
            return -3;
        ptr++;
    }
    
    _host_len = ptr - _host;
    if (_host_len < MIN_HOST_LEN ||
        _host_len >= MAX_HOST_LEN)
        return -4;
    
    if (*ptr == '/')
        _path = ptr++;
    else
        goto out;  /* only host parsed */
    
    while (*ptr && *ptr != '*' && *ptr != '\r' && *ptr != '\n')
        ptr++;
    
    _path_len = ptr - _path;
    if (_path_len >= MAX_PATH_LEN - 1)
        return -5;
    
    _path_wildcard = (*ptr == '*')? DAT_ATTR_WILDCAST : 0;

out:
    *host = _host;
    *path = _path;
    *host_len = (int)_host_len;
    *path_len = (int)_path_len;
    if(host_wc)
        *host_wc = _host_wildcard;
    if(path_wc)
        *path_wc = _path_wildcard;

    return 0;
}

int __insert_path(struct path_mi *pm, unsigned char *path, int path_len, int wildcast) {
    if (path == NULL) {
        return 0;
    }

    if (path_len <= 1) {
        if (path && *path == '/')
        // path '/'默认匹配
            return 0;
        return -1;
    }

    if (path_len == 2 && path[1] == '*') {
        pm->path_wildcast = 1;
        return 0;
    }

    if (pm->path_dat == NULL) {
        pm->path_dat = create_dat(0, 1);
        if (pm->path_dat == NULL) {
            return -1;
        }
    }

    return pm->path_dat->insert(pm->path_dat, path, (unsigned long)path_len, wildcast, NULL);
}

int insert_path(struct path_mi *pm, unsigned char *path, int path_len, int wildcast) {
    int ret;

    pthread_rwlock_wrlock(&pm->lock);
    ret = __insert_path(pm, path, path_len, wildcast);
    pthread_rwlock_unlock(&pm->lock);

    return ret;
}

int insert_url(struct url_dat *ud, char *url) {
    int ret;
    unsigned long len;
    char *host, *path;
    int host_len, path_len;
    int host_wc, path_wc;
    struct dat *host_dat = ud->d;
    
    int found = 0;
    struct path_mi *pm;
    
    len = strlen(url);
    if (len > MAX_HOST_LEN + MAX_PAT_LEN) {
        return -1;
    }

    if (split_url(url, len, &host, &host_len, &host_wc, &path, &path_len, &path_wc) < 0)
        return -1;

    if(ud == NULL)
        return -1;
    
    if (ud->d == NULL)
        return -1;

    /// insert host
    pthread_rwlock_wrlock(&(ud->lock));
    // 精确匹配
    pm = host_dat->match(host_dat, (unsigned char *)host, (unsigned long)host_len, 0, &found);
    if (!found) {
        pm = alloc_mi();
        if (pm == NULL) {
            pthread_rwlock_unlock(&ud->lock);
            return -1;
        }

        ret = host_dat->insert(host_dat, (unsigned char *)host, (unsigned long)host_len, host_wc, (void *)pm);
        if (ret < 0) {
            pthread_rwlock_unlock(&ud->lock);
            free_mi(pm);
            return -1;
        }
    }
    pthread_rwlock_unlock(&ud->lock);
    
    // insert path
    ret = insert_path(pm, (unsigned char *)path, path_len, path_wc);

    return ret;
}

static inline void * __find_host_path(struct dat *d, char *host, int host_len, char *path, int path_len, int *found) {
    struct path_mi *pm;
    struct dat *host_dat = d;
    
    pm = host_dat->match(host_dat, (unsigned char *)host, host_len, 1, found);
    if (*found == 0) {
        return NULL;
    }

    if (path_len <= 1) {
        return pm;
    }
    if (pm->path_dat == NULL) {
        if (pm->path_wildcast)
            return pm;

        *found = 0;
        return NULL;
    }
    
    pthread_rwlock_rdlock(&pm->lock);
    pm->path_dat->match(pm->path_dat, (unsigned char *)path, path_len, 1, found);
    pthread_rwlock_unlock(&pm->lock);
    
    return (void *)pm;
}

int find_host_path(struct url_dat *ud, char *host, int host_len, char *path, int path_len) {
    int found;
    void *data;
    
    if (ud == NULL)
        return 0;
    
    if (ud->d == NULL)
        return 0;

    pthread_rwlock_rdlock(&ud->lock);
    data = __find_host_path(ud->d, host, host_len, path, path_len, &found);
    pthread_rwlock_unlock(&ud->lock);
    
    return found;
}

int find_url(struct url_dat *ud, char *url, unsigned long len) {
    char *host, *path;
    int host_len, path_len;
    int ret;
    int found;
    
    ret = split_url(url, len, &host, &host_len, NULL, &path, &path_len, NULL);
    if (ret < 0)
        return 0;
    
    
    found = find_host_path(ud, host, host_len, path, path_len);
    return found;
}

struct url_dat *create_url_dat(int nocase) {
    struct url_dat *ud;
    
    ud = malloc(sizeof(*ud));
    if (!ud) {
        return NULL;
    }

    ud->d = create_dat(0, nocase);
    if (!ud->d) {
        free(ud);
        return NULL;
    }

    pthread_rwlock_init(&ud->lock, NULL);
    return ud;
}

void destroy_url_dat(struct url_dat *ud) {
    destroy_dat(ud->d, free_mi);
    pthread_rwlock_destroy(&ud->lock);
    free(ud);
}
