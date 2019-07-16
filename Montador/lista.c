/* -------------------------------------------------------
 * File: lista.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lista.h"

struct label {
    int endereco;
    char nome[30];
    struct label *proximo;
};
typedef struct label *Label;

struct lista {
    int tamanho;
    struct label *inicio;
};

Lista cria_label() {
    Lista lista;
    lista = (Lista) malloc(sizeof(struct lista));
    lista->tamanho = 0;
    lista->inicio = NULL;
    return(lista);
}

void destroi_label(Lista lista) {
    Label aux;
    aux = lista->inicio;
    while (aux != NULL) {
	lista->inicio = aux->proximo;
	free(aux);
	aux = lista->inicio;
    }
    free(lista);
    lista = NULL;
    return;
}

void insere_label(Lista lista, char *nome, int endereco) {
    Label label, aux;
    lista->tamanho++;
    label = (Label) malloc(sizeof(struct label));
    label->nome[0] = '\0';
    strcat(label->nome, nome);
    label->endereco = endereco;
    label->proximo = NULL;
    if (lista->inicio == NULL) { /* Caso inicial - Lista vazia; */
    	lista->inicio = label;
	return;
    }
    aux = lista->inicio; /* Inserir no final da lista; */
    while (aux->proximo != NULL) {
	aux = aux->proximo;
    }
    aux->proximo = label;
    return;
}

int retorna_label_endereco(Lista lista, char *label) {
    Label aux;
    aux = lista->inicio;
    while (aux != NULL) {
	if (strcmp(label, aux->nome) == 0) { /* Verifica se são iguais; */
            return(aux->endereco);
	}
	aux = aux->proximo;
    }
    return(-1); /* (-1) pois 0 é uma posição de memoria; */
}

char *retorna_label_nome(Lista lista, int posicao) {
    Label aux;
    int i;
    aux = lista->inicio;
    for (i = 0; i < posicao; i++) {
	aux = aux->proximo;
    }
    if (aux != NULL) {
        return(aux->nome);
    }
    return("");
}

int retorna_tamanho(Lista lista) {
    return(lista->tamanho);
}