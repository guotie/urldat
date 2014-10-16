//
//  url_match.c
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#include <stdio.h>
#include "url_match.h"

#define URL_MI_HOST_LEN  64
#define URL_MI_PATH_LEN 192

struct url_match_info {
    int host_wildcast;
    int path_wildcast;
    int path_noparam;
    char host_rev[URL_MI_HOST_LEN];
    char path[URL_MI_PATH_LEN];
};
