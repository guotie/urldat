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

#define DEFAULT_DAT_LEN     512
#define MAX_DAT_LEN         (1024 * 2048)
#define DAT_END_POS         -2147483648

#define FIRST_FREE_INDEX(d) ((d)->nodes[0].check)

#define NODE_BASE(d, i)  ((d)->nodes[i].base)
#define NODE_CHECK(d, i) ((d)->nodes[i].check)
#define NODE_DATA(d, i)  ((d)->nodes[i].data)
#define NODE_ATTR(d, i)  ((d)->nodes[i].attr)

#define TOLOWWER(c) do{ if((c)>='A' && (c)<='Z') (c) = (c) + 'a' - 'A'; }while(0)

void dump_dat(struct dat *d) {
    printf("DAT:\n    array_len=%d idle_count=%d nocase=%d\n", d->array_len, d->idle_count, d->nocase);
}

// 初始化数组链表
// 设置idle_count
void init_dat_nodes(struct dat *d) {
    int i;
    
    // position 0 是特殊位置，不是数组链表的一部分，相当于固定的链表头
    NODE_BASE(d, 0) = 0;
    NODE_CHECK(d, 0) = 1;
    NODE_DATA(d, 0) = NULL;

    for(i = 1; i < d->array_len; i ++) {
        NODE_BASE(d, i) = -(i - 1);
        NODE_CHECK(d, i) = -(i + 1);
        NODE_DATA(d, i) = NULL;
        NODE_ATTR(d, i) = 0;
    }
    NODE_BASE(d, 1) = -(i - 1);
    NODE_CHECK(d, i-1) = -1;

    d->idle_count = i - 1;
}

// expand dat array
int expand_dat_array(struct dat *d, int new_length) {
    int i;
    struct dat_node *nodes;

    if (new_length == 0) {
        new_length = d->array_len / 2 + d->array_len;
    } else if (new_length <= d->array_len) {
        new_length = d->array_len + 1;
    }
    
    new_length = (new_length + DEFAULT_DAT_LEN) & (~(DEFAULT_DAT_LEN-1));
    if (new_length > MAX_DAT_LEN) {
        return -1;
    }

    nodes = malloc(sizeof(struct dat_node) * new_length);
    if (!nodes) {
        return -1;
    }

    //printf("expand dat array from %d to %d 0x%lx d->nodes=0x%lx\n", d->array_len, new_length, (unsigned long)nodes, (unsigned long)d->nodes);

    // 复制原来nodes的数据
    //memset(nodes, 0, sizeof(struct dat_node) * new_length);
    memcpy(nodes, d->nodes, sizeof(struct dat_node) * d->array_len);
    
    // 初始化新增节点
    for (i = d->array_len; i < new_length; i ++) {
        nodes[i].base = -(i - 1);
        nodes[i].check = -(i + 1);
        nodes[i].data = NULL;
        nodes[i].attr = 0;
    }
    
    if (d->nodes[0].check == 0) {
        // 没有空余节点
        nodes[0].check = d->array_len;
        nodes[d->array_len].base = -(i-1);
        nodes[i-1].check = -d->array_len;
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

        NODE_BASE(d, -1 * c) = b;
        NODE_CHECK(d, -1 * b) = c;
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
        t = first = FIRST_FREE_INDEX(d);

        if (idx > first) {
            //for(t = -1 * NODE_CHECK(d, first); t != first && t < idx;) {
            //    t = -1 * NODE_CHECK(d, t);
            //}
            for (t = idx; t < d->array_len; t ++) {
                if (NODE_CHECK(d, t) < 0) {
                    break;
                }
            }
            if (t == d->array_len)
                t = first;
        }
        
        if (t == idx)
            assert(t != idx);

        NODE_BASE(d, idx) = NODE_BASE(d, t);
        NODE_CHECK(d, -1 * NODE_BASE(d, t)) = -1 * idx;
        
        NODE_BASE(d, t) = -1 * idx;
        NODE_CHECK(d, idx) = -1 * t;
        
        if (idx < FIRST_FREE_INDEX(d)) {
            FIRST_FREE_INDEX(d) = idx;
        }
    }

    NODE_DATA(d, idx) = NULL;
    NODE_ATTR(d, idx) = 0;

    d->idle_count ++;
    return;
}

int __find_pos(struct dat *d, int s, unsigned char ch) {
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

int find_pos(struct dat *d, int s, unsigned char ch, int *exist, int *conflict) {
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

int find_newbase(struct dat *d, int s, unsigned char *ca, int ca_count) {
    int nbase;
    int pos, start;
    int npos;
    int found;
    unsigned char ch;

    if (d->idle_count == 0) {
        if (expand_dat_array(d, 0) < 0) {
            return -1;
        }
    }

    // pos, next pos
    pos = start = FIRST_FREE_INDEX(d);
    npos = -1 * NODE_CHECK(d, pos);
    for (; ; pos = npos) {
        npos = -1 * NODE_CHECK(d, pos);
        if (start == npos) {
            if (expand_dat_array(d, 0) < 0) {
                return -1;
            }
        }
        if (pos <=  256) {
            continue;
        }
        found = 1;
        nbase = pos - ca[0];
        
        for (int i = 1; i < ca_count; i ++) {
            ch = ca[i];
            if (nbase + ch >= d->array_len) {
                if (expand_dat_array(d, nbase + ch) < 0) {
                    return -1;
                }
            }
            if (NODE_CHECK(d, nbase + ch) >= 0) {
                found = 0;
                break;
            }
        }
        if (found)
            return nbase;
    }

    return -1;
}

// just return state s has how many next states
int next_states_count(struct dat *d, int s) {
    unsigned char i;
    int cnt = 0;
    int base = NODE_BASE(d, s);
    
    if (base == DAT_END_POS) {
        return 0;
    }
    if (base < 0)
        base = -1 *base;

    for (i = 1; i; i ++) {
        if ((base + i) >= d->array_len )
            break;
        
        if (NODE_CHECK(d, base + i) == s)
            cnt ++;
    }

    return cnt;

}

int next_states(struct dat *d, int s, unsigned char *ca, unsigned long size) {
    unsigned char i;
    int cnt;
    int base = NODE_BASE(d, s);

    if (base == DAT_END_POS) {
        assert (base != DAT_END_POS);
    }
    if (base < 0)
        base = -1 *base;

    memset(ca, 0, size);
    for (i = 1, cnt = 0; i; i ++) {
        if ((base + i) >= d->array_len )
            break;

        if (NODE_CHECK(d, base + i) == s) {
            ca[cnt] = i;
            cnt ++;
        }
    }

    return cnt;
}

void relocate(struct dat *d, int s, int nbase, unsigned char *ca, int ca_count) {
    int i, j;
    int obase = NODE_BASE(d, s);
    int opos, npos;
    int st_count;
    unsigned char ch;
    unsigned char states[256];
    
    //printf("relocate: obase=%d nbase=%d ca=%s\n", obase, nbase, ca);
    
    for (i = 0; i < ca_count; i ++) {
        ch = ca[i];
        npos = nbase + ch;
        del_free_node_idx(d, npos);
        
        if (i == ca_count - 1) {
            NODE_CHECK(d, npos) = s;
            NODE_BASE(d, npos) = 0;
            break;
        }
        
        opos = obase + ch;
        if (obase < 0) opos = -obase + ch;
        
        NODE_BASE(d, npos) = NODE_BASE(d, opos);
        NODE_DATA(d, npos) = NODE_DATA(d, opos);
        NODE_ATTR(d, npos) = NODE_ATTR(d, opos);
        
        NODE_CHECK(d, npos) = s;
        
        // 下一位置的check值更新
        if (NODE_BASE(d, opos) != DAT_END_POS) {
            st_count = next_states(d, opos, states, sizeof(states));
            int opos_base = NODE_BASE(d, opos);
            for (j = 0; j < st_count; j ++) {
                if (NODE_BASE(d, opos) > 0) {
                    NODE_CHECK(d, opos_base + states[j]) = npos;
                } else {
                    NODE_CHECK(d, -opos_base + states[j]) = npos;
                }
            }
        }
        
        add_free_node_idx(d, opos);
    }

    if (obase < 0)
        NODE_BASE(d, s) = -nbase;
    else
        NODE_BASE(d, s) = nbase;
}

int resolve(struct dat *d, int s, unsigned char ch) {
    int cnt;
    int nbase;
    unsigned char ca[256];
    
    cnt = next_states(d, s, ca, sizeof(ca));
    ca[cnt++] = ch;
    
    nbase = find_newbase(d, s, ca, cnt);
    if (nbase < 0) {
        return -1;
    }

    relocate(d, s, nbase, ca, cnt);

    return nbase + ch;
}

// insert key to dat
int insert_pattern(struct dat *d, unsigned char *key, unsigned long key_len, unsigned long attr, void *data) {
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
                t = resolve(d, s, ch);
                if (t < 0)
                    return -1;
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

    NODE_ATTR(d, t) = attr;
    NODE_DATA(d, t) = data;

    return 0;
}

// build dat
int build_dat(struct dat *d, unsigned char *pats[MAX_PAT_LEN], int pat_count) {
    int i = 0;
    int ret = 0;
    unsigned long len = 0;
    unsigned char *pattern = (unsigned char *)(pats);

    for (; i < pat_count; i ++, pattern += MAX_PAT_LEN) {
        len = strlen((char *)pattern);
        if (len >= MAX_PAT_LEN - 1) {
            printf("pattern [index=%d %s] too long, length=%lu\n", i, pattern, len);
            continue;
        }
        ret = insert_pattern(d, pattern, len, 0, NULL);
        if (ret < 0) {
            printf("insert pattern index = %d %s failed\n", i, pattern);
        }
    }

    return 0;
}

// remove pattern
int remove_pattern(struct dat *d, unsigned char *key, unsigned long key_len, void (*free_fn)(void *)) {
    void *data = NULL;
    unsigned char ch;
    int base;
    int i = 0, s = 0, t;
    int nxt_cnt;
    
    if (key_len <= 0)
        return -1;
    
    // check if key in dat
    
    // s为前一个节点， t为后一个节点
    do {
        ch = *(key + i);
        if(d->nocase)
            TOLOWWER(ch);
        
        base = NODE_BASE(d, s);
        if (base >= 0) {
            t = base + ch;
        } else {
            if (base == DAT_END_POS)
                return 0;
            t = -base + ch;
        }

        assert(t > 0);
        if (t >= d->array_len) {
            return -2;
        }
        
        // 未找到
        if (NODE_CHECK(d, t) != s) {
            return -3;
        }

        i ++;
        s = t;
    } while (i < key_len);

    // 从后往前依次删除节点
    i --;
    
    // 处理最后一个节点
    base = NODE_BASE(d, t);
    if (base > 0) {
        // dat模式的一个子串
        // printf("pattern %s %lu is not an exact pattern in dat.\n", key, key_len);
        return 0;
    }
    s = NODE_CHECK(d, t);
    assert(base != 0);
    
    data = NODE_DATA(d, t);
    if (data && free_fn)
        free_fn(data);
    
    NODE_ATTR(d, t) = 0;
    if (base == DAT_END_POS)
        add_free_node_idx(d, t);
    else {
        // 该key是其他模式的子串，不能继续删除
        NODE_BASE(d, t) = -base;
        return 0;
    }
    i--;
    while(i >= 0) {
        t = s;
        assert(t > 0);
        nxt_cnt = next_states_count(d, t);
        base = NODE_BASE(d, t);
        
        // 该节点后有其他模式
        if (base < 0) {
            if ( nxt_cnt == 0) {
                // printf("set pos %d to DAT_END_POS\n", t);
                NODE_BASE(d, t) = DAT_END_POS;
            }
            return 0;
        }
        if (nxt_cnt > 0) {
            return 0;
        }
        // 删除节点
        s = NODE_CHECK(d, t);
        add_free_node_idx(d, t);
    
        i --;
    }

    return 0;
}

// match dat
// @param: option:
//             0: exact math
//             1: max length match, and the match should has (attr & DAT_ATTR_WILDCAST) > 0
//             2: shortest match
//
// @return: found:
//          0 not match
//          1 has exact match
//          2 has match
//          3 shortest match
// @return:
//          match data
void * match_dat(struct dat *d, unsigned char *target, unsigned long targetlen, int option, int *found) {
    void *data = NULL;
    unsigned long attr;
    unsigned char ch;
    int i = 0, s = 0, t = 0;
    int base;

    if (targetlen <= 0) {
        return NULL;
    }

    *found = 0;
    ch = *target;
    if (d->nocase)
        TOLOWWER(ch);
    t = d->nodes[0].base + ch;

    do {
        if (t >= d->array_len) {
            break;
        }
        
        if (NODE_CHECK(d, t) != s) {
            // 未找到
            break;
        }

        base = NODE_BASE(d, t);
        assert(base != 0);
        if (base < 0) {
            data = NODE_DATA(d, t);
            attr = NODE_ATTR(d, t);
            // 到了结束位置
            if (i == targetlen - 1)
                *found = 1;
            else {
                if (option != 0) {
                    if (option == 1 && attr & DAT_ATTR_WILDCAST)
                        *found = 2;
                    if (option == 2) // 最短match
                        return data;
                }
            }

            if (base == DAT_END_POS)
                return data;
        }

        i ++;
        if (i >= targetlen)
            break;

        s = t;
        ch = *(target + i);
        if (d->nocase) TOLOWWER(ch);
        
        if (base > 0) {
            t = base + ch;
        } else {
            t = -base + ch;
        }
    } while (i < targetlen);

    return data;
}


// init dat base array & check array
struct dat * create_dat(int array_len, int nocase) {
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
    
    d->nocase = nocase;
    d->array_len = array_len;
    d->nodes = malloc(sizeof(struct dat_node) * array_len);
    if (!d->nodes) {
        free(d);
        return NULL;
    }
    
    init_dat_nodes(d);
    
    d->insert = insert_pattern;
    d->remove = remove_pattern;
    d->match = match_dat;

    return d;
}

void destroy_dat(struct dat *d, void (*free_fn)(void *)) {
    int i;

    if (!d)
        return;

    for (i = 0; i < d->array_len; i ++) {
        if (NODE_BASE(d, i) < 0) {
            if(free_fn && NODE_DATA(d, i)) {
                free_fn(NODE_DATA(d, i));
            }
        }
    }

    free(d->nodes);
    free(d);
    return;
}
