/* -------------------------------------------------------
 * File: decodifica.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char decodifica_tipo(int instrucao) { /* Devolve o tipo da instrução; */
    int temp;
    temp = instrucao >> 26;
    if (temp == 0) { /* Syscal não é uma instrução do tipo R, mas a incluimos junto; */
        return('R');
    }
    if ((temp == 2) || (temp == 3)) {
        return('J');
    }
    else {
        return('I');
    }
}

int decodifica_instrucao(int instrucao, char tipo) { /* Devolve um inteiro capaz de identificar a instrução; */
    int temp;
    if (tipo == 'R') {
        temp = instrucao & 63;
        return(temp);
    }
    temp = instrucao >> 26; /* Se não for R, é J ou I */
    return(temp);
}