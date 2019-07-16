/* -------------------------------------------------------
 * File: arquivo.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>


FILE *abre_arquivo(char *arquivo, char *tipo) {
    FILE *arq;
    arq = fopen(arquivo, tipo);
    if (arq == NULL) {
        printf("ERRO - Arquivo %s", tipo);
        exit(0);
    }
    return(arq);
}

void fecha_arquivo(FILE *arq) {
    fclose(arq);
    return;
}

int retorna_instrucao(FILE *arq, char *instrucao) {
    if (fgets(instrucao, 100, arq) != NULL) {
        return(1);
    }
    return(0);
}