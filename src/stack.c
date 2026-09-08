#include <stdlib.h>
#include <assert.h>

#include "stack.h"

void push(Stack *s, void *value) {
    void **tmp = realloc(s->data, (s->size + 1) * sizeof(value));
    
    if (tmp == NULL) return;

    s->data = tmp;
    s->data[s->size] = value;
    s->size++; 
}

void *pop(Stack *s) {
    if (s->size == 0) return NULL;
    
    void* res = s->data[s->size - 1];
    s->size--;

    return res;
}

void *peek(Stack *s) {
    if (s->size == 0) return NULL;

    return s->data[s->size -1];
}

int isEmpty(const Stack *s) {
    return s->size == 0 ? 1 : 0;
}

int size(const Stack *s) {
    return s->size;
}

void destroyStack(Stack *s) {
    for (int i = 0; i < s->size; i++) {
       free(s->data[i]);
    }

    free(s->data);
    s->data = NULL;
    s->size = 0;
}
