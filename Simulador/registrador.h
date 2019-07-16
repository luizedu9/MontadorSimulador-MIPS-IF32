/* -------------------------------------------------------
 * File: registrador.h
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#ifndef REGISTRADOR_H
#define REGISTRADOR_H

int *inicia_registradores();
void destroi_registradores(int *registradores);
void insere_registrador(int *registradores, int registrador, int valor);
int retorna_registrador(int *registradores, int registrador);
int verifica_registrador(int instrucao, int registrador);
int verifica_imediate(int instrucao);
int verifica_address(int instrucao);

#endif /* REGISTRADOR_H */