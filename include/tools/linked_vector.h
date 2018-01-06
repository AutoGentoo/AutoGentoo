//
// Created by atuser on 1/6/18.
//

#ifndef HACKSAW_LINKED_VECTOR_H
#define HACKSAW_LINKED_VECTOR_H

typedef struct __LinkedNode LinkedNode;

struct __LinkedNode {
    void* data;
    
    LinkedNode* prev;
    LinkedNode* next;
};

LinkedNode** linked_vector_new ();
LinkedNode* linked_vector_get (LinkedNode** head_ref, int index);
LinkedNode* linked_vector_append (LinkedNode** prev_ref, void* data);
LinkedNode* linked_vector_prepend (LinkedNode** next_ref, void* data);

void linked_node_free (LinkedNode* node, int free_data);
void linked_vector_free (LinkedNode** head_ref, int free_data);

#endif //HACKSAW_LINKED_VECTOR_H
