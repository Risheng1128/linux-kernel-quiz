/**
 * @file    problem2.c
 * @brief   實作 82. Remove Duplicates from Sorted List II
 * (https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/)
 */

#include <stdio.h>

struct ListNode {
    int val;
    struct ListNode *next;
};


struct ListNode *deleteDuplicates(struct ListNode *head)
{
    if (!head)
        return NULL;

    if (head->next && head->val == head->next->val) {
        /* Remove all duplicate numbers */
        while (head->next && head->val == head->next->val)
            head = head->next;
        return deleteDuplicates(head->next);
    }

    head->next = deleteDuplicates(head->next);
    return head;
}

int main(void)
{
    return 0;
}