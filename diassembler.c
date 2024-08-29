//
// Created by luiza on 21/08/2024.
//

#include "diassembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct OpcodesAssembly {
    char opcodeSimbolico[30];
    int opcodeNumerico;
    int tamanhoEmPalavras;
    char acao[100];
    int numeroDeOperandos;
};

struct AddressObjCode {
    int address;
    int objCode;
};

struct programAssemblyInventado{
    char opcode[50];
    char operand1[50];
    char operand2[50];
};

struct dadosAssemblyInventado{
    int addressReferred;                // Endereço que ela foi chamada
    int addressVariableDeclared;        // Endereço em que a label está ou valor da const/space
    char type[50];                      // Tipo de variável
};

struct dataSectionAI{
    int address;
    char name[50];
    int value;
};

struct variablesList{
    char name[50];
    int type; //If ==1 it is label, ==2 it is const, ==3 it is space;
    int value;    //if it is label it has the address to return to, if it is const is the constant value
    int address;
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

// Returns the size/length of list in char
int sizeOfList(char **list) {
    int size = 0;
    while (list[size] != NULL) {
        size++;
    }
    return size;
}

// Checks if a string is in a list in char
int isInList(char *str, char *lista[]) {
    int size = sizeOfList(lista);
    for (int i = 0; i < size; i++) {
        if (strcmp(str, lista[i]) == 0) {
            return 1; // String found in the list
        }
    }
    return 0; // String not found in the list
}

// Returns the size/length of list in integer
int sizeOfListINT(int *list[]) {
    int size = 0;
    while (list[size] != NULL) {
        size++;
    }
    return size;
}

// Verifica se um inteiro está na lista
int isInListINT(int integer, int *lista[]) {
    int size = sizeOfListINT(lista);
    for (int i = 0; i < size; i++) {
        if (integer == *lista[i]) {
            return 1; // Inteiro encontrado na lista
        }
    }
    return 0; // Inteiro não encontrado na lista
}

void objCodeTreatment(char* filename){
    FILE *file;
    struct AddressObjCode objCode[100];
    struct programAssemblyInventado program1[100];
    struct dadosAssemblyInventado dadosAI[100];
    char line[1000], *token;
    int *listOfOperands[100] = {NULL};
    int opIndex = 0;
    int dadosIndex = 0;
    int lineCount = 0;
    int address = 0;
    int jmpModule = -1;
    int opcodeBool = 0;
    int quantOperandos = 0;
    int breakAddressing = 0;
    int found = 0;
    // Open the file for reading

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    while (fgets(line, 1000, file) != NULL) {
        char *token = strtok(line, " ");
        while (token != NULL) {
            //printf("Token: %s\n", token);
            objCode[address].address = address;
            objCode[address].objCode = atoi(token);
            if (address == 0){
                opcodeBool = 1;                        // Se for o endereço 00 sempre vamos assumir que é um opcode!!!
            }
            if (isInListINT(address, listOfOperands) == 1){
                strcpy(program1[lineCount].operand1, token);
                //printf("%s|%s|%s --const e space\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                for (int i = 0; i < dadosIndex; i++){
                    if (address == dadosAI[dadosIndex].addressVariableDeclared){
                        dadosAI[dadosIndex].addressReferred = atoi(token);
                    }
                }
                lineCount++;
                breakAddressing = 1;
            }
            if (objCode[address].objCode > 0 && objCode[address].objCode < 15 && opcodeBool == 1 && breakAddressing == 0){
                strcpy(program1[lineCount].opcode, token);
                // Caso de ADD, SUB, MUL, DIV, STORE, INPUT, OUT
                if ((objCode[address].objCode > 0 && objCode[address].objCode < 5) || (objCode[address].objCode > 9 && objCode[address].objCode < 14) ){
                    quantOperandos = 1;
                }
                // Caso de JMP, JMPN, JMPP, JPMZ
                if (objCode[address].objCode > 4 && objCode[address].objCode < 9){
                    quantOperandos = 1;
                    jmpModule = 1;
                    //printf("Jmp Module activated %d\n", jmpModule);
                }
                // Caso de COPY
                if (objCode[address].objCode == 9){
                    quantOperandos = 2;
                }
                // Caso de STOP
                if (objCode[address].objCode == 14){
                    quantOperandos = 0;
                    lineCount += 1;
                }

                opcodeBool = 0;
                breakAddressing = 1;
            }
            if (quantOperandos > 0 && opcodeBool == 0 && breakAddressing == 0){            // Se for pra ler um operando
                if (isInListINT(address, listOfOperands) == 0){
                    if (jmpModule != 1){
                        listOfOperands[opIndex] = malloc(sizeof(int));
                        *listOfOperands[opIndex] = atoi(token);
                        opIndex++;
                        for (int i = 0; i < dadosIndex; i++){
                            if (atoi(token) == dadosAI[i].addressVariableDeclared){
                                if (strcmp(dadosAI[i].type, "CONST") == 0){
                                    strcpy(dadosAI[i].type, "SPACE");
                                }
                                found = 1;
                            }
                        }
                        if (found == 0){
                            dadosAI[dadosIndex].addressVariableDeclared = atoi(token);
                            if (atoi(program1[lineCount].opcode) == 11 || atoi(program1[lineCount].opcode) == 12){
                                strcpy(dadosAI[dadosIndex].type, "SPACE");
                            } else {
                                strcpy(dadosAI[dadosIndex].type, "CONST");
                            }
                            dadosIndex++;
                        }

                    } else {
                        for (int i = 0; i < dadosIndex; i++){
                            if (atoi(token) == dadosAI[i].addressReferred){
                                found = 1;
                            }
                        }
                        if (found == 0){
                            dadosAI[dadosIndex].addressVariableDeclared = address;
                            strcpy(dadosAI[dadosIndex].type, "LABEL");
                            dadosAI[dadosIndex].addressReferred = atoi(token);
                            dadosIndex++;
                        }
                    }
                    found = 0;
                }
                if (quantOperandos == 2){
                    strcpy(program1[lineCount].operand2, token);
                    quantOperandos -= 1;
                    opcodeBool = 0;
                } else if (quantOperandos == 1){
                    strcpy(program1[lineCount].operand1, token);
                    //printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                    quantOperandos -= 1;
                    lineCount += 1;
                    opcodeBool = 1;
                }
                jmpModule = 0;
                breakAddressing = 1;
            }
            if (breakAddressing == 1){
                breakAddressing = 0;
            }
            address += 1;
            token = strtok(NULL, " ");
        }
    }
    fclose(file);
    printf("Var declarada//Referência//Tipo\n");
    for (int c = 0; c < dadosIndex; c++){
        printf("%d %d %s\n",dadosAI[c].addressVariableDeclared, dadosAI[c].addressReferred, dadosAI[c].type);
    }
    /*
    printf("\n");
    for (int a = 0; a < opIndex; a++){
        printf("%d %d\n", a, *listOfOperands[a]);
    }
     */
    printf("\n");
    for (int b = 0; b < lineCount; b++){
        printf("%d %s|%s|%s\n", b, program1[b].opcode, program1[b].operand1, program1[b].operand2);
    }

    // Free allocated memory when done
    for (int i = 0; i < opIndex; i++) {
        free(listOfOperands[i]);
    }
}



int main(int argc, char** argv) {
    char* filename = argv[1];
    objCodeTreatment(filename);
    return 0;
}
