//
// Created by luiza on 21/08/2024.
//

#include "diassembler.h"
#include <stdio.h>

struct OpcodesAssembly {
    char opcodeSimbolico[30];
    int opcodeNumerico;
    int tamanhoEmPalavras;
    char acao[100];
    int numeroDeOperandos;
};

struct OpcodesAssembly tabelaDeInstrucoes[15] = {
    {"ADD", 1, 2, "ACC <- ACC + mem(OP)", 1},
    {"SUB", 2, 2, "ACC <- ACC - mem(OP)", 1},
    {"MUL", 3, 2, "ACC <- ACC× mem(OP)", 1},
    {"MULT", 3, 2, "ACC <- ACC × mem(OP)", 1},
    {"DIV", 4, 2, "ACC <- ACC ÷ mem(OP)", 1},
    {"JMP", 5, 2, "PC <- OP", 1},
    {"JMPN", 6, 2, "Se ACC<0 então PC <- OP", 1},
    {"JMPP", 7, 2, "Se ACC>0 então PC <- OP", 1},
    {"JMPZ", 8, 2, "Se ACC=0 então PC <- OP", 1},
    {"COPY", 9, 3, "mem(OP2) <- mem(OP1)", 2},
    {"LOAD", 10, 2, "ACC <- mem(OP)", 1},
    {"STORE", 11, 2, "mem(OP) <- ACC", 1},
    {"INPUT", 12, 2, "mem(OP) <- entrada", 1},
    {"OUTPUT", 13, 2, "saída <- mem(OP)", 1},
    {"STOP", 14, 1, "Suspende a execução", 0}
};

int main(void)
{
    printf("Hello, World!\n");
    return 0;
}
