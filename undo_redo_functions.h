#ifndef CHECKER_UNDO_REDO_FUNCTIONS_H
#define CHECKER_UNDO_REDO_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "commands_functions.h"

#define MAXCHAR 1000

/*
 * functie care adauga un caracter inapoi la o anumita pozitie
 * aceasta functie este apelata de undo in cazul in care
 * este data comanda dupa backspace
 * list = list = lista in care fiecare nod este un caracter
 * str = caracterul vechi
 * poz = pozitia unde se gasea acel caracter
 */
void add_character_back(struct LinkedList *list, char str[100], int poz) {
    if(poz == 1) {
        struct Node *node = malloc(sizeof(struct Node));
        node->next = list->head;
        node->poz = poz;
        node->data = str[0];
        list->head = node;
        list->size++;
    } else {
        struct Node *finder = list->curr;
        if(finder == list->tail) {
            struct Node *node = malloc(sizeof(struct Node));
            node->prev = list->tail;
            node->poz = poz;
            node->data = str[0];
            list->tail = node;
            list->size++;
        } else {
            struct Node *node = malloc(sizeof(struct Node));
            node->poz = poz;
            node->data = str[0];
            node->prev = finder;
            node->next = finder->next;
            finder->next = node;
            node->next->prev = node;
            list->size++;
        }
    }
}

/*
 * functie care adauga o intreaga linie inapoi, aceasta este apelata la undo
 * in cazul in care o linie a fost stearsa
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void add_line_back(struct LinkedList *list, struct Stack *stack) {
    if(list->curr == list->tail) {
        for(int i = 1; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[i];
            node->poz = list->size + 1;
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
            list->size++;
        }
        struct Node *node = malloc(sizeof(struct Node));
        node->data = '\n';
        node->poz = list->size + 1;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->size++;
    } else if(list->curr == list->head) {
        struct Node *node = malloc(sizeof(struct Node));
        node->data = '\n';
        node->poz = stack->top->poz + strlen(stack->top->data) - 1;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
        list->size++;
        for(int i = 1; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[strlen(stack->top->data) - i];
            node->poz = stack->top->poz + strlen(stack->top->data) - i - 1;
            node->next = list->head;
            list->head->prev = node;
            list->head = node;
            list->size++;
        }
    } else {
        list->curr = list->curr->prev;
        for(int i = 1; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[i];
            node->poz = stack->top->poz + i;
            node->prev = list->curr;
            node->next = list->curr->next;
            list->curr->next = node;
            node->next->prev = node;
            list->curr = node;
            list->size++;
        }
        struct Node *node = malloc(sizeof(struct Node));
        node->data = '\n';
        node->poz = stack->top->poz + strlen(stack->top->data);
        node->prev = list->curr;
        node->next = list->curr->next;
        list->curr->next = node;
        node->next->prev = node;
        list->curr = node;
        list->size++;
    }
}

/*
 * functie care adauga un caracter inapoi in lista
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void add_back_to_list(struct LinkedList *list, struct Stack *stack) {
    if(stack->top->poz == 1) {
        for(int i = 1 ; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[strlen(stack->top->data) - i];
            node->poz = stack->top->poz + strlen(stack->top->data) - i - 1;
            list->head->prev = node;
            node->next = list->head;
            list->head = node;
            list->size++;
        }
    } else if(list->curr == list->tail) {
        list->curr = list->curr->prev;
        for(int i = 1 ; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[i];
            node->poz = stack->top->poz + i;
            node->prev = list->curr;
            node->next = list->curr->next;
            list->curr->next = node;
            node->next->prev = node;
            list->curr = node;
            list->size++;
        }
    } else {
        for(int i = 1 ; i < strlen(stack->top->data); i++) {
            struct Node *node = malloc(sizeof(struct Node));
            node->data = stack->top->data[i];
            node->poz = stack->top->poz + i;
            node->prev = list->curr;
            node->next = list->curr->next;
            list->curr->next = node;
            node->next->prev = node;
            list->curr = node;
            list->size++;
        }
    }
}

/*
 * functie care sterge ultima bucata de text adaugat in lista
 * in cazul in care nu a fost salvat
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 * stack_undo = stiva unde se patreaza comenzile dupa undo
 * pentru cazul in care se da redo
 */
void delete_recent_text(struct LinkedList *list, struct Stack *stack_undo,
        struct Stack *stack) {
    char str[MAXCHAR];
    int count = 0;
    while(list->tail->poz != stack->top->poz) {
        list->tail = list->tail->prev;
        str[count] = list->tail->next->data;
        free(list->tail->next);
        list->size--;
        count++;
    }
    list->curr = list->tail;
    if(stack_undo->top->data[0] == '-' && stack_undo->top->data[1] == '\0') {
        for(int i = 0 ; i < count; i++) {
            strncat(stack_undo->top->data, &str[count - i - 1], 1);
        }
    }
}

/*
 * functie care adauga dupa redo inapoi portiunea de text ce a fost stearsa
 * list = list = lista in care fiecare nod este un caracter
 * stack_undo = stiva unde se patreaza comenzile dupa undo
 * pentru cazul in care se da redo
 */
void add_recent_text(struct LinkedList *list, struct Stack *stack_undo) {
    for(int i = 1 ; i < strlen(stack_undo->top->data); i++) {
        struct Node *node = malloc(sizeof(struct Node));
        node->data = stack_undo->top->data[i];
        node->poz = list->size + 1;
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->size++;
    }
    list->curr = list->tail;
}

/*
 * functie pentru undo, in aceasta se memoreaza in stiva_undo comenzile
 * care ca au primit undo, si se intoarce lista inapoi la stadiul de dinainte de
 * comanda respectiva
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 * stack_undo = stiva unde se patreaza comenzile dupa undo
 * pentru cazul in care se da redo
 */
void undo_the_last_command(struct LinkedList *list, struct Stack *stack,
        struct Stack *stack_undo) {
    if(stack->size != 0) {
        //daca comanda cea mai recenta este save atunci se da direct return
        if(stack->top->command[0] == 's') {
            return;
        }
        //daca este backspace atunci se readuce utimul caracter sters inapoi
        if(stack->top->command[0] == 'b') {
            add_character_back(list, stack->top->data, stack->top->poz);
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            pop(stack);
            return;
        }
        //daca ultima comanda este delete line atunci se rescrie linia stearsa
        if(stack->top->command[0] == 'd' && stack->top->command[1] == 'l') {
            add_line_back(list, stack);
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            pop(stack);
            return;
        }
        //daca ultima comanda este goto line atunci cursorul se muta
        //pe pozitia cea mai recenta
        if(stack->top->command[0] == 'g' && stack->top->command[1] == 'l') {
            struct Node *node = list->head;
            for(int i = 0 ; i < list->size; i++) {
                if(node->poz == stack->top->poz) {
                    list->curr = node;
                }
                node = node->next;
            }
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            pop(stack);
            return;
        }
        //daca ultima comanda este goto character atunci cursorul se muta
        //pe pozitia cea mai recenta
        if(stack->top->command[0] == 'g' && stack->top->command[1] == 'c') {
            struct Node *node = list->head;
            for(int i = 0 ; i < list->size; i++) {
                if(node->poz == stack->top->poz) {
                    list->curr = node;
                }
                node = node->next;
            }
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            pop(stack);
            return;
        }
        //daca comanda este delte atunci se rescriu caracterele sterse
        if(stack->top->command[0] == 'd' &&
        (stack->top->command[1] == '\n' ||stack->top->command[1] == ' ')) {
            add_back_to_list(list, stack);
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            pop(stack);
            return;
        }
        //daca comanda este text atunci se sterge textul cel mai recent scris
        if(stack->top->command[0] == 't' && stack->top->command[1] == 'e') {
            add_to_stack(stack_undo, stack->top->command);
            stack_undo->top->poz = stack->top->poz;
            strcpy(stack_undo->top->data, stack->top->data);
            delete_recent_text(list, stack_undo,  stack);
            pop(stack);
            return;
        }
        //dupa fiecare comanda undo se da pop la stiva de comanzi
    }
}

/*
 * functie pentru redo acesta se poate da doar imediat dupa undo,
 * intru-cat doar atunci
 * stiva_undo are comenzi in interior
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 * stack_undo = stiva unde se patreaza comenzile dupa undo
 * pentru cazul in care se da redo
 */
void redo_the_last_command(struct LinkedList *list, struct Stack *stack,
        struct Stack *stack_undo) {
    //daca se gaseste in stiva comanda text, este rescris textul proaspat sters
    if(stack_undo->top->command[0] == 't' && stack_undo->top->command[1] == 'e') {
        add_to_stack(stack, stack_undo->top->command);
        stack->top->poz = stack_undo->top->poz;
        strcpy(stack->top->data, stack_undo->top->data);
        add_recent_text(list, stack_undo);
        pop(stack_undo);
        return;
    }
}

#endif //CHECKER_UNDO_REDO_FUNCTIONS_H
