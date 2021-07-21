#ifndef CHECKER_STRUCT_H
#define CHECKER_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 Structura pentru nodul listei
 */
struct Node {
    char data;
    int poz;
    struct Node* prev;
    struct Node* next;
};

/*
 Structura pentru lista
 */
struct LinkedList {
    struct Node* head;
    struct Node* tail;
    struct Node* curr;
    int size;
};

/*
 Structura pentru nodul stivei
 */
struct Nodestack {
    char data[100];
    int poz;
    char command[10];
    struct Nodestack *next;
};

/*
 Structura pentru stiva de comenzi
 */
struct Stack {
    struct Nodestack *top;
    int size;
};

/*
 * functie pentru eliberarea memoriei listei
 * list = lista in care fiecare nod este un caracter
 */
void free_memory(struct LinkedList *list) {
    if(list->size != 0) {
        struct Node *node = list->head;
        for(int i = 0 ; i < list->size - 1; i++) {
            node = node->next;
            free(node->prev);
        }
        free(node);
    }
}

/*
 * functie pentru eliberarea memoriei stivei de comenzi
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void free_stack(struct Stack *stack) {
    if(stack->size != 0) {
        struct Nodestack *node = stack->top;
        for(int i = 0 ; i < stack->size; i++) {
            stack->top = node->next;
            free(node);
            node = stack->top;
        }
    }
}

/*
 * functie pentru adaugarea unui element in stiva
 * node = un nod ce contine o comanda ce urmeaza sa fie adaugata
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void push(struct Nodestack *node, struct Stack *stack) {
    if(stack->size == 0) {
        stack->top = node;
    } else {
        node->next = stack->top;
        stack->top = node;
    }
    stack->size++;
}

/*
 * functie pentru scoaterea primului element din stiva
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void pop(struct Stack *stack) {
    if(stack->size == 0) {
        return;
    } else {
        struct Nodestack *node = stack->top;
        stack->top = stack->top->next;
        free(node);
        stack->size--;
    }
}

#endif //CHECKER_STRUCT_H
