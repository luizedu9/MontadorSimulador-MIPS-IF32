/* -------------------------------------------------------
 * File: main.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

/* -------------------------------------------------------
 * File: main.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"

int main(int argc, char **argv) {
    if (argc == 2) {
        simulador(argv[1]);
        return(0);
    }
    else {
        printf("Erro - Falta parâmetros de entrada");
    }
    return(-1);
}