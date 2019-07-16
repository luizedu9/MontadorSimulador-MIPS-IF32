# MontadorSimulador-MIPS-IF32

Instituto Federal de Educação, Ciência e Tecnologia de Minas Gerais, IFMG - Campus Formiga

Ciência da Computação

Montador e Simulador para um processador teórico baseado no MIPS (Microprocessor without Interlocked Pipeline Stages). Disciplina de 'Arquitetura e Organização de Computadores'.

Autores: Álissom Vieira da Cunha, Luiz Eduardo Pereira.

# Objetivo:

O objetivo deste trabalho era criar um montador e um simulador de um processador teórico MIPS-IF32 baseado no MIPS.

O montador recebe um algoritmo em Assembly e converte para linguagem de maquina.

O simulador recebe a linguagem de maquina e simula as instruções como se fosse em um processador.

Informações completas deste trabalho em Proposta_Montador.pdf, Relatório_Montador.pdf, Proposta_Simulador.pdf, Relatório_Simulador.pdf. 

# Execução:

No exemplo contido no projeto, foi realizado operações matematicas.

Montador:

gcc -Wall *.c -o montador -lm

./montador exemplo.txt saida.txt

Tradução do Assembly para linguagem natural em 'exemplo.txt':

soma 10 + 10;

subtrai 5 - 50;


Simulador:

gcc -Wall *.c -o simulador

./simulador exemplo.txt

Saida de 'exemplo.txt':

20

-45
