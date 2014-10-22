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
#include "url_match.h"

unsigned char urls[][MAX_PAT_LEN] = {
    "a",
    "aa∑a",
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
    "how", "bcefd", "apple", "google", "ms", "tencent", "baidu", "axon",
    "sssfdksfjiodsjifoejiofjidjsklfjkdlsajfkldsjaklf",
    "jieowju0jnvfkxnvjkxiurequ8493u04rs",
    "0394r9jnfdnvjxyfeqgfbdsafjdskvpokxopsvpfd-s0re",
    "293ri920hnvibvnwojfsjcoindisojfodf09ewhfewfs",
    "jdiso9f0e-fie0-wkfopsjdlnkxhvudapajfiodsaofe9wqfjijfkldsjfkldsnvkds",
    "ifew0920jfielwfnkdslnfkdlsnvjjifdig9ewfjklsfdklsnxnjvbjcx",
    "kdfjijinvx,nvjijof",
    "sina.com.cn/news.html",
    "google.com/api/auth/mail.xjioewfds",
    "147rt.net/images/dinbu1.js",
    "147rt.net/images/huanden.js",
    "1532888.com/ad/0.js",
    "1532888.com/ad/00.js",
    "1532888.com/ad/1.js",
    "1532888.com/ad/120X60≈ßß.js",
    "1532888.com/ad/120xm.js",
    "17.wo.com.cn/./newtouch2014/scripts/app.wx.min.js",
    "17173.com/https://s.ue.17173cdn.com/a/lib/passport/v2/js/passport.js",
    "17500.cn/0613/Js/common.js?v=58",
    "17500.cn/0613/Js/jquery-1.5.2.min.js",
    "17500.cn/https://w.cnzz.com/q_stat.php?id=1000449615&l=3",
    "17et.com/res/mobile/script/page.js?v=20110901111",
    "2345.com/css/common_20140911.js?v=2.28.1",
    "25800.net/js/google_caf.js?rte=1&tm=2&dn=25800.net&tid=1026",
    "25800.net/js/standard.js?rte=1&tm=2&dn=25800.net&tid=1026",
    "25pp.com/staticfiles/js/jquery-1.7.2.min.js",
    "33ly.com/Templates/33ly/Skins/newindex/js/jquery-1.7.2.min.js",
    "33ly.com/Templates/33ly/Skins/newindex/js/jquery.newSlideShow.js",
    "33ly.com/Templates/33ly/Skins/newindex/js/main.js",
    "33ly.com/Templates/33ly/Skins/newindex/js/new_default.js",
    "36.01ny.cn/api.php?mod=js&bid=5620",
    "36.01ny.cn/api.php?mod=js&bid=5621",
    "36.01ny.cn/api.php?mod=js&bid=5622",
    "36.cn/https://cert.ebs.gov.cn/govicon.js?id=9f50b45d-54a8-4af6-8c0e-68d3269670af&width=36&height=50",
};

void test_dat() {
    int ret = 0;
    unsigned char *url;
    struct dat *t_dat;
    int i, found;
    
    t_dat = create_dat(0, 1);
    printf("sizeof urls: %lu\n", sizeof(urls));
    ret = build_dat(t_dat, (unsigned char **)urls, sizeof(urls)/MAX_PAT_LEN);
    if (ret < 0) {
        printf("build_dat failed!\n");
    }
    
    for (i = 0; i < sizeof(urls)/MAX_PAT_LEN; i++) {
        url = urls[i];
        t_dat->match(t_dat, url, strlen((char *)url), 0, &found);
        if (found == 0) {
            printf("url %d %s should be matched.\n", i, url);
        } else {
            
        }
    }
    
    // test remove
    for (i = 0; i < sizeof(urls)/MAX_PAT_LEN; i++) {
        url = urls[i];
        ret = t_dat->remove(t_dat, url, strlen((char *)url), NULL);
        if (ret != 0) {
            printf("remove %d %s failed: %d\n", i, url, ret);
        }
        
        t_dat->match(t_dat, url, strlen((char *)url), 0, &found);
        if (found != 0) {
            printf("url %d %s has been removed, should be be matched.\n", i, url);
        }
    }
}

unsigned long getstrlen(char *ptr) {
    unsigned long len = 0;
    char *p = ptr;

    while(*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
        p++;
    }
    len = p - ptr;
    return len;
}

void test_file(char *fn) {
    char line[1024];
    char *ptr;
    int ret;
    int count = 0;
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
        len = getstrlen(ptr);
        ret = f_dat->insert(f_dat, (unsigned char *)ptr, len, 0, NULL);
        if (ret < 0) {
            printf("insert dat key %s failed!\n", ptr);
        }
        count ++;
        //if (count % 10 == 0)
        //    printf("insert %d key\n", count);
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        printf("fseek file %s to start failed!\n", fn);
        return;
    }
    count = 0;
    while (fgets(line, sizeof(line)-1, fp)) {
        count ++;
        len = getstrlen(line);
        data = f_dat->match(f_dat, (unsigned char *)line, len, 0, &found);
        if (found == 0) {
            printf("line %d %s should be matched.\n", count, line);
        }
    }

    fclose(fp);
}

void test_url(char *fn) {
    char line[1024];
    char *ptr;
    int ret;
    int count = 0;
    unsigned long len;
    FILE *fp = fopen(fn, "r");
    int found;
    
    if (!fp) {
        printf("Cannot open file %s\n", fn);
        return;
    }
    while (fgets(line, sizeof(line)-1, fp)) {
        ptr = line;
        ret = insert_url(ptr);
        if (ret < 0) {
            printf("insert dat key %s failed!\n", ptr);
        }
        count ++;
        //if (count % 10 == 0)
        //    printf("insert %d key\n", count);
    }
    if (fseek(fp, 0, SEEK_SET) < 0) {
        printf("fseek file %s to start failed!\n", fn);
        return;
    }
    
    count = 0;
    while (fgets(line, sizeof(line)-1, fp)) {
        count ++;
        len = getstrlen(line);
        found = find_url(line, len);
        if (found == 0) {
            printf("line %d %s should be matched.\n", count, line);
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
