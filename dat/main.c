//
//  main.c
//  dat
//
//  Created by guotie on 14-10-15.
//  Copyright (c) 2014年 guotie. All rights reserved.
//

#include <stdio.h>
#include <strings.h>

#include "dat.h"

char urls[][MAX_PAT_LEN] = { "a",
    "aaa",
    "abcd",
    "aa",
    "aab",
    "ad",
    "aceiw",
    "djkafiew",
    "asd", "aglmnqioew",
    "bbbbb",
    "c", "cccc", "cccd",
    "bbbbbbae",
    "http://www.sina.cn", "alpha", "zzbc", "fals", "hi!", "ab", "cc", "ca", "sets",
    "wow", "baa", "ma", "mm",
    "how", "bcefd", "apple", "google", "ms", "tencent", "baidu", "axon"
};

void test_dat() {
    int ret = 0;
    char *url;
    struct dat *t_dat;
    int i, found;
    
    t_dat = create_dat(0, 1);
    printf("sizeof urls: %lu\n", sizeof(urls));
    ret = build_dat(t_dat, (char **)urls, sizeof(urls)/MAX_PAT_LEN);
    if (ret < 0) {
        printf("build_dat failed!\n");
    }
    
    for (i = 0; i < sizeof(urls)/MAX_PAT_LEN; i++) {
        url = urls[i];
        t_dat->match(t_dat, url, strlen(url), 0, &found);
        if (found == 0) {
            printf("url %d %s should be matched.\n", i, url);
        } else {
            
        }
    }
}

void test_file(char *fn) {
    char line[1024];
    char *ptr;
    int ret;
    unsigned long len;
    FILE *fp = fopen(fn, "r");
    struct dat *f_dat;
    int found;
    void *data;
    
    if (!fp) {
        printf("Cannot open file %s\n", fn);
        return;
    }
    
    f_dat = create_dat(0, 1);
    if (f_dat == NULL) {
        printf("create dat failed!\n");
        return;
    }

    while (fgets(line, sizeof(line)-1, fp)) {
        ptr = line;
        len = strlen(ptr);
        ret = f_dat->insert(f_dat, ptr, len, NULL);
        if (ret < 0) {
            printf("insert dat key %s failed!\n", ptr);
        }
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        printf("fseek file %s to start failed!\n", fn);
        return;
    }
    while (fgets(line, sizeof(line)-1, fp)) {
        len = strlen(line);
        data = f_dat->match(f_dat, line, len, 0, &found);
        if (found == 0) {
            printf("line %s should be matched.\n", line);
        }
    }

    fclose(fp);
}

int main(int argc, const char * argv[])
{
    // insert code here...
    if (argc == 1)
        test_dat();
    else if (argc == 2) {
        test_file((char *)argv[1]);
    }

    return 0;
}
