#include <stdint.h>
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
    if (!q) {
        return NULL;
    }
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l) {
        return;
    }
    element_t *to_free, *next_to_free;
    list_for_each_entry_safe (to_free, next_to_free, l, list) {
        q_release_element(to_free);
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
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }
    new_node->value = malloc(strlen(s) + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    strncpy(new_node->value, s, strlen(s));
    new_node->value[strlen(s)] = '\0';

    list_add(&new_node->list, head);
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
    element_t *new_tail = malloc(sizeof(element_t));
    if (!new_tail) {
        return false;
    }

    new_tail->value = malloc(strlen(s) + 1);
    strncpy(new_tail->value, s, strlen(s));
    new_tail->value[strlen(s)] = '\0';

    list_add_tail(&new_tail->list, head);
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
    // Return NULL if queue is NULL or empty
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *head_element = list_first_entry(head, element_t, list);
    list_del_init(&head_element->list);

    if (sp) {
        strncpy(sp, head_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return head_element;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // Return NULL if queue is NULL or empty
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *tail_element = list_last_entry(head, element_t, list);
    list_del_init(&tail_element->list);

    if (sp) {
        strncpy(sp, tail_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return tail_element;
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
    int num = 0;
    if (!head || list_empty(head)) {
        return num;
    }

    struct list_head *running;
    list_for_each (running, head)
        num++;

    return num;
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

    struct list_head *first = head->next;
    struct list_head *second = head->next;

    while (second != head && second->next != head) {
        first = first->next;
        second = second->next->next;
    }

    list_del(first);
    q_release_element(list_entry(first, element_t, list));
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

    struct list_head *first = head;
    struct list_head *second = head;

    while (second->next != head) {
        if (list_entry(first, element_t, list) !=
            list_entry(second, element_t, list)) {
            first->next = second;
        }
        second = second->next;
    }

    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *first = head->next;

    while (first != head && first->next != head) {
        struct list_head *second = first->next;

        first->next = second->next;
        first->next->prev = first;

        second->prev = first->prev;
        second->prev->next = second;

        second->next = first;
        first->prev = second;

        first = first->next;
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
    if (!head->next) {
        return;
    }

    struct list_head *running, *safe;

    list_for_each_safe (running, safe, head) {
        running->next = running->prev;
        running->prev = safe;
    }
    running->next = running->prev;
    running->prev = safe;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
struct list_head *merge(struct list_head *first, struct list_head *second)
{
    struct list_head *new_head = NULL;
    struct list_head **ptr = &new_head;

    for (; first && second; ptr = &(*ptr)->next) {
        element_t *first_entry = list_entry(first, element_t, list);
        element_t *second_entry = list_entry(second, element_t, list);
        if (strcmp(first_entry->value, second_entry->value) < 0) {
            *ptr = first;
            first = first->next;
        } else {
            *ptr = second;
            second = second->next;
        }
    }
    *ptr = (struct list_head *) ((uintptr_t) first | (uintptr_t) second);
    return new_head;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head || !head->next) {
        return head;
    }

    struct list_head *first = head;
    struct list_head *second = head->next;

    // find middle pointer
    while (second && second->next) {
        first = first->next;
        second = second->next->next;
    }

    struct list_head *middle = first->next;
    first->next = NULL;

    return merge(merge_sort(head), merge_sort(middle));
}

void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    // become not circular
    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    // reconnect to double-linked
    struct list_head *first = head;
    struct list_head *second = head->next;
    while (second) {
        second->prev = first;
        first = first->next;
        second = second->next;
    }
    first->next = head;
    head->prev = first;
}
