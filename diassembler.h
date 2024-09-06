//
// Created by luiza on 21/08/2024.
//

// diasassembler.h
#ifndef DIASSEMBLER_H
#define DIASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura para armazenar os códigos de operação
struct OpcodesAssembly {
    char opcodeSimbolico[30];
    int opcodeNumerico;
    int tamanhoEmPalavras;
    char acao[100];
    int numeroDeOperandos;
};

// Estrutura para armazenar o código objeto com endereços
struct AddressObjCode {
    int address;
    int objCode;
};

// Estrutura para o programa assembly inventado
struct programAssemblyInventado {
    char opcode[50];
    char operand1[50];
    char operand2[50];
};

// Estrutura para dados do assembly inventado
struct dadosAssemblyInventado {
    int addressReferred;                // Endereço referenciado
    int addressVariableDeclared;        // Endereço da variável ou valor da constante
    char type[50];                      // Tipo da variável
};

// Estrutura para a seção de dados
struct dataSectionAI {
    int address;
    char name[50];
    int value;
};

// Estrutura para a lista de variáveis
struct variablesList {
    char name[50];
    int type; // 1 = label, 2 = const, 3 = space
    int value;    // Endereço para label, valor para const
    int address;
};

// Estrutura para o retorno da função objCodeTreatment
struct ReturnValues {
    struct programAssemblyInventado program1[100];
    struct dadosAssemblyInventado dadosAI[100];
    int lineCount;
    int dadosIndex;
};

// Estrutura para o mapeamento Inventado para IA-32
struct InventadoParaIA32 {
    int opcodeAI;
    char codigoIA32[200];
};

// Tabelas de instruções e dicionário de IA-32
extern struct OpcodesAssembly tabelaDeInstrucoes[17];
extern struct InventadoParaIA32 dicionarioIA32[16];
extern char funcoes[3700];

// Funções utilitárias
int sizeOfList(char **list);
int isInList(char *str, char *lista[]);
int sizeOfListINT(int *list[]);
int isInListINT(int integer, int *lista[]);

// Função principal para tratamento do código objeto
struct ReturnValues objCodeTreatment(char* filename);

#endif // DIASSEMBLER_H
