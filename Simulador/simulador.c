/* -------------------------------------------------------
 * File: simulador.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "simulador.h"
#include "busca.h"
#include "registrador.h"
#include "decodifica.h"
#include "executa.h"

int program_counter = 0;
Informacao informacao;

void simulador(char *arq_entrada) {
    char tipo;
    int id_instrucao;
    int *registradores;
    program_counter = 0;
    informacao = fetch(arq_entrada); /* Busca; */
    registradores = inicia_registradores();
    while (program_counter < informacao->tamanho_instrucao) {
        tipo = decodifica_tipo(informacao->instrucao[program_counter]); /* Decodifica; */
        id_instrucao = decodifica_instrucao(informacao->instrucao[program_counter], tipo); /* Decodifica; */
        executa(registradores, informacao->instrucao[program_counter], tipo, id_instrucao); /* Executa; */
        program_counter++;
    }
    destroi_registradores(registradores);
    destroi_fetch(informacao);
}

int retorna_program_counter() {
    return(program_counter);
}

void altera_program_counter(int pc) {
    program_counter = pc;
    return;
}

int retorna_memoria(int posicao) {
    return(informacao->dado[posicao]);
}

void altera_memoria(int posicao, int valor) {
    informacao->dado[posicao] = valor;
    return;
}