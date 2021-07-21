#ifndef CHECKER_COMMANDS_FUNCTIONS_H
#define CHECKER_COMMANDS_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

#define MAXCHAR 1000

/*
 * functie pentru stergerea ultimului caracter prin eliminarea ultimului
 * nod de dinainte de cursor
 * functia are 3 cazuri, daca trebuie sa stearga primul caracter, ultimul
 * sau un caracter din interiorul listei
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void delete_last_chr(struct LinkedList *list, struct Stack *stack) {
    struct Nodestack *node = malloc(sizeof(struct Nodestack));
    node->data[0] = list->curr->data;
    node->data[1] = '\0';
    node->poz = list->curr->poz;
    strcpy(node->command, "b\n");
    push(node, stack);
    if(list->curr == list->tail) {
        list->curr = list->curr->prev;
        free(list->curr->next);
        list->curr->next = NULL;
        list->tail = list->curr;
        list->size--;
    }
    else if(list->curr == list->head) {
        list->curr = list->curr->next;
        free(list->curr->prev);
        list->curr->prev = NULL;
        list->head = list->curr;
        list->size--;
    } else {
        list->curr = list->curr->prev;
        list->curr->next = list->curr->next->next;
        free(list->curr->next->prev);
        list->curr->next->prev = list->curr;
        list->size--;
    }
}

/*
 * functie care adauga un un nod in lista in functie de unde se
 * afla cursorul
 * list = list = lista in care fiecare nod este un caracter
 * c = caracterul ce trecuie adaugat
 */
void add_to_list(struct LinkedList *list, char c) {
    struct Node *node = malloc(sizeof(struct Node));
    node->data = c;
    node->poz = list->size + 1;
    if (list->size == 0) {
        list->head = list->tail = list->curr = node;
    } else {
        if (list->curr == list->tail) {
            list->tail->next = node;
            node->prev = list->tail;
            list->tail = node;
            list->curr = node;
        } else {
            node->prev = list->curr;
            node->next = list->curr->next;
            list->curr->next = node;
            node->next->prev = node;
            list->curr = node;
        }
    }
    list->size++;
}

/*
 * functie care salveaza fisierul, uploadand modificarile facute
 * in fisierul editor.out
 * list = list = lista in care fiecare nod este un caracter
 * out = pointer catre fiserul in care se scriu caracterele din lista
 * la save
 */
void save_file(struct LinkedList *list, FILE *out) {
    struct Node *node = list->head;
    for(int i = 0 ; i < list->size; i++) {
        fprintf(out, "%c", node->data);
        node = node->next;
    }
}

/*
 * functie care creaza un nod de stiva pe care il
 * adauga apeland functia push
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 * a = numele comenzii ce trebuie salvata in stiva de comenzi
 */
void add_to_stack(struct Stack *stack, char a[10]) {
    struct Nodestack *node = malloc(sizeof(struct Nodestack));
    strcpy(node->command, a);
    node->poz = -1;
    strcpy(node->data, "-");
    push(node, stack);
}

/*
 * functie ce muta cursorul la inceputul liniei primita ca
 * parametru
 * list = list = lista in care fiecare nod este un caracter
 * line = numarul liniei unde trebuie sa ajunga cursorul
 */
void go_to_line(struct LinkedList * list, int line) {
    int count  = 1;
    struct Node *node = list->head;
    list->curr = list->tail;
    for(int i = 0 ; i < list->size; i++) {
        if(count == line) {
            list->curr = node;
            break;
        }
        if(node->data == '\n') {
            count++;
        }
        node = node->next;
    }
}

/*
 * functie ce muta cursorul de-a lungul unei linii un numar de caractere
 * list = list = lista in care fiecare nod este un caracter
 * character = numarul de caractere pe care trebuie sa le strabata cursorul
 */
void go_to_char(struct LinkedList * list, int character) {
    for(int i = 0; i < character; i++) {
        list->curr = list->curr->next;
    }
}

/*
 * functie pentru stergerea unei linii, aceasta are si ea 3 cazuri
 * daca linia care trebuie stearsa este prima linie , daca este
 * ultima si daca este o linie oarecare
 * in aceasta functie mai sunt salvate (strncat) si caracterele sterse pentru
 * a putea fi rescrise la comenzile de undo si redo
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 */
void delete_line(struct LinkedList *list, struct Stack *stack) {
    struct Node *node = list->curr;
    //cszul cu prima linie
    if(list->curr == list->head) {
        while(node->data != '\n') {
            strncat(stack->top->data, &node->data, 1);
            node = node->next;
            free(node->prev);
            list->size--;
            list->head = node;
        }
        if(node != list->tail) {
            node = node->next;
            free(node->prev);
            list->size--;
            list->head = node;
        } else {
            free(node);
            list->size--;
        }
        list->curr = list->head;
    } else {
        while(node->data != '\n') {
            strncat(stack->top->data, &node->data, 1);
            node = node->prev;
            node->next = node->next->next;
            free(node->next->prev);
            node->next->prev = node;
            list->size--;
            node = node->next;
        }
        //cazul cu ultima linie
        if(node != list->tail) {
            node = node->prev;
            node->next = node->next->next;
            free(node->next->prev);
            node->next->prev = node;
            list->size--;
            list->curr = node->next;
        } else {
            //cazul cu o linie oarecare
            node->prev->prev->next = node;
            node = node->prev->prev;
            free(node->next->prev);
            node->next->prev = node;
            list->size--;
            list->curr = node->next;
        }
    }
}

/*
 * functie ce muta cursorul la inceputul liniei pe care
 * se afla
 * list = list = lista in care fiecare nod este un caracter
 */
void go_to_start_line(struct LinkedList *list) {
    if(list->curr != list->tail && list->curr->data == '\n') {
        list->curr = list->curr->next;
    }
    if(list->curr != list->head) {
        while (list->curr->prev->data != '\n') {
            list->curr = list->curr->prev;
        }
    }
}

/*
 * functie care sterge un caracter primit ca parametru de dupa cursor
 * list = list = lista in care fiecare nod este un caracter
 * stack = stiva de comenzi in care se memoreaza caractere
 * sterse si pozitiile lor
 * c = numarul de elemente ce trebuie sterse din lista
 */
void delete_after_current(struct LinkedList *list, struct Stack *stack, int c) {
    for(int i = 0 ; i < c; i++) {
        if(list->curr == list->head) {
            strncat(stack->top->data, &list->curr->data, 1);
            list->curr = list->curr->next;
            free(list->head);
            list->head = list->curr;
            list->size--;
        } else if(list->curr == list->tail) {
            return;
        } else {
            strncat(stack->top->data, &list->curr->data, 1);
            list->curr = list->curr->prev;
            list->curr->next = list->curr->next->next;
            free(list->curr->next->prev);
            list->curr->next->prev = list->curr;
            list->curr = list->curr->next;
            list->size--;
        }
    }
}

/*
 * functie care intoarce pozitia unde incepe un anumit cuvant
 * daca nu gaseste intorace 0
 * list = list = lista in care fiecare nod este un caracter
 * s = cuvantul care trebuie gasit in lista de caractere
 */
int find_word(struct LinkedList *list, char s[100]) {
    while(list->curr != list->tail) {
        int k = 1;
        struct Node *node = list->curr;
        for(int i = 0; i < strlen(s); i++) {
            if(node->data != s[i]) {
                k = 0;
                break;
            } else {
                node = node->next;
            }
        }
        if(k == 1) {
            return list->curr->poz;
        } else {
            list->curr = list->curr->next;
        }
    }
    return 0;
}

/*
 * functie care inlocuieste un cuvand cu un altul
 * aceasta verifica 2 cazuri, daca vechiul cuvant are mai multe
 * caractere decat noul si daca vechiul cuvant avea mai putine caractere
 * in functie de aceste 2 cazuri au fost sterse sau adaugate mai
 * multe noduri
 * list = list = lista in care fiecare nod este un caracter
 * old_word = cuvantul ce trebuie inlocuit
 * new_word = cuvantul ce trebuie sa inlocuiasca
 */
void replace_old_word(struct LinkedList *list, char old_word[100],
        char new_word[100]) {
    if(strlen(old_word) >= strlen(new_word)) {
        for(int i = 0; i < strlen(new_word); i++) {
            list->curr->data = new_word[i];
            list->curr = list->curr->next;
        }
        for(int i = 0; i < strlen(old_word) - strlen(new_word); i++) {
            if(list->curr == list->tail) {
                list->curr = list->curr->prev;
                free(list->tail);
                list->tail = list->curr;
                list->size--;
            } else {
                list->curr = list->curr->prev;
                list->curr->next = list->curr->next->next;
                free(list->curr->next->prev);
                list->curr->next->prev = list->curr;
                list->size--;
            }
        }
    } else {
        for(int i = 0; i < strlen(old_word); i++) {
            list->curr->data = new_word[i];
            list->curr = list->curr->next;
        }
        list->curr = list->curr->prev;
        for(int i = 0; i < strlen(new_word) - strlen(old_word); i++) {
            if(list->curr == list->tail) {
                struct Node *node = malloc(sizeof (struct Node));
                node->data = new_word[strlen(old_word) + i];
                node->poz = list->size + 1;
                list->curr->next = node;
                node->prev = list->curr;
                list->curr = node;
                list->tail = node;
                list->size++;
            } else {
                struct Node *node = malloc(sizeof (struct Node));
                node->data = new_word[strlen(old_word) + i];
                node->poz = list->curr->poz + 1;
                node->prev = list->curr;
                node->next = list->curr->next;
                list->curr->next = node;
                node->next->prev = node;
                list->curr = list->curr->next;
                list->size++;
            }
        }
    }
}

/*
 * functie care sterge un cuvant gasit in unul din cele 3 cazuri
 * fie la inceput, fie la final, fie intre
 * list = list = lista in care fiecare nod este un caracter
 * old_word = cuvantul ce trebuie sters
 */
void delete_old_word(struct LinkedList *list, char old_word[100]) {
    for(int i = 0; i < strlen(old_word); i++) {
        if(list->curr == list->head) {
            list->curr = list->curr->next;
            list->head = list->curr;
            free(list->curr->prev);
            list->size--;
        } else if(list->curr == list->tail) {
            list->curr = list->curr->prev;
            free(list->tail);
            list->tail = list->curr;
            list->size--;
        } else {
            list->curr = list->curr->prev;
            list->curr->next = list->curr->next->next;
            free(list->curr->next->prev);
            list->curr->next->prev = list->curr;
            list->size--;
            list->curr = list->curr->next;
        }
    }
}

#endif //CHECKER_COMMANDS_FUNCTIONS_H
