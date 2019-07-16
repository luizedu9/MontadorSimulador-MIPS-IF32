/* -------------------------------------------------------
 * File: executa.c
 * Autores: Álissom Vieira da Cunha - Luiz Eduardo Pereira
 * Matrícula: 0021623 - 0021619
 * Data: 28/06/2017
------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "registrador.h"
#include "simulador.h"
#include "busca.h"

void executa(int *registradores, int instrucao, char tipo, int id_instrucao) {
    int rd, rs, rt, shamt, immediate, address;
    if (tipo == 'R') {
        rs = verifica_registrador(instrucao, 1);
        rt = verifica_registrador(instrucao, 2);
        rd = verifica_registrador(instrucao, 3);
        shamt = verifica_registrador(instrucao, 4);
        if (id_instrucao == 32) { /* add; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rs) + retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 33) { /* addu; */
            insere_registrador(registradores, rd, (unsigned) ((unsigned) retorna_registrador(registradores, rs) + (unsigned) retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 34) { /* sub; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rs) - retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 35) { /* subu; */
            insere_registrador(registradores, rd, (unsigned) ((unsigned) retorna_registrador(registradores, rs) - (unsigned) retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 36) { /* and; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rs) & retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 37) { /* or; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rs) | retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 38) { /* xor; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rs) ^ retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 39) { /* nor; */
            insere_registrador(registradores, rd, !(retorna_registrador(registradores, rs) | retorna_registrador(registradores, rt)));
            return;
        }
        if (id_instrucao == 42) { /* slt; */
            if (retorna_registrador(registradores, rs) < retorna_registrador(registradores, rt)) {
                insere_registrador(registradores, rd, 1);
            }
            else {
                insere_registrador(registradores, rd, 0);
            }
            return;
        }
        if (id_instrucao == 43) { /* sltu; */
            if ((unsigned) retorna_registrador(registradores, rs) < (unsigned) retorna_registrador(registradores, rt)) {
                insere_registrador(registradores, rd, (unsigned) 1);
            }
            else {
                insere_registrador(registradores, rd, (unsigned) 0);
            }
            return;
        }
        if (id_instrucao == 0) { /* sll; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) << shamt)); /* IGUAL AO SRA??*********/
            return;
        }
        if (id_instrucao == 2) { /* srl; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) >> shamt));
            return;
        }
        if (id_instrucao == 3) { /* sra; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) << shamt));
            return;
        }
        if (id_instrucao == 4) { /* sllv; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) << retorna_registrador(registradores, rs)));
            return;
        }
        if (id_instrucao == 6) { /* srlv; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) >> retorna_registrador(registradores, rs)));
            return;
        }
        if (id_instrucao == 7) { /* srav; */
            insere_registrador(registradores, rd, (retorna_registrador(registradores, rt) >> retorna_registrador(registradores, rs)));
            return;
        }   
        if (id_instrucao == 8) { /* jr; */
            altera_program_counter(retorna_registrador(registradores, rs));
            return;
        }
        if (id_instrucao == 12) { /* syscall; */
            if (retorna_registrador(registradores, 2) == 1) { /* print integer; */
                printf("%d\n", retorna_registrador(registradores, 4));  /* 4 - %a0; */
                return;
            }
            if (retorna_registrador(registradores, 2) == 4) { /* print string; */   
                return;
            }
            if (retorna_registrador(registradores, 2) == 5) { /* read integer; */
                int integer;
                scanf("%d", &integer);
                insere_registrador(registradores, 2, integer); /* 2 - $v0; */ 
                return;
            }
            if (retorna_registrador(registradores, 2) == 8) { /* read string; */
                return;
            }
            if (retorna_registrador(registradores, 2) == 10) { /* exit; */
                exit(0);
            }
            if (retorna_registrador(registradores, 2) == 11) { /* print character; */
                printf("%c\n", retorna_registrador(registradores, 4));  /* 4 - %a0; */
                return;
            }
            if (retorna_registrador(registradores, 2) == 12) { /* read character; */
                char character;
                scanf("%c", &character);
                insere_registrador(registradores, 2, character); /* 2 - $v0; */ 
                return;
            }
            if (retorna_registrador(registradores, 2) == 34) { /* print integer in hexadecimal; */
                printf("%x", (unsigned) retorna_registrador(registradores, 4));
                return;
            }
        }
    }
    if (tipo == 'J') {
        address = verifica_address(instrucao);
        if (id_instrucao == 2) { /* j; */
            altera_program_counter((retorna_program_counter() & 0xf0000000) | address);
            return;
        }
        if (id_instrucao == 3) { /* jal; */
            altera_program_counter((retorna_program_counter() & 0xf0000000) | address);
            insere_registrador(registradores, 31, (retorna_program_counter() + 8));
            return;
        }
    }
    if (tipo == 'I') {
        rs = verifica_registrador(instrucao, 1);
        rt = verifica_registrador(instrucao, 2);
        immediate = verifica_imediate(instrucao);
        if (id_instrucao == 32) { /* lb; */
            rt = (retorna_memoria(rs + immediate) >> 12);
            return;
        }
        if (id_instrucao == 35) { /* lw; */
            rt = retorna_memoria(rs + immediate);
            return;
        }
        if (id_instrucao == 36) { /* lbu; */
            rt = (unsigned) ((unsigned) retorna_memoria(rs + immediate) >> (unsigned) 12);
            return;
        }
        if (id_instrucao == 40) { /* sb; */
            altera_memoria((rs + immediate), (0xff & rt));
            return;
        }
        if (id_instrucao == 43) { /* sw; */
            altera_memoria((rs + immediate), rt);
            return;
        }
        if (id_instrucao == 8) { /* addi; */
            insere_registrador(registradores, rt, (retorna_registrador(registradores, rs) + immediate));
            return;
        }
        if (id_instrucao == 9) { /* addiu; */
            insere_registrador(registradores, rt, (unsigned) ((unsigned) retorna_registrador(registradores, rs) + (unsigned) immediate));
            return;
        }
        if (id_instrucao == 10) { /* slti; */
            if (retorna_registrador(registradores, rs) < immediate) {
                insere_registrador(registradores, rt, 1);
            }
            else {
                insere_registrador(registradores, rt, 0);
            }
            return;
        }
        if (id_instrucao == 12) { /* andi; */
            insere_registrador(registradores, rt, (retorna_registrador(registradores, rs) & immediate));
            return;
        }
        if (id_instrucao == 13) { /* ori; */
            insere_registrador(registradores, rt, (retorna_registrador(registradores, rs) | immediate));            
            return;
        }
        if (id_instrucao == 14) { /* xori; */
            insere_registrador(registradores, rt, (retorna_registrador(registradores, rs) ^ immediate));
            return;
        }   
        if (id_instrucao == 15) { /* lui; */
            insere_registrador(registradores, rt, (immediate << 16));
            return;
        }   
        if (id_instrucao == 1) { /* bltz; */
            if (rs < 0) {
                altera_program_counter(immediate);
            }
            return;
        }
        if (id_instrucao == 4) { /* beq; */
            if (rs == rt) {
                altera_program_counter(immediate);
            }
            return;
        }
        if (id_instrucao == 5) { /* bne; */
            if (rs != rt) {
                altera_program_counter(immediate);
            }
            return;
        }
    }
    return;
}