/* -------------------------------------------------------
 * File: arquivo.h
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#ifndef ARQUIVO_H
#define ARQUIVO_H

FILE *abre_arquivo(char *arquivo, char *tipo);/* O parâmetro de entrada (char *tipo) é a função de abertura do arquivo a ser utilizada, por exemplo (apend); */
void fecha_arquivo(FILE *arquivo);/* Fecha o arquivo depois que acabar de modifica-lo para liberar o arquivo, caso seja necessário em outra aplicaçao; */
int retorna_instrucao(FILE *arquivo, char *instrucao, int tamanho);/* Lê o arquivo para retornar os dados das instruções que se encontram armazenadas no arquivo; */
void escreve_arquivo(FILE *arquivo, char *string); /* Escreve a saída do programa em um arquivo; */

#endif /* ARQUIVO_H */

