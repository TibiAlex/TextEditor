#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "commands_functions.h"
#include "undo_redo_functions.h"

#define MAXCHAR 1000

/*
 * functia principala care citeste toate comenzile
 */
int main() {
    //declararea listei si a stivelor
	struct LinkedList *list = malloc(sizeof(struct LinkedList));
	list->size = 0;
	struct Stack *stack = malloc(sizeof(struct Stack));
	stack->size = 0;
    struct Stack *stack_undo = malloc(sizeof(struct Stack));
    stack_undo->size = 0;
	int is_command = 0;
	char str[MAXCHAR];
	//deschiderea fisierului editor.on
	FILE *in = fopen("editor.in","r");
    if (in == NULL){
        printf("Could not open file editor.in");
        return 1;
    }
    //este parcurs fisierul linie cu linie
    while (fgets(str, MAXCHAR, in) != NULL) {
        //se verifica daca se schimba urmatoarele imputuri
        if(strstr(str, "::i")) {

            if(is_command == 0) is_command = 1;
            else is_command = 0;
        } else {
            //se verifica daca urmeaza o comanda sau text
            if(is_command == 0) {
                add_to_stack(stack, "text");
                if(list->size == 0) {
                    stack->top->poz = 1;
                } else{
                    stack->top->poz = list->curr->poz;
                }
                for(int i = 0; i < strlen(str) - 1; i++) {
                    add_to_list(list, str[i]);
                }
                if(list->curr != list->tail) {
                    if(list->curr->next->poz < list->curr->poz) {
                        continue;
                    }
                }
                add_to_list(list, '\n');
            } else {
                //in cazul in care comanda este save se slaveazalista actuala
                //in fisierul editor.out
                if(str[0] == 's') {
                    add_to_stack(stack, str);
                    FILE *out = fopen("editor.out","w");
                    if (out == NULL){
                        printf("Could not open file editor.out");
                        return 1;
                    }
                    save_file(list, out);
                    fclose(out);
                }
                //in cazul comenzii quit se iese din program
                if(str[0] == 'q') {
                    break;
                }
                //in cazul in care comanda este backspace
                //se apeleaza fucntia delete_last_chr
                //care sterge nodul curent
                if(str[0] == 'b') {
                    if(list->curr->data == '\n') {
                        list->curr = list->curr->prev;
                    } else {
                        list->curr = list->curr->next;
                    }
                    delete_last_chr(list, stack);
                }
                //goto line muta cursorul pe alta linie
                if(str[0] == 'g' && str[1] == 'l') {
                    add_to_stack(stack, str);
                    stack->top->poz = list->curr->poz;
                    go_to_line(list, stack->top->command[3] - 48);
                    if(list->curr->poz != 1) {
                        list->curr = list->curr->prev;
                    }
                }
                //goto char muta cursorul pe alt caracter dintr-o linie
                if(str[0] == 'g' && str[1] == 'c') {
                    if(strlen(str) == 7) {
                        add_to_stack(stack, str);
                        stack->top->poz = list->curr->poz;
                        go_to_line(list, str[5] - 48);
                        go_to_char(list, str[3] - 48);
                        if(list->curr->poz != 1) {
                            list->curr = list->curr->prev;
                        }
                    } else {
                        add_to_stack(stack, str);
                        stack->top->poz = list->curr->poz;
                        go_to_char(list, str[3] - 48);
                    }
                }
                //delete line sterge o linie intreaga
                if(str[0] == 'd' && str[1] == 'l') {
                    if(strlen(str) == 5) {
                        add_to_stack(stack, str);
                        go_to_line(list, str[3] - 48);
                        if(list->curr->poz == 1) {
                            stack->top->poz = list->curr->poz;
                        } else {
                            stack->top->poz = list->curr->prev->poz;
                        }
                        if(list->curr != list->tail) {
                            delete_line(list, stack);
                        }
                        if(list->curr == list->tail) {
                            list->curr->poz = stack->top->poz;
                        }
                    } else if(strlen(str) == 6) {
                        add_to_stack(stack, str);
                        go_to_line(list, (str[3] - 48) * 10 + (str[4] - 48));
                        if(list->curr->poz == 1) {
                            stack->top->poz = list->curr->poz;
                        } else {
                            stack->top->poz = list->curr->prev->poz;
                        }
                        if(list->curr != list->tail) {
                            delete_line(list, stack);
                        }
                        if(list->curr == list->tail) {
                            list->curr->poz = stack->top->poz;
                        }
                    } else {
                         add_to_stack(stack, str);
                        go_to_start_line(list);
                         if(list->curr->poz == 1) {
                             stack->top->poz = list->curr->poz;
                         } else {
                             stack->top->poz = list->curr->prev->poz;
                         }
                         delete_line(list, stack);
                         if(list->curr == list->tail) {
                             list->curr->poz = stack->top->poz;
                         }
                    }
                }
                //delete sterge un numar de caractere pe care
                //poate sa-l primeasca din fisier, altfel se sterge un caracter
                if(str[0] == 'd' && (str[1] == ' ' || str[1] == '\n')) {
                    add_to_stack(stack, str);
                    if(list->curr == list->head) {
                        stack->top->poz = list->curr->poz;
                    } else {
                        stack->top->poz = list->curr->prev->poz;
                    }
                     if(str[1] == '\n') {
                          delete_after_current(list, stack, 1);
                     } else {
                         delete_after_current(list, stack, str[2] - 48);
                     }
                }
                /*
                 * comanda replace are 2 paramentrii
                 * pe care le scoatem folosind strtok
                 * si pe care le dam mai apoi ca paramentrii
                 * in functiile noastre
                 * comanda trebuie sa gaseasca primul parametru
                 * si sa il inlocuiasca in lista cu al 2 lea
                 */
                if(str[0] == 'r' && str[1] == 'e') {
                    char str2[100];
                    strcpy(str2, str);
                    char *old_word, *new_word;
                    old_word = strtok(str, " ");
                    old_word = strtok(NULL, " ");
                    new_word = strtok(NULL, "\n");
                    int pozitie = 0;
                    pozitie = find_word(list, old_word);
                    if(pozitie != 0) {
                        add_to_stack(stack, str2);
                        stack->top->poz = pozitie;
                        replace_old_word(list, old_word, new_word);
                    }
                }
                //replace all este o comanda asemanatoare cu cea de sus
                //dar aceasta inlocuieste toate aparitiile cuvantului
                if(str[0] == 'r' && str[1] == 'a') {
                    int n = list->curr->poz;
                    char str2[100];
                    strcpy(str2, str);
                    char *old_word, *new_word;
                    old_word = strtok(str, " ");
                    old_word = strtok(NULL, " ");
                    new_word = strtok(NULL, "\n");
                    struct Node *node = list->curr;
                    while(node != list->tail) {
                        int pozitie = 0;
                        pozitie = find_word(list, old_word);
                        if(pozitie != 0) {
                            node = list->curr;
                            add_to_stack(stack, str2);
                            stack->top->poz = pozitie;
                            replace_old_word(list, old_word, new_word);
                            for(int i = 0 ; i < strlen(new_word) - 1; i++) {
                                list->curr = list->curr->next;
                            }
                        } else {
                            break;
                        }
                    }
                    node = list->head;
                    for(int i = 0 ; i < list->size; i++) {
                        if(node->poz == n) {
                            list->curr = node;
                        }
                        node = node->next;
                    }
                }
                //delete word gaseste si sterge un cuvant primit parametru
                if(str[0] == 'd' && str[1] == 'w') {
                    char str2[100];
                    strcpy(str2, str);
                    char *old_word;
                    old_word = strtok(str, " ");
                    old_word = strtok(NULL, "\n");
                    int pozitie = 0;
                    pozitie = find_word(list, old_word);
                    if(pozitie != 0) {
                        add_to_stack(stack, str2);
                        stack->top->poz = pozitie;
                        delete_old_word(list, old_word);
                    }
                }
                //delete all sterge toate cuvintele
                if(str[0] == 'd' && str[1] == 'a') {
                    char str2[100];
                    strcpy(str2, str);
                    char *old_word;
                    old_word = strtok(str, " ");
                    old_word = strtok(NULL, "\n");
                    struct Node *node = list->curr;
                    while(node != list->tail) {
                        int pozitie = 0;
                        pozitie = find_word(list, old_word);
                        if(pozitie != 0) {
                            node = list->curr;
                            add_to_stack(stack, str2);
                            stack->top->poz = pozitie;
                            delete_old_word(list, old_word);
                        } else {
                            break;
                        }
                    }
                }
                //comanda undo reintoarce lista la stadiul precedent
                if(str[0] == 'u') {
                    undo_the_last_command(list, stack, stack_undo);
                }
                //comanda redo poate fi folosita doar dupa undo
                //si reintoarce lista la stadiul precedent undo
                if(str[0] == 'r' && str[1] == '\n') {
                    redo_the_last_command(list, stack, stack_undo);
                }
            }
        }
        //in acest for se parcurge lista si se sterg 2 \n
        //gasite unul langa celalalt
        struct  Node *node = list->head;
        for(int i = 0 ; i < list->size - 1; i++) {
            if(node->data == '\n' && node->next->data == '\n') {
                node = node->prev;
                node->next = node->next->next;
                free(node->next->prev);
                node->next->prev = node;
                list->size--;
            }
            node = node->next;
        }
    }

    //in final eliberam memoria ocupata in timpul programului si
    // inchidem fisierul
	fclose(in);
    free_memory(list);
    free_stack(stack);
    free_stack(stack_undo);
	free(list);
	free(stack);
	free(stack_undo);
}
