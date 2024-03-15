#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head == NULL)
        return;

    struct list_head *current, *next_node;
    list_for_each_safe (current, next_node, head) {
        element_t *node = list_entry(current, element_t, list);
        list_del(current);
        q_release_element(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (new_element == NULL)
        return false;

    new_element->value = strdup(s);
    if (new_element->value == NULL) {
        free(new_element);
        return false;
    }
    list_add(&new_element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *new_element = (element_t *) malloc(sizeof(element_t));
    if (new_element == NULL)
        return false;

    new_element->value = strdup(s);
    if (new_element->value == NULL) {
        free(new_element);
        return false;
    }
    list_add_tail(&new_element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;


    element_t *first_element = list_first_entry(head, element_t, list);
    if (first_element->value) {
        strncpy(sp, first_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&first_element->list);
    return first_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    element_t *last_element = list_last_entry(head, element_t, list);
    if (last_element->value) {
        strncpy(sp, last_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&last_element->list);
    return last_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return 0;

    int count = 0;
    struct list_head *current;
    list_for_each (current, head) {
        count++;
    }
    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;

    struct list_head *slow = head;
    struct list_head *fast;
    list_for_each (fast, head) {
        fast = fast->next;
        slow = slow->next;
        if (fast == head)
            break;
    }
    list_del(slow);

    element_t *deleted_element = list_entry(slow, element_t, list);
    q_release_element(deleted_element);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return true;

    struct list_head *current, *next_node;
    bool last_check = false;
    list_for_each_safe (current, next_node, head) {
        element_t *nodeA = list_entry(current, element_t, list);
        element_t *nodeB = list_entry(next_node, element_t, list);

        if (next_node != head && !strcmp(nodeA->value, nodeB->value)) {
            last_check = true;
            list_del(current);
            q_release_element(nodeA);
        } else if (last_check) {
            last_check = false;
            list_del(current);
            q_release_element(nodeA);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *current, *next_node;
    list_for_each_safe (current, next_node, head) {
        current->next = current->prev;
        current->prev = next_node;
    }
    current->next = current->prev;
    current->prev = next_node;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (head == NULL || list_empty(head) || list_is_singular(head) || k <= 1)
        return;

    struct list_head *current, *next_node, start, *end;
    int count = 0;
    INIT_LIST_HEAD(&start);
    end = head;

    list_for_each_safe (current, next_node, head) {
        count++;
        if (count == k) {
            list_cut_position(&start, end, current);
            q_reverse(&start);
            list_splice_init(&start, end);
            count = 0;
            end = next_node->prev;
        }
    }
}

void merge(struct list_head *left,
           struct list_head *right,
           struct list_head *head)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *l_node = list_entry(left->next, element_t, list);
        element_t *r_node = list_entry(right->next, element_t, list);

        if (strcmp(l_node->value, r_node->value) <= 0)
            list_move_tail(left->next, head);
        else
            list_move_tail(right->next, head);
    }

    if (!list_empty(left))
        list_splice_tail_init(left, head);
    else
        list_splice_tail_init(right, head);
}

void merge_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *fast;
    struct list_head *slow = head;
    list_for_each (fast, head) {
        fast = fast->next;
        slow = slow->next;
        if (fast == head)
            break;
    }

    LIST_HEAD(left);
    LIST_HEAD(right);

    list_splice_tail_init(head, &right);
    list_cut_position(&left, &right, slow);

    merge_sort(&left);
    merge_sort(&right);

    merge(&left, &right, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    merge_sort(head);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return 0;

    element_t *current, *next_node;
    list_for_each_entry_safe (current, next_node, head, list) {
        element_t *scan = list_entry(&next_node->list, element_t, list);
        while (&scan->list != head) {
            if (strcmp(scan->value, current->value) < 0) {
                list_del(&current->list);
                q_release_element(current);
                break;
            }
            scan = list_entry(scan->list.next, element_t, list);
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return 0;

    element_t *current, *next_node;
    list_for_each_entry_safe (current, next_node, head, list) {
        element_t *scan = list_entry(&next_node->list, element_t, list);
        while (&scan->list != head) {
            if (strcmp(scan->value, current->value) > 0) {
                list_del(&current->list);
                q_release_element(current);
                break;
            }
            scan = list_entry(scan->list.next, element_t, list);
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (head == NULL || list_empty(head))
        return 0;

    queue_contex_t *m_queue = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *new_node = NULL;
    list_for_each_entry (new_node, head->next, chain) {
        if (new_node == list_entry(head, queue_contex_t, chain))
            break;
        list_splice_tail_init(new_node->q, m_queue->q);
        m_queue->size += new_node->size;
        new_node->size = 0;
    }

    q_sort(m_queue->q, descend);
    return m_queue->size;
}
