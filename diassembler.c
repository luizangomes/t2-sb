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

void tradutor(char* filename){
    FILE *file;
    struct AddressObjCode objCode[100];
    struct programAssemblyInventado program1[100];
    char line[1000], *token;
    int listOfOperands[100], opIndex=0;
    int lineCount = 0;
    int address = 0;
    int opcode = 0, stopOpcode = 0;
    int opcodeBool = 0, opcodeOperands = 0, readOperand = 0;
    // Open the file for reading

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    while (fgets(line, 1000, file) != NULL) {
        printf("%s\n", line);
        char *token = strtok(line, " ");
        while (token != NULL) {
            objCode[address].address = address;
            objCode[address].objCode = atoi(token);
            if (objCode[address].objCode < 15 && objCode[address].objCode > 0 && stopOpcode == 0){
                opcodeBool = 1;
                opcode = objCode[address].objCode;
                strcpy(program1[lineCount].opcode, token);
                if ((opcode > 0 && opcode < 9) || (opcode > 9 && opcode < 14)){
                    opcodeOperands = 1;
                    readOperand = 1;
                } else if (opcode == 9){
                    opcodeOperands = 2;
                    readOperand = 1;
                } else if (opcode == 14){
                    opcodeOperands = 0;
                    readOperand = 0;
                    stopOpcode = 1;
                    strcpy(program1[lineCount].opcode, token);
                    strcpy(program1[lineCount].operand1,  "");
                    strcpy(program1[lineCount].operand2, "");
                    printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                    lineCount += 1;
                }
            }
            if (readOperand > 0 && opcodeBool == 0){
                opcode = 0;
                listOfOperands[opIndex] = atoi(token);
                opIndex++;
                if (opcodeOperands == 1){
                    if (strcmp(program1[lineCount].opcode, "9") == 0){
                        strcpy(program1[lineCount].operand2, token);

                    } else {
                        strcpy(program1[lineCount].operand1,  token);
                        strcpy(program1[lineCount].operand2, "");
                    }
                    printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                    lineCount += 1;
                    opcodeOperands -= 1;
                }
                if (opcodeOperands == 2){
                    strcpy(program1[lineCount].operand1, token);
                    opcodeOperands -= 1;
                }
            }
            if (stopOpcode == 1 && opcodeBool == 0){
                strcpy(program1[lineCount].opcode, "");
                strcpy(program1[lineCount].operand1,  token);
                strcpy(program1[lineCount].operand2, "");
                printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                lineCount += 1;
            }
            //printf("        %d : %d\n", objCode[address].address, objCode[address].objCode);
            address += 1;
            opcodeBool = 0;
            token = strtok(NULL, " ");
        }
    }
}


int main(int argc, char** argv) {
    char* filename = argv[1];
    tradutor(filename);
    return 0;
}
