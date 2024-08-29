//
// Created by luiza on 29/08/2024.
//

#include "tradutor.h"
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

struct operandsAssemblyInventado{
    int addressMentioned;
    int variableAddress;
    char name[50];
    int value;
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

void tradutor(char* filename){
    FILE *file;
    struct AddressObjCode objCode[100];
    struct programAssemblyInventado program1[100];
    struct operandsAssemblyInventado operandsAI[100];
    struct dataSectionAI dataSection[100];
    struct variablesList varList[100];
    char line[1000], *token, *varSimpleList[100];
    int *listOfOperands[100] = {NULL};
    int  opIndex=0, dsIndex = 0, vlIndex = 0;
    int lineCount = 0, slIndex = 0;
    int address = 0;
    int opcode = 0, stopOpcode = 0, stopAddress = 10000, firstVariableAddress = 0, lastVariableAddress = 0;
    char varName[50] = "", varTemp[50] = "";
    int varInt = 0, rewindFLV = 0, readingAddresses = 0, jmpModule = -1;
    int opcodeBool = 0, opcodeOperands = 0, readOperand = 0, jmpLabel = 0, constVar = 0, spaceVar = 0;
    // Open the file for reading

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    for (int i = 0; i < 100; i++) {
        free(varSimpleList[i]);
    };

    // Initialize variable list
    for (int i = 0; i < 100; i++) {
        varSimpleList[i] = malloc(50 * sizeof(char));
        if (varSimpleList[i] == NULL) {
            perror("Memory allocation failed");
            exit(1);
        }
    }

    while (fgets(line, 1000, file) != NULL) {
        char *token = strtok(line, " ");
        while (token != NULL) {
            objCode[address].address = address;
            objCode[address].objCode = atoi(token);
            printf("%d %d needs to read operands? %d opcodeBool? %d\n", objCode[address].address, objCode[address].objCode, readOperand, opcodeBool);
            if (isInListINT(address, listOfOperands) == 1 && (address >= firstVariableAddress || address <= lastVariableAddress) && address != jmpModule){
                printf("É um address ---> %d\n", address);
                readOperand = 1;
                readingAddresses = 1;
            }
            if (objCode[address].objCode < 15 && objCode[address].objCode > 0 && readOperand == 0 && stopOpcode == 0 && jmpLabel == 0 && readingAddresses == 0){
                opcodeBool = 1;
                opcode = objCode[address].objCode;
                strcpy(program1[lineCount].opcode, token);
                if ((opcode > 0 && opcode < 9) || (opcode > 9 && opcode < 14)){
                    printf("Opcode é %d\n", opcode);
                    opcodeOperands = 1;
                    readOperand = 1;
                    if (opcode > 4 && opcode < 9){
                        jmpLabel = 1;
                    }
                    if (opcode == 11 || opcode == 12){
                        spaceVar = 1;
                    }
                } else if (opcode == 9){
                    opcodeOperands = 2;
                    readOperand = 1;
                } else if (opcode == 14){
                    opcodeOperands = 0;
                    readOperand = 0;
                    stopOpcode = 1;
                    stopAddress = address;
                    //printf("STOP_ADDRESS = %d\n", stopAddress);
                    strcpy(program1[lineCount].operand1,  "");
                    strcpy(program1[lineCount].operand2, "");
                    //printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                    lineCount += 1;
                }
            }
            if (readOperand > 0 && opcodeBool == 0){
                opcode = 0;
                printf("Needs to read %d operands\n", opcodeOperands);
                varInt = atoi(token);
                if ((firstVariableAddress == 0 && readingAddresses == 0) || (jmpModule > -1 && lastVariableAddress == 0)){
                    firstVariableAddress = atoi(token);
                    lastVariableAddress = atoi(token);
                    printf("The first first variable address of section data %d\n", firstVariableAddress);
                }
                if (readingAddresses == 0 && isInListINT(varInt, listOfOperands) == 0 && (address >= firstVariableAddress || address <= lastVariableAddress)){
                    printf("Aqui %d!!!!\n", atoi(token));
                    listOfOperands[opIndex] = malloc(sizeof(int));
                    *listOfOperands[opIndex] = varInt;
                    for (int i = 0; i < opIndex; i++){
                        printf("%d %d %d\n", firstVariableAddress, lastVariableAddress, *listOfOperands[i]);
                        if (firstVariableAddress > varInt && varInt != jmpModule){
                            firstVariableAddress = varInt;
                            printf("The first variable address of section data %d\n", firstVariableAddress);
                        }
                        if (lastVariableAddress < varInt && varInt != jmpModule){
                            lastVariableAddress = varInt;
                            printf("The last variable address of section data %d\n", lastVariableAddress);
                        }
                    }
                    opIndex++;
                }

                if (jmpLabel == 1){
                    operandsAI[opIndex].addressMentioned = address;
                    operandsAI[opIndex].variableAddress = atoi(token);
                    //sprintf(varTemp, "%d", address);
                    strcpy(varName, "label");
                    strcat(varName, token);
                    strcpy(operandsAI[opIndex].name, varName);
                    if (isInList(operandsAI[opIndex].name, varSimpleList) == 0 || vlIndex == 0){
                        strcpy(varSimpleList[slIndex], operandsAI[opIndex].name);
                        slIndex++;
                        strcpy(varList[vlIndex].name, operandsAI[opIndex].name);
                        varList[vlIndex].type = 1;
                        varList[vlIndex].value = address;
                        varList[vlIndex].address = atoi(token);
                        vlIndex++;
                    }
                    jmpModule = atoi(token);
                    if (lastVariableAddress == jmpModule){
                        lastVariableAddress -=1;
                    }
                    jmpLabel = 0;
                    //printf("%d %s %d\n", operandsAI[opIndex].addressMentioned, operandsAI[opIndex].name, operandsAI[opIndex].variableAddress);
                } else {
                    if (readingAddresses == 0 && isInListINT(address, listOfOperands) == 0 && (address < firstVariableAddress || address > lastVariableAddress)){
                        operandsAI[opIndex].addressMentioned = address;
                        operandsAI[opIndex].variableAddress = atoi(token);
                        //sprintf(varTemp, "%d", address);
                        strcpy(varName, "variable");
                        strcat(varName, token);
                        printf("\n\n%s\n\n", varName);
                        strcpy(operandsAI[opIndex].name, varName);
                        if (isInList(operandsAI[opIndex].name, varSimpleList) == 0|| vlIndex == 0){
                            strcpy(varSimpleList[slIndex], operandsAI[opIndex].name);
                            slIndex++;
                            strcpy(varList[vlIndex].name, operandsAI[opIndex].name);
                            varList[vlIndex].address = atoi(token);
                            if (spaceVar == 1){
                                varList[vlIndex].type = 3;
                                spaceVar = 0;
                            }
                            vlIndex++;
                        }
                    }
                    //printf("%d %s %d\n", operandsAI[opIndex].addressMentioned, operandsAI[opIndex].name, operandsAI[opIndex].variableAddress);
                }
                if (opcodeOperands == 1){
                    if (strcmp(program1[lineCount].opcode, "9") == 0){
                        strcpy(program1[lineCount].operand2, token);

                    } else {
                        strcpy(program1[lineCount].operand1,  token);
                        strcpy(program1[lineCount].operand2, "");
                    }
                    //printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                    //lineCount += 1;
                    opcodeOperands -= 1;
                    readOperand = 0;
                }
                if (opcodeOperands == 2){
                    strcpy(program1[lineCount].operand1, token);
                    opcodeOperands = opcodeOperands - 1;
                }
            }
            if ((firstVariableAddress < address || lastVariableAddress > address) && opcodeBool == 0){
                /*
                dataSection[dsIndex].address = address;
                strcpy(varName, "variable");
                sprintf(varTemp, "%d", address);
                strcat(varName, varTemp);
                strcpy(dataSection[dsIndex].name, varName);
                dataSection[dsIndex].value =  atoi(token);
                //printf("%d %s %d\n", dataSection[dsIndex].address, dataSection[dsIndex].name, dataSection[dsIndex].value);
                dsIndex++;
                for (int i = 0; i < vlIndex; i++) {
                    if (strcmp(varName, varList[i].name) == 0) {
                        varList[i].value = atoi(token);
                    }
                }
                 */
                if (opcode == 0 && readOperand == 1) {
                    printf("entrei aqui!!! %s\n", token);
                    strcpy(program1[lineCount].opcode, "");
                    strcpy(program1[lineCount].operand1,  token);
                    strcpy(program1[lineCount].operand2, "");
                }
                //printf("%s|%s|%s\n", program1[lineCount].opcode, program1[lineCount].operand1, program1[lineCount].operand2);
                lineCount += 1;
            }
            //printf("        %d : %d\n", objCode[address].address, objCode[address].objCode);
            if (address > 0 && readingAddresses == 1 && (address > firstVariableAddress && address == lastVariableAddress)){
                printf("Last address!!!\n");
                firstVariableAddress = address;            // Rewind for more opcodes and addresses being read
                if (address < jmpModule){
                    for (int i = 0; i < opIndex; i++){
                        if (jmpModule > *listOfOperands[i]){
                            lastVariableAddress = jmpModule;
                        }
                    }
                }
                readOperand = 0;
                opcode = 0;
                opcodeBool = 0;
                opcodeOperands = 0;
                readingAddresses = 0;
            }
            address += 1;
            if (opcodeBool == 1){
                opcodeBool = 0;
            }
            token = strtok(NULL, " ");
        }
    }
    fclose(file);
    printf("name type value address\n");
    for (int a = 0; a < vlIndex; a++){
        if (varList[a].type != 1 && varList[a].type != 3){
            varList[a].type = 2;
        }
        printf("%s %d %d %d\n", varList[a].name, varList[a].type, varList[a].value, varList[a].address);
    }
    printf("\n");
    for (int b = 0; b < lineCount; b++){
        printf("%d %s|%s|%s\n", b, program1[b].opcode, program1[b].operand1, program1[b].operand2);
    }

    for (int i = 0; i < 100; i++) {
        free(varSimpleList[i]);
    };

    // Free allocated memory when done
    for (int i = 0; i < opIndex; i++) {
        free(listOfOperands[i]);
    }
}



int main(int argc, char** argv) {
    char* filename = argv[1];
    tradutor(filename);
    return 0;
}
