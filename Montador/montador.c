/* -------------------------------------------------------
 * File: montador.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 06/05/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "montador.h"
#include "lista.h"
#include "arquivo.h"

Lista lista_data = NULL, lista_text = NULL;
int mem_data = 0, mem_text = 0;

/* ----------------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------ CABEÇALHO ------------------------------------------------ */
/* ----------------------------------------------------------------------------------------------------------- */
void le_data(char *arquivo_entrada, char *arquivo_saida);
void le_label(char *arquivo_entrada);
void le_instrucao(char *arquivo_entrada, char *arquivo_saida);

void verifica_funcao(char *funcao, char *instrucao, char *instrucao_saida);
int verifica_pseudo(char *funcao, char *instrucao, char *pseudo_instrucao);
void verifica_diretiva(char *instrucao);
char *verifica_registrador(char *registrador);
void conta_memoria(char *instrucao);

void bloco_instrucao(char *instrucao, char bloco[][30], int qtd_bloco);
void bloco_especial(char *instrucao, char bloco[][30]);
void upper_lower(char *big, char *upper, char *lower);
void quebra_pseudo(char *pseudo_instrucao, char *pseudo1, char *pseudo2);
int conversor(char *numero, int qtd_bit, char *resultado, int tipo);
void bin_hexa(char* instrucao, char* instrucaosaida);
void int_string(int numero, char *string);
int verifica_label(char *instrucao);
void remove_label(char *instrucao);
void escreve_label(FILE *arq_saida, int boolean);
void go_text(FILE *arq);
void formata_instrucao(char *instrucao);
/* ----------------------------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------- INICIALIZADOR ---------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void montador(char *arq_entrada, char *arq_saida) {
   lista_text = cria_label();
   lista_data = cria_label();
   le_data(arq_entrada, arq_saida);
   le_label(arq_entrada);
   le_instrucao(arq_entrada, arq_saida);
   destroi_label(lista_text);
   destroi_label(lista_data);
   return;
}

/* NOTA SOBRE O ARQUIVO TEMPORARIO:
 Para calcular a memoria usada, seria necessario fazer quase o mesmo esforço da tradução,
 já que teria que contar quantas words cada label usaria, entao optamos por fazer tudo de uma vez.
 O unico problema, é que para gravar o ".data <inteiro>" é necessario ter a memoria calculada previamente,
 o que nesse caso, nao temos. Entao a solução é guardar a tradução em um arquivo temporario, e depois
 que a memoria for totalmente calculada e gravada, passar a tradução para o arquivo de saida; */
void le_data(char *arquivo_entrada, char *arquivo_saida) {
    FILE *arq_entrada, *arq_saida, *arq_temporario;
    char instrucao[100], string[100];
    arq_entrada = abre_arquivo(arquivo_entrada, "r");
    arq_temporario = abre_arquivo("temporario.tmp", "w"); /* Cria arquivo; */
    fecha_arquivo(arq_temporario);
    while (retorna_instrucao(arq_entrada, instrucao, sizeof(instrucao))) { 
        instrucao[strlen(instrucao)] = '\0';
        formata_instrucao(instrucao);
        if (strcmp(instrucao, ".text") == 0) { /* Acabou o .data; */
            break;
        }
        if ((instrucao[0] != '\0') && (strcmp(instrucao, ".data") != 0)) {
            verifica_diretiva(instrucao);
        }
    }
    arq_saida = abre_arquivo(arquivo_saida, "w");
    arq_temporario = abre_arquivo("temporario.tmp", "r");
    sprintf(string, ".data %d\n", (mem_data / 4)); /* Converte mem_data para uma string; */
    escreve_arquivo(arq_saida, string);
    escreve_label(arq_saida, 0);
    while (retorna_instrucao(arq_temporario, instrucao, sizeof(instrucao))) { /* Escreve os dados do .data no arquivo de saída; */
        escreve_arquivo(arq_saida, instrucao);
    }
    escreve_arquivo(arq_saida, ".enddata");
    escreve_arquivo(arq_saida, "\n");
    fecha_arquivo(arq_entrada);
    fecha_arquivo(arq_saida);
    fecha_arquivo(arq_temporario);
    remove("temporario.tmp");
    return;
}

void le_label(char *arquivo_entrada) { /* Pega os labels do arquivo de entrada e insere na lista de labels; */
    FILE *arq_entrada;
    int i;
    char instrucao[100], label[100];
    arq_entrada = abre_arquivo(arquivo_entrada, "r");
    go_text(arq_entrada);
    while (retorna_instrucao(arq_entrada, instrucao, sizeof(instrucao))) { /* Pega os dados das instruçoes no arquivo; */
        instrucao[strlen(instrucao)] = '\0'; /* Coloca um '\0' no final da string para terminá-la; */
        formata_instrucao(instrucao);
        if ((instrucao[0] != '\0') && (instrucao[0] != '#') && (instrucao[0] != '\n') && (instrucao[0] != '\r')) { /* Se for diferente de linha sem label ou instrução; */
            if (verifica_label(instrucao)) {
                /*i = 0;                
                while (instrucao[i] != ':') {
                    label[i] = instrucao[i];
                    i++;
                }
                label[i] = '\0';*/
                label[0] = '\0';
                strcpy(label, instrucao);
                for (i = 0; i < strlen(label); i++) {
                    if (label[i] == ':') {
                       label[i] = '\0'; /* Corta tudo, exceto o nome do Label; */
                       break;
                    }
                }
                insere_label(lista_text, label, mem_text);
            }
            conta_memoria(instrucao); /* Se for uma instrução, aumenta a contagem de memoria; */
        }
    }
    fecha_arquivo(arq_entrada);
    return;
}

void le_instrucao(char *arquivo_entrada, char *arquivo_saida) { /* Pega as instruçoes no arquivo de entrada e verifica se ela é uma instrução ou uma pseudo instrução, e depois joga no arquivo de saída;*/ 
    FILE *arq_entrada, *arq_saida;						
    int i;
    char instrucao[100], instrucao_saida[33], pseudo_instrucao[65], pseudo_saida[33], instrucao_hexa[11], string[100], funcao[10];
    arq_entrada = abre_arquivo(arquivo_entrada, "r");
    arq_saida = abre_arquivo(arquivo_saida, "a");
    go_text(arq_entrada);
    sprintf(string, ".text %d\n", (mem_text / 4)); /* Converte mem_text para uma string; */
    escreve_arquivo(arq_saida, string);
    escreve_label(arq_saida, 1);
    while (retorna_instrucao(arq_entrada, instrucao, sizeof(instrucao))) { /* Pega as instrucoes no arquivo de entrada; */
        i = 0;
        instrucao[strlen(instrucao)] = '\0';
        formata_instrucao(instrucao);
        if (verifica_label(instrucao)) {
            remove_label(instrucao);
        }
        while ((instrucao[i] != ' ') && (instrucao[i] != '\0')) { /* Verifica nome da Instrução; */
            funcao[i] = instrucao[i];	
            i++;	
        }
        funcao[i] = '\0';
        if (verifica_pseudo(funcao, instrucao, pseudo_instrucao)) { /* Verifica se é uma pseudo instrução, se for quebra ela em duas instruçoes, com exceção ao mov; */
            quebra_pseudo(pseudo_instrucao, instrucao_saida, pseudo_saida); 
            bin_hexa(instrucao_saida, instrucao_hexa);
            escreve_arquivo(arq_saida, instrucao_hexa);
            escreve_arquivo(arq_saida, "\n"); 
            if (!pseudo_instrucao[32] == '\0') { /* Pseudo-instrução com duas instruções; */
                bin_hexa(pseudo_saida, instrucao_hexa);
                escreve_arquivo(arq_saida, instrucao_hexa);
                escreve_arquivo(arq_saida, "\n");
            }
        }
        else {
            verifica_funcao(funcao, instrucao, instrucao_saida); /* verifica qual é a instrução a partir do nome dela; */
            if (instrucao[0] != '\0') {
                bin_hexa(instrucao_saida, instrucao_hexa);
                escreve_arquivo(arq_saida, instrucao_hexa);
                escreve_arquivo(arq_saida, "\n");            
            }
        }
    }
    escreve_arquivo(arq_saida, ".endtext"); /* Termina o arquivo com um .endtext para falar que a área de código terminou; */
    fclose(arq_entrada);
    fclose(arq_saida);
    return;
}
/* ----------------------------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------------------------- */
/* ----------------------------------------------- CONSTRUTOR ------------------------------------------------ */
/* ----------------------------------------------------------------------------------------------------------- */
void verifica_funcao(char *funcao, char *instrucao, char *instrucao_saida) { /* Verifica se tem a instrução que foi pega no arquivo de entrada pelo nome, e retorna a tradução em binário da função; */
    char shamt[6], immediate[17], address[27];								
    char bloco[3][30];
    instrucao_saida[0] = '\0';
    if (strcmp(funcao, "lui") == 0) {
        bloco_instrucao(instrucao, bloco, 2);
		strcat(instrucao_saida, "001111");
        strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[1], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "add") == 0) {
    	bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
        strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100000");
        return;
    }
    if (strcmp(funcao, "sub") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100010");
        return;
    }
    if (strcmp(funcao, "slt") == 0) {
    	bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "101010");
        return;
    }
    if (strcmp(funcao, "addi") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "slti") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001010");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "and") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100100");
        return;
    }
    if (strcmp(funcao, "or") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100101");
        return;
    }
    if (strcmp(funcao, "xor") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100110");
        return;
    }
    if (strcmp(funcao, "nor") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100111");
        return;
    }
    if (strcmp(funcao, "andi") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001100");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;        
    }
    if (strcmp(funcao, "ori") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001101");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0 );
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "xori") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001110");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "lw") == 0) { /* lw rt, imm(rs) */
        bloco_especial(instrucao, bloco);
		strcat(instrucao_saida, "100011");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "sw") == 0) {
        bloco_especial(instrucao, bloco);
		strcat(instrucao_saida, "101011");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;        
    }
    if (strcmp(funcao, "j") == 0) {
        bloco_instrucao(instrucao, bloco, 1);
		strcat(instrucao_saida, "000010");
        if (isalpha(bloco[0][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[0]), address);
            conversor(address, 26, address, 1); 
        }
        else {
            conversor(bloco[0], 26, address, 1);
        }
		strcat(instrucao_saida, address);
        return;
    }
    if (strcmp(funcao, "jr") == 0) {
		bloco_instrucao(instrucao, bloco, 1);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "001000");
        return;
    }
    if (strcmp(funcao, "bltz") == 0) {
        bloco_instrucao(instrucao, bloco, 2);
		strcat(instrucao_saida, "000001");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
        strcat(instrucao_saida, "00000");
        conversor(bloco[1], 16, immediate, 0);
		strcat(instrucao_saida, immediate); 
        return;
    }
    if (strcmp(funcao, "beq") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000100");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
        strcat(instrucao_saida, verifica_registrador(bloco[1]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "bne") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000101");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
        strcat(instrucao_saida, verifica_registrador(bloco[1]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "mfhi") == 0) {
		bloco_instrucao(instrucao, bloco, 1);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "010000");
        return;
    }
    if (strcmp(funcao, "mflo") == 0) {
		bloco_instrucao(instrucao, bloco, 1);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "010010");
        return;
    }
    if (strcmp(funcao, "addu") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100001");
        return;
    }
    if (strcmp(funcao, "subu") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "100011");
        return;
    }
    if (strcmp(funcao, "addiu") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "001001");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
        strcat(instrucao_saida, verifica_registrador(bloco[0]));
        conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "sll") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
        strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 5, shamt, 0);
		strcat(instrucao_saida, shamt);
		strcat(instrucao_saida, "000000");
        return;
    }
    if (strcmp(funcao, "srl") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
        strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 5, shamt, 0);
		strcat(instrucao_saida, shamt);
		strcat(instrucao_saida, "000010");
        return;
    }
    if (strcmp(funcao, "sra") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
        strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 5, shamt, 0);
		strcat(instrucao_saida, shamt);
		strcat(instrucao_saida, "000011");
        return;
    }
    if (strcmp(funcao, "sllv") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "000100");
        return;
    }
    if (strcmp(funcao, "srlv") == 0) {
		bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "000110");
        return;
    }
    if (strcmp(funcao, "srav") == 0) {
        bloco_instrucao(instrucao, bloco, 3);
		strcat(instrucao_saida, "000000");
		strcat(instrucao_saida, verifica_registrador(bloco[2]));
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		strcat(instrucao_saida, "00000");
		strcat(instrucao_saida, "000111");
        return;
    }
    if (strcmp(funcao, "lb") == 0) {
        bloco_especial(instrucao, bloco);
		strcat(instrucao_saida, "100000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "lbu") == 0) {
        bloco_especial(instrucao, bloco);
		strcat(instrucao_saida, "100100");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "sb") == 0) { 
        bloco_especial(instrucao, bloco);
		strcat(instrucao_saida, "101000");
		strcat(instrucao_saida, verifica_registrador(bloco[1]));
		strcat(instrucao_saida, verifica_registrador(bloco[0]));
		conversor(bloco[2], 16, immediate, 0);
		strcat(instrucao_saida, immediate);
        return;
    }
    if (strcmp(funcao, "jal") == 0) {
        bloco_instrucao(instrucao, bloco, 1);
		strcat(instrucao_saida, "000011");
        if (isalpha(bloco[0][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[0]), address);
            conversor(address, 26, address, 0); 
        }
        else {
            conversor(bloco[0], 26, address, 1);
        }
		strcat(instrucao_saida, address);
        return;
    }
    if (strcmp(funcao, "syscall") == 0) {
        strcat(instrucao_saida, "00000000000000000000000000001100");
        return;
    }	
    instrucao[0] = '\0'; /* Instrução Invalida; */
    return;
}

int verifica_pseudo(char *funcao, char *instrucao, char *pseudo_instrucao) { /* Se ela for uma pseudo-instrucao, traduz ela nas instruções correspondentes; */
    char big[33], upper[17], lower[17], endereco[33];
    char bloco[3][30];
    pseudo_instrucao[0] = '\0';
    if (strcmp(funcao, "mov") == 0) { /* "addi $rt, $rs, 0" */
    	bloco_instrucao(instrucao, bloco, 2);
		strcat(pseudo_instrucao, "001000");
		strcat(pseudo_instrucao, verifica_registrador(bloco[1]));
        strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, "0000000000000000");
        pseudo_instrucao[32] = '\0';
        return(1);
    }
    if (strcmp(funcao, "li") == 0) { /* "lui $rs, upper(big)" - "ori $rs, $rs, lower(big)" */
        bloco_instrucao(instrucao, bloco, 2);
        conversor(bloco[1], 32, big, 0);
        upper_lower(big, upper, lower);
        strcat(pseudo_instrucao, "001111"); /* lui */
        strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
        strcat(pseudo_instrucao, upper);
		strcat(pseudo_instrucao, "001101"); /* ori */
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
        strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, lower);
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    if (strcmp(funcao, "la") == 0) { /* "lui $rs, upper(big)" - "ori $rs, $rs, lower(big)" */
        bloco_instrucao(instrucao, bloco, 2);
        if (isalpha(bloco[1][0])) {
            int_string(retorna_label_endereco(lista_data, bloco[1]), endereco);
            conversor(endereco, 32, big, 0); 
        }
        else {
            conversor(bloco[2], 32, big, 0);
        }  
        upper_lower(big, upper, lower);
        strcat(pseudo_instrucao, "001111"); /* lui */
        strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
        strcat(pseudo_instrucao, upper);
		strcat(pseudo_instrucao, "001101"); /* ori */
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
        strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, lower);
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    if (strcmp(funcao, "bge") == 0) { /* "slt $t0, $rt, $rs" - "beq $t0, $zero, LABEL" */
        bloco_instrucao(instrucao, bloco, 3);
		strcat(pseudo_instrucao, "000000"); /* slt */
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, verifica_registrador(bloco[1]));	
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
		strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, "101010");
        strcat(pseudo_instrucao, "000100"); /* beq */
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
        strcat(pseudo_instrucao, verifica_registrador("$zero"));
        if (isalpha(bloco[2][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[2]), endereco);
            conversor(endereco, 16, upper, 0);
            strcat(pseudo_instrucao, upper); 
        }
        else {
            conversor(bloco[2], 16, upper, 1);
            strcat(pseudo_instrucao, upper); 
        }    
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    if (strcmp(funcao, "bgt") == 0) { /* "slt $t0, $rs, $rt" - "bne $t0, $zero, LABEL" */
        bloco_instrucao(instrucao, bloco, 3);
		strcat(pseudo_instrucao, "000000"); /* slt */
		strcat(pseudo_instrucao, verifica_registrador(bloco[1]));
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
		strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, "101010");
        strcat(pseudo_instrucao, "000101"); /* bne */
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
        strcat(pseudo_instrucao, verifica_registrador("$zero"));
        if (isalpha(bloco[2][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[2]), endereco);
            conversor(endereco, 16, upper, 0);
            strcat(pseudo_instrucao, upper);
        }
        else {
            conversor(bloco[2], 16, upper, 1);
            strcat(pseudo_instrucao, upper); 
        }    
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    if (strcmp(funcao, "ble") == 0) { /* "slt $t0, $rs, $rt" - "beq $t0, $zero, LABEL" */
        bloco_instrucao(instrucao, bloco, 3);
		strcat(pseudo_instrucao, "000000"); /* slt */
		strcat(pseudo_instrucao, verifica_registrador(bloco[1]));
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
		strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, "101010");
        strcat(pseudo_instrucao, "000100"); /* beq */
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
        strcat(pseudo_instrucao, verifica_registrador("$zero"));
        if (isalpha(bloco[2][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[2]), endereco);
            conversor(endereco, 16, upper, 0);
            strcat(pseudo_instrucao, upper); 
        }
        else {
            conversor(bloco[2], 16, upper, 1);
            strcat(pseudo_instrucao, upper); 
        } 
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    if (strcmp(funcao, "blt") == 0) { /* "slt $t0, $rt, $rs" - "bne $t0, $zero, LABEL" */
        bloco_instrucao(instrucao, bloco, 3);
		strcat(pseudo_instrucao, "000000"); /* slt */
		strcat(pseudo_instrucao, verifica_registrador(bloco[0]));
		strcat(pseudo_instrucao, verifica_registrador(bloco[1]));
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
		strcat(pseudo_instrucao, "00000");
		strcat(pseudo_instrucao, "101010");
        strcat(pseudo_instrucao, "000101"); /* bne */
		strcat(pseudo_instrucao, verifica_registrador("$t0"));
        strcat(pseudo_instrucao, verifica_registrador("$zero"));
        if (isalpha(bloco[2][0])) {
            int_string(retorna_label_endereco(lista_text, bloco[2]), endereco);
            conversor(endereco, 16, upper, 0);
            strcat(pseudo_instrucao, upper);
        }
        else {
            conversor(bloco[2], 16, upper, 1);
            strcat(pseudo_instrucao, upper); 
        }    
        pseudo_instrucao[64] = '\0';
        return(1);
    }
    return(0);
}

void verifica_diretiva(char *instrucao) { /* Verifica se a instrução encontrada é uma diretiva, e coloca os dados da diretiva em um arquivo temporário, já com as words na ordem certa; */
    FILE *arq_temporario;
    int i = 0, j = 0, k = 0, cont, acabou = 0, letra;
    char label[31], diretiva[11], word[33], hexa[33];
    char byte[9];
    arq_temporario = abre_arquivo("temporario.tmp", "a");
    for (k = 0; k < 32; k++) { /* Zera word; */
            word[k] = '0';
        }
        word[32] = '\0';
    while (instrucao[i] != ':') { /* Encontra Label; */
        label[i] = instrucao[i];	
        i++;	
    }
    label[i] = '\0';
    i += 2;
    insere_label(lista_data, label, mem_data);
    while (instrucao[i] != ' ') { /* Encontra Diretiva; */
        diretiva[j] = instrucao[i];
        j++;
        i++;
    }
    diretiva[j] = '\0';
    i++;
    if (strcmp(diretiva, ".byte") == 0) { /* --------------------------- .BYTE --------------------------- */
        cont = 0;
        while ((instrucao[i] != '\0') && !(acabou)) { /* Encontra parametros; */
            j = 0;
            while ((instrucao[i] != ',') && instrucao[i] != '\0') { /* Pega Byte; */
                byte[j] = instrucao[i];
                j++;
                i++;
            }
            byte[j] = '\0';
            if (instrucao[i] == '\0') {
                acabou = 1;
            }
            i += 2;
            conversor(byte, 8, byte, 0); /* Converte número para byte; */
            cont++;
            j = 0;
            if (cont == 1) { /* Coloca os bits no byte menos significativo; */
                for (k = 24; k < 32; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) { /* Se sequencia de bytes acabou, salva a word incompleta; */
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4; 
                    break;
                }
            }
            if (cont == 2) {
               for (k = 16; k < 24; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) {
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4;
                    break;
                }
            }
            if (cont == 3) {
                for (k = 8; k < 16; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) {
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4;
                    break;
                }
            }
            if (cont == 4) { /* Coloca os bits no byte mais significativo; */
                for (k = 0; k < 8; k++) {
                    word[k] = byte[j];
                    j++;
                }
                bin_hexa(word, hexa);
                escreve_arquivo(arq_temporario, hexa);       
                escreve_arquivo(arq_temporario, "\n"); 
                mem_data += 4; 
                cont = 0; /* Reinicia contador; */
                for (k = 0; k < 32; k++) { /* Zera word; */
                   word[k] = '0';
                }
            }
        }
    }
    if (strcmp(diretiva, ".word") == 0) { /* --------------------------- .WORD --------------------------- */
        j = 0;
        while (instrucao[i] != '\0') { /* Encontra Parametro; */
            diretiva[j] = instrucao[i];
            j++;
            i++;
        }
        diretiva[j] = '\0';
        conversor(diretiva, 32, word, 0);
        bin_hexa(word, hexa);
        mem_data += 4;
        escreve_arquivo(arq_temporario, hexa);       
        escreve_arquivo(arq_temporario, "\n"); 
    }
    if (strcmp(diretiva,".asciiz") == 0) { /* ------------------------- . ASCIIZ ------------------------- */
        cont = 0;
        i++; /* Pulas as aspas */
        while ((instrucao[i] != '"') && (instrucao[i] != '\0')) { /* Encontra parametros; */
            j = 0;
            if ((instrucao[i] == '\\') && (instrucao[i + 1] == 'n')) { /* Caso especial: '\n' */
                letra = 10;
                i++;
            }
            else {
                letra = (int) instrucao[i];
            }
            sprintf(byte, "%d", letra);
            conversor(byte, 8, byte, 0);
            i++;
            if ((instrucao[i] == '"') || (instrucao[i] == '\0')) {
                acabou = 1;
            }
            cont++;
            j = 0;
            if (cont == 1) { /* Coloca os bits no byte menos significativo; */
                for (k = 24; k < 32; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) { /* Se sequencia de bytes acabou, salva a word incompleta; */
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4; 
                    break;
                }
            }
            if (cont == 2) {
				for (k = 16; k < 24; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) {
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4;
                    break;
                }
            }
            if (cont == 3) {
                for (k = 8; k < 16; k++) {
                    word[k] = byte[j];
                    j++;
                }
                if (acabou) {
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4;
                    break;
                }
            }
            if (cont == 4) { /* Coloca os bits no byte mais significativo; */
                for (k = 0; k < 8; k++) {
                    word[k] = byte[j];
                    j++;
                }
                bin_hexa(word, hexa);
                escreve_arquivo(arq_temporario, hexa);       
                escreve_arquivo(arq_temporario, "\n"); 
                mem_data += 4; 
                cont = 0; /* Reinicia contador; */
                for (k = 0; k < 32; k++) { /* Zera word; */
                   word[k] = '0';
                }
                if (acabou) {
                    bin_hexa(word, hexa);
                    escreve_arquivo(arq_temporario, hexa);       
                    escreve_arquivo(arq_temporario, "\n"); 
                    mem_data += 4;  
                }
            }
        }
    }
    if (strcmp(diretiva, ".space") == 0) { /* -------------------------- .SPACE -------------------------- */
        j = 0;
        while (instrucao[i] != '\0') { /* Pega Byte; */
            byte[j] = instrucao[i];
            j++;
            i++;
        }
        byte[j] = '\0';
        i = atoi(byte); /* Quantidade de words a ser reservada; */
        j = i / 4; /* Quociente; */
        k = i % 4; /* Resto; */
        if (k > 0) { /* Se houve resto, entao aumentar uma word; */
            i = j + 1; 
        }
        else {
            i = j;
        }
        bin_hexa(word, hexa); /* word ja foi previamente preenchida com "0"; */
        for (j = 0; j < i; j++) { 
            mem_data += 4;
            escreve_arquivo(arq_temporario, hexa);       
            escreve_arquivo(arq_temporario, "\n"); 
        }
    }
    fecha_arquivo(arq_temporario);
}

char *verifica_registrador(char *registrador) {
    if ((strcmp(registrador, "$zero") == 0) || (strcmp(registrador, "$0") == 0)) {
	return("00000");
    }
    if ((strcmp(registrador, "$at") == 0) || (strcmp(registrador, "$1") == 0)) {
	return("00001");
    }
    if ((strcmp(registrador, "$v0") == 0) || (strcmp(registrador, "$2") == 0)) {
    	return("00010");
    }
    if ((strcmp(registrador, "$v1") == 0) || (strcmp(registrador, "$3") == 0)) {
	return("00011");
    }
    if ((strcmp(registrador, "$a0") == 0) || (strcmp(registrador, "$4") == 0)) {
	return("00100");
    }
    if ((strcmp(registrador, "$a1") == 0) || (strcmp(registrador, "$5") == 0)) {
    	return("00101");
    }
    if ((strcmp(registrador, "$a2") == 0) || (strcmp(registrador, "$6") == 0)) {
    	return("00110");
    }
    if ((strcmp(registrador, "$a3") == 0) || (strcmp(registrador, "$7") == 0)) {
    	return("00111");
    }
    if ((strcmp(registrador, "$t0") == 0) || (strcmp(registrador, "$8") == 0)) {
    	return("01000");
    }
    if ((strcmp(registrador, "$t1") == 0) || (strcmp(registrador, "$9") == 0)) {
	return("01001");
    }
    if ((strcmp(registrador, "$t2") == 0) || (strcmp(registrador, "$10") == 0)) {
    	return("01010");
    }
    if ((strcmp(registrador, "$t3") == 0) || (strcmp(registrador, "$11") == 0)) {
    	return("01011");
    }
    if ((strcmp(registrador, "$t4") == 0) || (strcmp(registrador, "$12") == 0)) {
    	return("01100");
    }
    if ((strcmp(registrador, "$t5") == 0) || (strcmp(registrador, "$13") == 0)) {
	return("01101");
    }
    if ((strcmp(registrador, "$t6") == 0) || (strcmp(registrador, "$14") == 0)) {
    	return("01110");
    }
    if ((strcmp(registrador, "$t7") == 0) || (strcmp(registrador, "$15") == 0)) {
    	return("01111");
    }
    if ((strcmp(registrador, "$s0") == 0) || (strcmp(registrador, "$16") == 0)) {
    	return("10000");
    }
    if ((strcmp(registrador, "$s1") == 0) || (strcmp(registrador, "$17") == 0)) {
    	return("10001");
    }
    if ((strcmp(registrador, "$s2") == 0) || (strcmp(registrador, "$18") == 0)) {
    	return("10010");
    }
    if ((strcmp(registrador, "$s3") == 0) || (strcmp(registrador, "$19") == 0)) {
    	return("10011");
    }
    if ((strcmp(registrador, "$s4") == 0) || (strcmp(registrador, "$20") == 0)) {
    	return("10100");
    }
    if ((strcmp(registrador, "$s5") == 0) || (strcmp(registrador, "$21") == 0)) {
    	return("10101");
    }
    if ((strcmp(registrador, "$s6") == 0) || (strcmp(registrador, "$22") == 0)) {
    	return("10110");
    }
    if ((strcmp(registrador, "$s7") == 0) || (strcmp(registrador, "$23") == 0)) {
    	return("10111");
    }
    if ((strcmp(registrador, "$t8") == 0) || (strcmp(registrador, "$24") == 0)) {
    	return("11000");
    }
    if ((strcmp(registrador, "$t9") == 0) || (strcmp(registrador, "$25") == 0)) {
    	return("11001");
    }
    if ((strcmp(registrador, "$k0") == 0) || (strcmp(registrador, "$26") == 0)) {
    	return("11010");
    }
    if ((strcmp(registrador, "$k1") == 0) || (strcmp(registrador, "$27") == 0)) {
    	return("11011");
    }
    if ((strcmp(registrador, "$gp") == 0) || (strcmp(registrador, "$28") == 0)) {
    	return("11100");
    }
    if ((strcmp(registrador, "$sp") == 0) || (strcmp(registrador, "$29") == 0)) {
    	return("11101");
    }
    if ((strcmp(registrador, "$fp") == 0) || (strcmp(registrador, "$30") == 0)) {
    	return("11110");
    }
    if ((strcmp(registrador, "$ra") == 0) || (strcmp(registrador, "$31") == 0)) {
    	return("11111");
    }
    return(NULL);
}

void conta_memoria(char *instrucao) {
    int i = 0;    
    char funcao[30];
    if (verifica_label(instrucao)) {
        remove_label(instrucao);
    }
    while ((instrucao[i] != ' ') && (instrucao[i] != '\0')) {
        funcao[i] = instrucao[i];            
        i++;
    }
    funcao[i] = '\0';
    if ((strcmp(funcao, "li") == 0) || (strcmp(funcao, "la") == 0) || (strcmp(funcao, "bge") == 0) || (strcmp(funcao, "bgt") == 0) || (strcmp(funcao, "ble") == 0) || (strcmp(funcao, "blt") == 0)) {
        mem_text += 4;
    }
    mem_text += 4; /* Se for pseudo-instruçao, recebe +8; */
    return;
}
/* ----------------------------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------- FUNÇÕES ÚTEIS ---------------------------------------------- */
/* ----------------------------------------------------------------------------------------------------------- */
void bloco_instrucao(char *instrucao, char bloco[][30], int qtd_bloco) { /* Como cada instrução tem um número diferente de parâmetros, essa função finaliza a string dependendo da quantidade de blocos que serão utilizados; */
    int i = 0, j = 0;
    while ((instrucao[i] != ' ') && (instrucao[i] != '\0')) { /* Lê novamente a função; */
        i++;
    }
    i++;
    while ((instrucao[i] != ' ') && (instrucao[i] != ',') && (instrucao[i] != '\0')) {
		bloco[0][j] = instrucao[i];	
		i++;	
		j++;
    }
    bloco[0][j] = '\0';		
    j = 0;
    if (qtd_bloco >= 2) {
		i += 2;
		while ((instrucao[i] != ' ') && (instrucao[i] != ',') && (instrucao[i] != '\0')) {
			bloco[1][j] = instrucao[i];	
			i++;	
			j++;
		}
		bloco[1][j] = '\0';
    }
    else {
		bloco[1][0] = '\0';
		bloco[2][0] = '\0';
    }
    j = 0;
    if (qtd_bloco == 3) {
        i += 2;
		while ((instrucao[i] != ' ') && (instrucao[i] != '\0')) {
            bloco[2][j] = instrucao[i];	
            i++;	
            j++;
		}
		bloco[2][j] = '\0';
    }
    else {
		bloco[2][0] = '\0';
    }
    return;
}

void bloco_especial(char *instrucao, char bloco[][30]) { /* Pega "instrução rt, imm(rs)" e trata os parenteses da funçao; */
    int i = 0, j = 0;
    while ((instrucao[i] != ' ') && (instrucao[i] != '\0')) { /* Lê novamente a função; */
        i++;
    }
    i++;
    while (instrucao[i] != ',') {
		bloco[0][j] = instrucao[i];	
		i++;	
		j++;
    }
    bloco[0][j] = '\0';		
    j = 0;
    i += 2;
    while (instrucao[i] != '(') {
        bloco[2][j] = instrucao[i];
        i++;	
        j++;
    }
    bloco[2][j] = '\0';		
    i++;
    j = 0;
    while (instrucao[i] != ')') {
        bloco[1][j] = instrucao[i];
        i++;	
        j++;
    }
    bloco[1][j] = '\0';	
    return;
}

void upper_lower(char *big, char *upper, char *lower) { /* Dependendo da função ela utiliza a parte upper que são os 16 bits maios significativos ou a parte lower, que são os 16 bits menos significativos; */
    int i, j = 0;
    for (i = 0; i < 16; i++) {
        upper[i] = big[i];
    }
    upper[16] = '\0';
    for (i = 16; i < 32; i++) {
        lower[j] = big[i];
        j++;
    }
    lower[16] = '\0';
    return;
}

void quebra_pseudo(char *pseudo_instrucao, char *pseudo1, char *pseudo2) { /* Como uma pseudo instrução, nos nossos casos tratados são duas instruçoes, parte ela em dois para poder retornar cada instrução separada; */
    int i, j = 0;
    for (i = 0; i < 32; i++) {
        pseudo1[i] = pseudo_instrucao[i];
    }
    pseudo1[32] = '\0';
    for (i = 32; i < 64; i++) {
        pseudo2[j] = pseudo_instrucao[i];
        j++;
    }
    pseudo2[64] = '\0';
    return;
}

int conversor(char *numero, int qtd_bit, char *resultado, int tipo) { /* tipo(Decimal) = 0; tipo(Hexa) = 1; */
    int i, j, tamanho, numero_decimal, quociente, resto;
    int *vetor_numero;
    if (tipo) { /* Se for Hexadecimal, converte para decimal; */
        tamanho = strlen(numero);
        vetor_numero = (int*) malloc(tamanho * sizeof(int));
        for (i = 0; i < tamanho; i++) {
            if (numero[i] >= '0' && numero[i] <= '9') /* Se for um número decimal, converte string para int; */
				vetor_numero[i] = (int) numero[i] - (int) '0'; /* Valor ASCII de numero[i] - valor ASCII de 0; */
            else {
				switch(tolower(numero[i])) { /* Por ser um número hexadecimal, os números 10 à 15 são representados por letras, e se faz essa conversão para decimal para poder converter para binário; */
                    case 'a': vetor_numero[i] = 10; break;
                    case 'b': vetor_numero[i] = 11; break;
                    case 'c': vetor_numero[i] = 12; break;
                    case 'd': vetor_numero[i] = 13; break;
                    case 'e': vetor_numero[i] = 14; break;
                    case 'f': vetor_numero[i] = 15; break;
                    default: vetor_numero[i] = -1; break;
				}
            }
        }
        j = 0;
        for (i = (tamanho - 1); i > -1; i--) {
            numero_decimal += vetor_numero[j] * pow(16, i);		
            j++;
		}
        free(vetor_numero); 
    }
    else { /* Converte numero para inteiro; */
        numero_decimal = atoi(numero);
    }    
    if (numero_decimal >= (pow(2, qtd_bit))) {
        return(1); /* OVERFLOW; */
    }      
    for (i = 0; i < qtd_bit; i++) {
        resultado[i] = '0';
    }
    resultado[qtd_bit] = '\0';
    i = qtd_bit;
    quociente = numero_decimal;
    while (quociente != 0) {
		quociente /= 2;
		resto = numero_decimal - quociente * 2;
        numero_decimal = quociente;
        resultado[i - 1] = (char) (resto + 48);
        i--;
    }   
    return(0);
}

void bin_hexa(char* instrucao, char* instrucaosaida) { /* Converte um binário para um hexadecimal; */
    int i, j = 0, tamanho;
    char bin[5];
    tamanho = strlen(instrucao);
    for (i = 0; i < tamanho; i += 4) { /* Separa o binário em blocos de 4 bits, pois um hexadecimal é representado de 4 em 4 bits, quando convertido para um binário; */
    	bin[0] = instrucao[i];
		bin[1] = instrucao[i + 1]; 
		bin[2] = instrucao[i + 2];
		bin[3] = instrucao[i + 3];
        bin[4] = '\0';
		if (strcmp(bin,"0000") == 0) {/* Contém todas as possibilidades possíveis de representação de um hexadecimal com apenas 4 bits; */
            instrucaosaida[j] = '0';		
		}
		if (strcmp(bin,"0001") == 0) {
            instrucaosaida[j] = '1';		
		}
    	if (strcmp(bin,"0010") == 0) {
            instrucaosaida[j] = '2';		
		}
    	if (strcmp(bin,"0011") == 0) {
            instrucaosaida[j] = '3';		
		}
    	if (strcmp(bin,"0100") == 0) {
            instrucaosaida[j] = '4';		
		}
    	if (strcmp(bin,"0101") == 0) {
            instrucaosaida[j] = '5';		
		}
    	if (strcmp(bin,"0110") == 0) {
            instrucaosaida[j] = '6';		
		}
		if (strcmp(bin,"0111") == 0) {
            instrucaosaida[j] = '7';		
		}
    	if (strcmp(bin,"1000") == 0) {
            instrucaosaida[j] = '8';		
		}
		if (strcmp(bin,"1001") == 0) {
            instrucaosaida[j] = '9';		
		}
        if (strcmp(bin,"1010") == 0) {
            instrucaosaida[j] = 'a';		
		}
    	if (strcmp(bin,"1011") == 0) {
            instrucaosaida[j] = 'b';		
		}
    	if (strcmp(bin,"1100") == 0) {
            instrucaosaida[j] = 'c';	
		}
		if (strcmp(bin,"1101") == 0) {
            instrucaosaida[j] = 'd';	
        }
		if (strcmp(bin,"1110") == 0) {
            instrucaosaida[j] = 'e';		
		}
        if (strcmp(bin,"1111") == 0) {
            instrucaosaida[j] = 'f';		
        }
	j++;
    }
    instrucaosaida[j] = '\0';
    return;
}

void int_string(int numero, char *string) { /* Converte um número inteiro para uma string */
    sprintf(string, "%d", numero);
    return;
}

int verifica_label(char *instrucao) { /* Verifica se a instrução é um label e remove comentario; */
    int i, verificador = 0;
    for (i = 0; i < strlen(instrucao); i++) {
    	if (instrucao[i] == ':') {
            verificador = 1;
            break;
		}
	}
    return(verificador);
}

void remove_label(char *instrucao) {
    int i = 0, j = 0, ativador = 0;
    while(instrucao[i] != '\0') { /* Remove o label da instrucao; */
        if (instrucao[i] == ':') {
            ativador = 1;
            i += 2; /* pula o ':' e o ' '; */
        }
        if (ativador) {
            instrucao[j] = instrucao[i];
            j++;
        }
        i++;
    } 
    instrucao[j] = '\0';
    return;
}

void escreve_label(FILE *arq_saida, int boolean) { 		/* Pega o label que está guardado na lista e escreve no arquivo de saída; */
    int i;												/* Data = 0; Text = 1; */
    char *nome, string[100];
    if (boolean) {
        for (i = 0; i < retorna_tamanho(lista_text); i++) {
            nome = retorna_label_nome(lista_text, i); /* Pega o nome do label */
            sprintf(string, "= %s %d\n", nome, retorna_label_endereco(lista_text, nome)); /* coloca em apenas uma string o nome do label e o endereco; */
            escreve_arquivo(arq_saida, string);
        }   
    }
    else {
        for (i = 0; i < retorna_tamanho(lista_data); i++) {
            nome = retorna_label_nome(lista_data, i);
            sprintf(string, "= %s %d\n", nome, retorna_label_endereco(lista_data, nome));
            escreve_arquivo(arq_saida, string);
        }   
    }
    
}

void go_text(FILE *arq) {  /* Acha o início da área de código; */
    char instrucao[100];
    instrucao[99] = '\0';
    retorna_instrucao(arq, instrucao, sizeof(instrucao));
    formata_instrucao(instrucao);
    while (strcmp(instrucao, ".text") != 0) {
        retorna_instrucao(arq, instrucao, sizeof(instrucao));
        formata_instrucao(instrucao);
    }
    return;
}

void formata_instrucao(char *instrucao) { /* Retira possíveis erros de entrada, como espaços duplicados, tabs, etc; */
    int i, j;
    for (i = 0; instrucao[i] != '\0'; i++) { /* Transforma tab em espaço; */
        if (instrucao[i] == '\t') {
			instrucao[i] = ' ';
        }
    }
    for (i = 0; instrucao[i] != '\0'; i++) { /* Remove espaços duplicados, espaços antes de virgula; */
        while (((instrucao[i] == ' ') && (instrucao[i + 1] == ' ')) || ((instrucao[i] == ' ') && (instrucao[i + 1] == ','))) {
            for (j = i; instrucao[j] != '\0'; j++) {
                instrucao[j] = instrucao[j + 1];
            }
        }
    }
    for (i = 0; instrucao[i] != '\0'; i++) { /* Adiciona um espaço após virgula; */
        if((instrucao[i] == ',') && (instrucao[i + 1] != ' ')) {
            for (j = strlen(instrucao) - 1; j != i; j--) {
                instrucao[j + 1] = instrucao[j];
            }
            instrucao[j + 1] = ' ';
        }
    }
    if (instrucao[0] == ' ') { /* Remove espaço do inicio; */
        for (i = 0; i < strlen(instrucao) - 1; i++) {
            instrucao[i] = instrucao[i + 1];
        }
        instrucao[i]= '\0';
    }    
    for (i = 0; i < strlen(instrucao); i++) { /* Remove comentario, \r e \n; */
		if ((instrucao[i] == '#') || (instrucao[i] == '\n') || (instrucao[i] == '\r')) {
            instrucao[i] = '\0';
            break;
		}
    }
    if (instrucao[strlen(instrucao) - 1] == ' ') { /* Remove espaço do final; */
        instrucao[strlen(instrucao) - 1] = '\0';
    }
    return;
}
/* ----------------------------------------------------------------------------------------------------------- */