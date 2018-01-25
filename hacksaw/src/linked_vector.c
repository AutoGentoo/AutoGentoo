//
// Created by atuser on 1/6/18.
//

#include <stdlib.h>
#include <autogentoo/hacksaw/tools.h>

LinkedNode* _linked_node_new () {
    LinkedNode* out = malloc(sizeof(LinkedNode));
    out->data = NULL;
    out->next = NULL;
    out->prev = NULL;
    
    return out;
}

LinkedNode** linked_vector_new () {
    LinkedNode** out = malloc (sizeof(LinkedNode*));
    *out = NULL;
    
    return out;
}

LinkedNode* linked_vector_get (LinkedNode** head_ref, int index) {
    LinkedNode* current_ref = *head_ref;
    
    int i;
    for (i = 0; i != index && current_ref; i++) {
        current_ref = current_ref->next;
    }
    
    return current_ref;
}

LinkedNode* linked_vector_append (LinkedNode** prev_ref, void* data) {
    LinkedNode* new_node = _linked_node_new ();
    
    new_node->data = data;
    new_node->prev = *prev_ref;
    new_node->next = (*prev_ref)->next;
    
    if ((*prev_ref)->next)
        (*prev_ref)->next->prev = new_node;
    (*prev_ref)->next = new_node;
    
    return new_node;
}

LinkedNode* linked_vector_prepend (LinkedNode** next_ref, void* data) {
    LinkedNode* new_node = _linked_node_new ();
    
    new_node->data = data;
    new_node->next = *next_ref;
    new_node->prev = (*next_ref)->prev;
    
    if ((*next_ref)->prev)
        (*next_ref)->prev->next = new_node;
    
    (*next_ref)->prev = new_node;
    
    return new_node;
}

void linked_node_free (LinkedNode* node, int free_data) {
    if (node->next)
        node->next->prev = NULL;
    if (node->prev)
        node->prev->next = NULL;
    
    if (free_data)
        free (node->data);
    
    free (node);
}

void linked_vector_free (LinkedNode** head_ref, int free_data) {
    LinkedNode* current_ref = *head_ref;
    LinkedNode* buf_ref = NULL;
    while (current_ref) {
        buf_ref = current_ref->next;
        linked_node_free (current_ref, free_data);
        current_ref = buf_ref;
    }
}