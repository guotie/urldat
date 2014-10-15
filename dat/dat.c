//
//  dat.c
//  dat
//
//  Created by guotie on 14-10-14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dat.h"

#define DEFAULT_DAT_LEN     4096
#define MAX_DAT_LEN         (1024 * 2048)
#define DAT_END_POS         -2147483648

#define FIRST_FREE_INDEX(d) ((d)->nodes[0].check)

#define NODE_BASE(d, i)  ((d)->nodes[i].base)
#define NODE_CHECK(d, i) ((d)->nodes[i].check)

#define TOLOWWER(c) do{ if((c)>='A' && (c)<='Z') (c) = (c) + 'a' - 'A'; }while(0)

// 初始化数组链表
// 设置idle_count
void init_dat_nodes(struct dat *d) {
    int i;
    
    // position 0 是特殊位置，不是数组链表的一部分，相当于固定的链表头
    NODE_BASE(d, 0) = 0;
    NODE_CHECK(d, 0) = 1;

    for(i = 1; i < d->array_len; i ++) {
        NODE_BASE(d, i) = -(i - 1);
        NODE_CHECK(d, i) = -(i + 1);
    }
    NODE_BASE(d, 1) = -(i - 1);
    NODE_CHECK(d, i-1) = -1;

    d->idle_count = i - 1;
}

// init dat base array & check array
struct dat * init_dat(int array_len) {
    struct dat *d = NULL;
    
    d = malloc(sizeof(struct dat));
    if (d == NULL) {
        return NULL;
    }

    memset(d, 0, sizeof(struct dat));
    if (array_len <= 0) {
        array_len = DEFAULT_DAT_LEN;
    }

    assert(array_len > 256);
    
    d->array_len = array_len;
    d->nodes = malloc(sizeof(struct dat_node) * array_len);
    if (!d->nodes) {
        free(d);
        return NULL;
    }
    
    init_dat_nodes(d);

    return 0;
}


// expand dat array
int expand_dat_array(struct dat *d, int new_length) {
    int i;
    struct dat_node *nodes;
    
    if (new_length <= d->array_len) {
        return 0;
    }
    
    new_length = (new_length + 4096) & (~4096);

    nodes = malloc(sizeof(struct dat_node) * new_length);
    if (!nodes) {
        return -1;
    }
    
    // 复制原来nodes的数据
    memset(nodes, 0, sizeof(struct dat_node) * new_length);
    memcpy(nodes, d->nodes, sizeof(struct dat_node) * d->array_len);
    
    // 初始化新增节点
    for (i = d->array_len; i < new_length; i ++) {
        nodes[i].base = -(i - 1);
        nodes[i].check = -(i + 1);
    }
    
    if (d->nodes[0].check == 0) {
        // 没有空余节点
        nodes[0].check = d->array_len;
        nodes[d->array_len].base = -(i-1);
        nodes[d->array_len].check = -d->array_len;
    } else {
        // 原有的空余节点与新增节点连接起来
        int first = NODE_CHECK(d, 0);
        int last = -1 * NODE_BASE(d, first);
        
        nodes[d->array_len].base = -last;
        nodes[last].check = -d->array_len;
        
        nodes[first].base = -i + 1;
        nodes[i - 1].check = -first;
    }

    free(d->nodes);
    d->nodes = nodes;
    d->idle_count += new_length - d->array_len;
    d->array_len = new_length;

    return 0;
}

// 将节点idx从空闲状态移除
void del_free_node_idx(struct dat *d, int idx) {
    if (d->idle_count <= 0) {
        return;
    }
    if (d->idle_count == 1) {
        NODE_CHECK(d, 0) = 0;
    } else {
        int b = NODE_BASE(d, idx);
        int c = NODE_CHECK(d, idx);
        if (idx == FIRST_FREE_INDEX(d)) {
            FIRST_FREE_INDEX(d) = -1 * c;
        }
        d->nodes[-1 * c].base = b;
        d->nodes[-1 * b].check = c;
    }

    d->idle_count --;
    return;
}

void add_free_node_idx(struct dat* d, int idx) {
    int t, first;

    if (d->idle_count == 0) {
        FIRST_FREE_INDEX(d) = idx;
        NODE_BASE(d, idx) = -idx;
        NODE_CHECK(d, idx) = -idx;
    } else {
        first = FIRST_FREE_INDEX(d);

        for(t = -1 * NODE_CHECK(d, first); t != first && t < idx;) {
            t = -1 * NODE_CHECK(d, t);
        }
        
        NODE_BASE(d, idx) = NODE_BASE(d, t);
        NODE_CHECK(d, -1 * NODE_BASE(d, t)) = -1 * idx;
        
        NODE_BASE(d, t) = -1 * idx;
        NODE_CHECK(d, idx) = -1 * t;
        
        if (idx < FIRST_FREE_INDEX(d)) {
            FIRST_FREE_INDEX(d) = idx;
        }
    }

    d->idle_count ++;
    return;
}

int __find_pos(struct dat *d, int s, char ch) {
    int start, pos, olen;
    
    if (d->idle_count > 0) {
        pos = start = FIRST_FREE_INDEX(d);
        do {
            pos = -1 * NODE_CHECK(d, pos);
        } while(pos <= 256 && pos != start);
        
        if (pos > 256) {
            return pos;
        }
    }
    olen = d->array_len;
    if(expand_dat_array(d, olen + 256) < 0) {
        return -1;
    }
    
    return olen;
}

int find_pos(struct dat *d, int s, char ch, int *exist, int *conflict) {
    int pos = -1;
    int base = NODE_BASE(d, s);

    if (s == 0) {
        pos = NODE_BASE(d, 0) + ch;
    } else {
        if (base == 0) {
            pos = __find_pos(d, s, ch);
            if (pos > 0) {
                NODE_BASE(d, s) = pos - ch;
            }
        } else if (base > 0) {
            pos = base + ch;
        } else if (base == DAT_END_POS) {
            pos = __find_pos(d, s, ch);
            if (pos > 0) {
                NODE_BASE(d, s) = ch - pos;
            }
            return pos;
        } else {
            // < 0
            pos = -1 * base + ch;
        }
    }
    
    if (pos >= d->array_len) {
        if (expand_dat_array(d, pos) < 0) {
            return -1;
        }
    }
    
    if (s == NODE_CHECK(d, pos)) {
        *exist = 1;
        return pos;
    }

    if (NODE_CHECK(d, pos) >= 0)
        *conflict = 1;

    return pos;
}

int resolve(struct dat *d, int s, unsigned char ch) {
    return 0;
}

int relocate(struct dat *d, int s, unsigned char ch) {
    return 0;
}

// insert key to dat
int insert_pattern(struct dat *d, char *key, unsigned long key_len, void *data) {
    int exist;
    int conflict;
    int i = 0, s = 0, t = 0;
    unsigned char ch;
    
    for (; i < key_len; i ++) {
        ch = *(key + i);
        if (d->nocase) TOLOWWER(ch);
        
        exist = 0;
        conflict = 0;
        t = find_pos(d, s, ch, &exist, &conflict);
        if (t < 0) {
            return -1;
        }
        if (exist == 0) {
            if (conflict) {
                t = relocate(d, s, ch);
            } else {
                del_free_node_idx(d, t);
                // base值由下一个确定
                NODE_BASE(d, t) = 0;
                NODE_CHECK(d, t) = s;
            }
        }
        s = t;
        
        if (i == key_len - 1) {
            if (NODE_BASE(d, t) > 0) {
                NODE_BASE(d, t) = -1 * NODE_BASE(d, t);
            } else if (NODE_BASE(d, t) == 0){
                NODE_BASE(d, t) = DAT_END_POS;
            }
        }
    }

    return 0;
}

// build dat
int build_dat(struct dat *d, char *pats[MAX_PAT_LEN], int pat_count) {
    int i = 0;
    int ret = 0;
    unsigned long len = 0;
    char *pattern = (char *)(*pats);

    for (; i < pat_count; i ++, pattern += MAX_PAT_LEN) {
        len = strlen(pattern);
        if (len >= MAX_PAT_LEN - 1) {
            printf("pattern [index=%d %s] too long, length=%lu\n", i, pattern, len);
            continue;
        }
        ret = insert_pattern(d, pattern, len, NULL);
        if (ret < 0) {
            printf("insert pattern index = %d %s failed\n", i, pattern);
        }
    }

    return 0;
}

// remove pattern
void remove_pattern(struct dat *d, char *key, unsigned long key_len) {
    
}

// match dat
int match_dat(struct dat *d, char *target, unsigned long targetlen, int option) {
    return 0;
}
