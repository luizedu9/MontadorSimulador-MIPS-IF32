/* -------------------------------------------------------
 * File: registrador.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "registrador.h"

int *inicia_registradores() {
    int *registradores;
    int i;
    registradores = (int*) malloc(32 * sizeof(int));
    for (i = 0; i < 32; i++) {
        registradores[i] = 0;
    }
    return(registradores);
}

void destroi_registradores(int *registradores) {
    free(registradores);
    return;
}

void insere_registrador(int *registradores, int registrador, int valor) {
    registradores[registrador] = valor;
    return;
}

int retorna_registrador(int *registradores, int registrador) {
    return(registradores[registrador]);
}

int verifica_registrador(int instrucao, int registrador) { /* Retorna o registrador requisitado (1, 2 ou 3); */
    int temp;
    if (registrador == 1) {
        temp = instrucao >> 21;   
    }
    if (registrador == 2) {
        temp = instrucao >> 16;
    }
    if (registrador == 3) {
        temp = instrucao >> 11;
    }
    if (registrador == 4) { /* shamt; */
        temp = instrucao >> 6;
    }
    temp = temp & 31;
    return(temp);
}

int verifica_imediate(int instrucao) {
    return(instrucao & 65535);
}

int verifica_address(int instrucao) {
    return(instrucao & 67108863);
}