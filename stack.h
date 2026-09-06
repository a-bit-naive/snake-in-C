
#ifndef STACK_H
#define STACK_H

typedef struct {
    void **data;
    int size;
} Stack;

void push(Stack *s, void* value);

void *pop(Stack *s);

void *peek(Stack *s);

int isEmpty(const Stack *s);

int size(const Stack *s);

void destroyStack(Stack *s);

#endif
