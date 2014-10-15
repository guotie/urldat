//
//  url_match.h
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#ifndef dat_url_match_h
#define dat_url_match_h

#include "dat.h"

struct url_match {
    char *host;
    char *path;
    unsigned short host_len;
    unsigned short path_len;
    
    int host_wildcast;
    int path_wildcast;
};


#endif
