/* -------------------------------------------------------
 * File: busca.h
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#ifndef busca_H
#define busca_H

struct informacao {
    unsigned int *dado;
    int tamanho_dado;
    unsigned int *instrucao;
    int tamanho_instrucao;
};
typedef struct informacao *Informacao;

Informacao fetch(char *arquivo);
void destroi_fetch(Informacao informacao);

#endif