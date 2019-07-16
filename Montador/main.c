/* -------------------------------------------------------
 * File: main.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "montador.h"

int main(int argc, char **argv) {
    if(argc == 3) {
        montador(argv[1], argv[2]);
        return 0;
    }
    else {
        printf("Erro - Falta parâmetros de entrada");
    }
    return(-1);
}