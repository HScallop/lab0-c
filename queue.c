#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (q) {
        INIT_LIST_HEAD(q);
    }
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *entry = NULL, *next = NULL;
    list_for_each_entry_safe (entry, next, l, list) {
        free(entry->value);
        free(entry);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add(&e->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }
    e->value = strdup(s);
    if (!e->value) {
        free(e);
        return false;
    }
    list_add_tail(&e->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&e->list);
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *e = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&e->list);
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    struct list_head *node;
    int count = 0;
    list_for_each (node, head) {
        count++;
    }
    return count;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *fast, *slow;
    fast = slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head) {
        return false;
    }
    if (list_empty(head) || list_is_singular(head)) {
        return true;
    }
    element_t *node, *safe;
    bool dup = false;
    list_for_each_entry_safe (node, safe, head, list) {
        if ((node->list.next != head) &&
            (strcmp(node->value,
                    list_entry(node->list.next, element_t, list)->value) ==
             0)) {
            list_del(&node->list);
            q_release_element(node);
            dup = true;
        } else if (dup) {
            list_del(&node->list);
            q_release_element(node);
            dup = false;
        }
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */

void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node;
    for (node = head->next; node != head && node->next != head;
         node = node->next) {
        struct list_head *tmp = node->next;
        list_del(tmp);
        list_add_tail(tmp, node);
    }
}
/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}
<<<<<<< HEAD


struct list_head *mergeTwoLists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; l1 && l2; *node = (*node)->next) {
        node = (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) < 0)
                   ? &l1
                   : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }

    *ptr = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);

    return head;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head->next) {
        return head;
    }
    struct list_head *fast = head, *slow = head, *mid;
    while (true) {
        if (!fast || !fast->next)
            break;
        fast = fast->next->next;
        slow = slow->next;
    }

    mid = slow;
    slow->prev->next = NULL;

    struct list_head *l1 = mergesort(head), *l2 = mergesort(mid);
    return mergeTwoLists(l1, l2);
}
=======
>>>>>>> 7e1ed43 (Implement function q_reverse)

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
<<<<<<< HEAD
=======
 struct list_head *mergeTwoLists(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; l1 && l2; *node = (*node)->next) {
        node = (strcmp(list_entry(l1, element_t, list)->value,
                       list_entry(l2, element_t, list)->value) < 0)
                   ? &l1
                   : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }

    *ptr = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);

    return head;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head->next) {
        return head;
    }
    struct list_head *fast = head, *slow = head, *mid;
    while (true) {
        if (!fast || !fast->next)
            break;
        fast = fast->next->next;
        slow = slow->next;
    }

    mid = slow;
    slow->prev->next = NULL;

    struct list_head *l1 = mergesort(head), *l2 = mergesort(mid);
    return mergeTwoLists(l1, l2);
}

>>>>>>> 2d4945f (Implement function q_sort)
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *node = head->next, *tmp;
    head->prev->next = NULL;
    head->next = NULL;

    node = mergesort(node);

    tmp = head;
    tmp->next = node;
    while (tmp->next) {
        tmp->next->prev = tmp;
        tmp = tmp->next;
    }
    tmp->next = head;
    head->prev = tmp;
}