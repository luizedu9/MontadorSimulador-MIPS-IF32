/* -------------------------------------------------------
 * File: busca.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "arquivo.h"
#include "busca.h"

int pega_tamanho(char *string);

Informacao fetch(char *arquivo) {
    char string[30];
    int i = 0;
    FILE *arq;
    Informacao informacao = (Informacao) malloc(sizeof(struct informacao));
    arq = abre_arquivo(arquivo, "r"); 
    retorna_instrucao(arq, string); /* .data; */
    informacao->tamanho_dado = pega_tamanho(string);
    informacao->dado = (unsigned int*) malloc((informacao->tamanho_dado + 100) * sizeof(unsigned int));
    retorna_instrucao(arq, string);
    while(strcmp(string, ".enddata\n") != 0) {
        if (string[0] != '=') {
            string[8] = '\0';
            informacao->dado[i] = strtol(string, NULL, 16);
            i++;
        }
        retorna_instrucao(arq, string);
    } 
    i = 0;
    retorna_instrucao(arq, string); /* .text; */
    informacao->tamanho_instrucao = pega_tamanho(string);
    informacao->instrucao = (unsigned int*) malloc(informacao->tamanho_instrucao * sizeof(unsigned int));
    retorna_instrucao(arq, string); 
    while(strcmp(string, ".endtext") != 0) {
        if (string[0] != '=') {
            string[8] = '\0';
            informacao->instrucao[i] = strtol(string, NULL, 16);
            i++;
        }    
        retorna_instrucao(arq, string);
    } 
    fecha_arquivo(arq);
    return(informacao);
}

int pega_tamanho(char *string) {
    int i = 6, j = 0;
    char stamanho[15];
    while (string[i] != '\0') {
        stamanho[j] = string[i];
        j++;
        i++;
    }
    stamanho[j] = '\0';    
    return(atoi(stamanho));
}

void destroi_fetch(Informacao informacao) {
    free(informacao->dado);
    free(informacao->instrucao);  
    free(informacao);
    return;
}