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

unsigned char urls1[][MAX_PAT_LEN] = {
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
unsigned char urls2[][MAX_PAT_LEN] = {
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
    "A",
    "AA∑A",
    "ABCD",
    "AA",
    "AAB",
    "AD",
    "ACEIW",
    "DJKAFIEW",
    "ASD", "AGLMNQIOEW",
    "BBBBB",
    "C", "CCCC", "CCCD",
    "BBBBBBAE",
    "HTTP://WWW.SINA.CN", "ALPHA", "ZZBC", "FALS", "HI!", "AB", "CC", "CA", "SETS",
    "WOW", "BAA", "MA", "MM",
    "HOW", "BCEFD", "APPLE", "GOOGLE", "MS", "TENCENT", "BAIDU", "AXON",
    "SSSFDKSFJIODSJIFOEJIOFJIDJSKLFJKDLSAJFKLDSJAKLF",
    "JIEOWJU0JNVFKXNVJKXIUREQU8493U04RS",
    "0394R9JNFDNVJXYFEQGFBDSAFJDSKVPOKXOPSVPFD-S0RE",
    "293RI920HNVIBVNWOJFSJCOINDISOJFODF09EWHFEWFS",
    "JDISO9F0E-FIE0-WKFOPSJDLNKXHVUDAPAJFIODSAOFE9WQFJIJFKLDSJFKLDSNVKDS",
    "IFEW0920JFIELWFNKDSLNFKDLSNVJJIFDIG9EWFJKLSFDKLSNXNJVBJCX",
    "KDFJIJINVX,NVJIJOF",
    "SINA.COM.CN/NEWS.HTML",
    "GOOGLE.COM/API/AUTH/MAIL.XJIOEWFDS",
    "147RT.NET/IMAGES/DINBU1.JS",
    "147RT.NET/IMAGES/HUANDEN.JS",
    "1532888.COM/AD/0.JS",
    "1532888.COM/AD/00.JS",
    "1532888.COM/AD/1.JS",
    "1532888.COM/AD/120X60≈SSSS.JS",
    "1532888.COM/AD/120XM.JS",
    "17.WO.COM.CN/./NEWTOUCH2014/SCRIPTS/APP.WX.MIN.JS",
    "17173.COM/HTTPS://S.UE.17173CDN.COM/A/LIB/PASSPORT/V2/JS/PASSPORT.JS",
    "17500.CN/0613/JS/COMMON.JS?V=58",
    "17500.CN/0613/JS/JQUERY-1.5.2.MIN.JS",
    "17500.CN/HTTPS://W.CNZZ.COM/Q_STAT.PHP?ID=1000449615&L=3",
    "17ET.COM/RES/MOBILE/SCRIPT/PAGE.JS?V=20110901111",
    "2345.COM/CSS/COMMON_20140911.JS?V=2.28.1",
    "25800.NET/JS/GOOGLE_CAF.JS?RTE=1&TM=2&DN=25800.NET&TID=1026",
    "25800.NET/JS/STANDARD.JS?RTE=1&TM=2&DN=25800.NET&TID=1026",
    "25PP.COM/STATICFILES/JS/JQUERY-1.7.2.MIN.JS",
    "33LY.COM/TEMPLATES/33LY/SKINS/NEWINDEX/JS/JQUERY-1.7.2.MIN.JS",
    "33LY.COM/TEMPLATES/33LY/SKINS/NEWINDEX/JS/JQUERY.NEWSLIDESHOW.JS",
    "33LY.COM/TEMPLATES/33LY/SKINS/NEWINDEX/JS/MAIN.JS",
    "33LY.COM/TEMPLATES/33LY/SKINS/NEWINDEX/JS/NEW_DEFAULT.JS",
    "36.01NY.CN/API.PHP?MOD=JS&BID=5620",
    "36.01NY.CN/API.PHP?MOD=JS&BID=5621",
    "36.01NY.CN/API.PHP?MOD=JS&BID=5622",
    "36.CN/HTTPS://CERT.EBS.GOV.CN/GOVICON.JS?ID=9F50B45D-54A8-4AF6-8C0E-68D3269670AF&WIDTH=36&HEIGHT=50",
};

void test_dat(char *urls[MAX_PAT_LEN], int len, int nocase) {
    int ret = 0;
    unsigned char *url;
    struct dat *t_dat;
    int i, found;
    int items = len / MAX_PAT_LEN;
    
    t_dat = create_dat(0, nocase);
    printf("sizeof urls: %lu, items: %d\n", sizeof((char **)urls), items);
    ret = build_dat(t_dat, (unsigned char **)urls, items);
    if (ret < 0) {
        printf("build_dat failed!\n");
    }
    
    for (i = 0; i < items; i++) {
        url = (unsigned char *)((char **)urls) + i * MAX_PAT_LEN;
        t_dat->match(t_dat, url, strlen((char *)url), 0, &found);
        if (found == 0) {
            printf("url %d %s should be matched.\n", i, url);
        } else {
            
        }
    }
    
    // test remove
    for (i = 0; i < items; i++) {
        url = (unsigned char *)(urls) + i * MAX_PAT_LEN;
        
        t_dat->match(t_dat, url, strlen((char *)url), 0, &found);
        if (found == 0) {
            printf("url %d %s should be matched.\n", i, url);
        }
        
        ret = t_dat->remove(t_dat, url, strlen((char *)url), NULL);
        if (ret != 0) {
            printf("remove %d %s failed: %d\n", i, url, ret);
        }
        
        t_dat->match(t_dat, url, strlen((char *)url), 0, &found);
        if (found != 0) {
            printf("url %d %s has been removed, should NOT be matched.\n", i, url);
        }
    }
    dump_dat(t_dat);
    destroy_dat(t_dat, NULL);
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

    if (fseek(fp, 0, SEEK_SET) < 0) {
        printf("fseek file %s to start failed 2!\n", fn);
        return;
    }
    count = 0;
    while (fgets(line, sizeof(line)-1, fp)) {
        count ++;
        len = getstrlen(line);
        f_dat->match(f_dat, (unsigned char *)line, len, 0, &found);
        if (found == 0) {
            printf("line %d %s should be matched.\n", count, line);
        }
        ret = f_dat->remove(f_dat, (unsigned char *)line, len, NULL);
        if (ret < 0) {
            printf("remove %d %s failed!\n", count, line);
            continue;
        }
        data = f_dat->match(f_dat, (unsigned char *)line, len, 0, &found);
        if (found != 0) {
            printf("line %d %s has been removed, should NOT be matched.\n", count, line);
        }
    }

    fclose(fp);
    dump_dat(f_dat);
    destroy_dat(f_dat, NULL);
}

void test_url(char *fn) {
    char line[1024];
    char *ptr;
    int ret;
    int count = 0;
    unsigned long len;
    FILE *fp = fopen(fn, "r");
    struct url_dat *ud = NULL;
    int found;
    
    ud = create_url_dat(0);
    if (!ud) {
        printf("Cannot create url_dat\n");
        return;
    }
    if (!fp) {
        printf("Cannot open file %s\n", fn);
        return;
    }
    
    while (fgets(line, sizeof(line)-1, fp)) {
        ptr = line;
        ret = insert_url(ud, ptr);
        if (ret < 0) {
            printf("insert dat key %s failed! ret=%d\n", ptr, ret);
        }
        count ++;
        //if (count % 10 == 0)
        //    printf("insert %d key\n", count);
    }
    printf("insert %d items.\n", count);
    if (fseek(fp, 0, SEEK_SET) < 0) {
        printf("fseek file %s to start failed!\n", fn);
        return;
    }
    
    count = 0;
    while (fgets(line, sizeof(line)-1, fp)) {
        count ++;
        len = getstrlen(line);
        found = find_url(ud, line, len);
        if (found <= 0) {
            printf("line %d %s should be matched.\n", count, line);
        }
    }
    
    fclose(fp);
    destroy_url_dat(ud);
}

int main(int argc, const char * argv[])
{
    // insert code here...
    if (argc == 1) {
        test_dat((char **)urls1, sizeof(urls1), 0);
        test_dat((char **)urls1, sizeof(urls1), 1);
        test_dat((char **)urls2, sizeof(urls2), 0);
        test_dat((char **)urls2, sizeof(urls2), 1);
    }
    else if (argc == 2) {
        test_file((char *)argv[1]);
        test_url((char *)argv[1]);
    }

    return 0;
}
