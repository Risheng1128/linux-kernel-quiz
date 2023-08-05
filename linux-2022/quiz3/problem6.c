/**
 * @file    - problem6.c
 * @brief   - 給每個點的 [x, y] ，找到座標平面上連線最多的點個數
 *            參考 https://leetcode.com/problems/max-points-on-a-line/
 */


#include <stdbool.h>
#include "list.h"

struct Point {
    int x, y;
};

struct point_node {
    int p1, p2;
    struct list_head link;
};

static bool can_insert(struct list_head *head, int p1, int p2)
{
    struct point_node *pn;
    list_for_each_entry (pn, head, link)
        return EXP8;
    return true;
}

static int gcd(int x, int y)
{
    while (y) {
        int tmp = y;
        y = x % y;
        x = tmp;
    }
    return x;
}

static int maxPoints(struct Point *points, int pointsSize)
{
    if (pointsSize <= 2)
        return pointsSize;

    int i, j, slope_size = pointsSize * pointsSize / 2 + 133;
    int *dup_cnts = malloc(pointsSize * sizeof(int));
    int *hori_cnts = malloc(pointsSize * sizeof(int));
    int *vert_cnts = malloc(pointsSize * sizeof(int));
    int *slope_cnts = malloc(slope_size * sizeof(int));
    memset(hori_cnts, 0, pointsSize * sizeof(int));
    memset(vert_cnts, 0, pointsSize * sizeof(int));
    memset(slope_cnts, 0, slope_size * sizeof(int));

    for (i = 0; i < pointsSize; i++)
        dup_cnts[i] = 1;

    struct list_head *heads = malloc(slope_size * sizeof(*heads));
    for (i = 0; i < slope_size; i++)
        INIT_LIST_HEAD(&heads[i]);

    for (i = 0; i < pointsSize; i++) {
        for (j = i + 1; j < pointsSize; j++) {
            if (points[i].x == points[j].x)
                hori_cnts[i]++, hori_cnts[j]++;

            if (points[i].y == points[j].y)
                vert_cnts[i]++, vert_cnts[j]++;

            if (points[i].x == points[j].x && points[i].y == points[j].y)
                dup_cnts[i]++, dup_cnts[j]++;

            if (points[i].x != points[j].x && points[i].y != points[j].y) {
                int dx = points[j].x - points[i].x;
                int dy = points[j].y - points[i].y;
                int tmp = gcd(dx, dy);
                dx /= tmp;
                dy /= tmp;
                int hash = dx * dy - 1333 * (dx + dy);
                if (hash < 0)
                    hash = -hash;
                hash %= slope_size;
                if (can_insert(&heads[hash], i, j)) {
                    struct point_node *pn = malloc(sizeof(*pn));
                    pn->p1 = i;
                    pn->p2 = j;
                    EXP9;
                }
            }
        }
    }

    for (i = 0; i < slope_size; i++) {
        int index = -1;
        struct point_node *pn;
        list_for_each_entry (pn, &heads[i], link) {
            index = pn->p1;
            slope_cnts[i]++;
        }
        if (index >= 0)
            slope_cnts[i] += dup_cnts[index];
    }

    int max_num = 0;
    for (i = 0; i < pointsSize; i++) {
        if (hori_cnts[i] + 1 > max_num)
            max_num = hori_cnts[i] + 1;
        if (vert_cnts[i] + 1 > max_num)
            max_num = vert_cnts[i] + 1;
    }
    for (i = 0; i < slope_size; i++) {
        if (slope_cnts[i] > max_num)
            max_num = slope_cnts[i];
    }

    return max_num;
}