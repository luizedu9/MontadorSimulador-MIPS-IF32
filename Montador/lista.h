/* -------------------------------------------------------
 * File: lista.h
* Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#ifndef LISTA_H
#define LISTA_H

typedef struct lista *Lista;

Lista cria_label();
void destroi_label(Lista lista);
void insere_label(Lista lista, char *nome, int endereco);
int retorna_label_endereco(Lista lista, char *label);
char *retorna_label_nome(Lista lista, int posicao);
int retorna_tamanho(Lista lista);

#endif /* LISTA_H */