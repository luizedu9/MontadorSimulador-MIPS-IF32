/* -------------------------------------------------------
 * File: simulador.h
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#ifndef simulador_H
#define simulador_H

void simulador(char *arq_entrada);
int retorna_program_counter();
void altera_program_counter(int pc);
int retorna_memoria(int posicao);
void altera_memoria(int posicao, int valor);

#endif