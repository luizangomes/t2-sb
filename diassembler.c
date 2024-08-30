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

// Struct para retorno da objCodeTreatment
struct ReturnValues {
    struct programAssemblyInventado program1[100];
    struct dadosAssemblyInventado dadosAI[100];
    int lineCount;
    int dadosIndex;
};

struct InventadoParaIA32 {
    int opcodeAI;
    char codigoIA32[100];
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

struct InventadoParaIA32 dicionarioIA32[14] = {
    {1, "\tadd eax, [%s]\n"},    // ADD
    {2, "\tsub eax, [%s]\n"},    // SUB
    {3, "\timul eax, [%s]\n"},    // MUL
    {4, "\tcdq\n\tidiv dword [%s]\n"},    // DIV
    {5, "\tjump %s\n"},    // JMP
    {6, "\tcmp eax,0\n\tjl %s\n"},    // JMPN
    {7, "\tcmp eax,0\n\tjg %s\n"},    // JMPP
    {8, "\tcmp eax,0\n\tje %s\n"},    // JMPZ
    {9, "\tmov eax, [%s]\n\tmov [%s], eax\n"},    // COPY: a variável 1 é a OP1 e a variável 2 é a OP2
    {10, "\tmov eax, %s\n"},    // LOAD: a variável é o que você quer carregar
    {11, "\tmov [%s], eax\n"},    // STORE: a variável que você quer guardar, lembrando que tem que ter algo no acumulador
    //{12, "\tpush dword %d\ncall INPUT\n"},
    {12, "\tpush 16\n\tpush %s\n\tcall input\n\tpush dword %s\n\tpush dword %s\n\tcall string_to_int\n"},    // INPUT: o primeiro %s é um buffer, 2o é a variável que você quer guardar, e a 3a é o buffer novamente
    {13, "\tpush %s\n\tpush dword [%s]\n\tcall int_to_string\n\tpush dword 16\n\tpush dword %s\n\tcall output\n"},    // OUTPUT: o primeiro %s é o buffer, o segundo é o que você quer imprimir, e o terceiro é o buffer de novo
    {14, "\tmov eax, 1\n\txor ebx, ebx\n\tint 0x80\n"}    // STOP
};

char funcoes[1000] = "";
// Esse INPUT é somente para integers, supostamente ele trata inteiros negativos e positivos
//INPUT:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\n\txor eax, eax\n\txor edx, edx\n\txor ebx, ebx\n\tmov bl, 1\n\n\tmov eax, 3\n\tmov ebx, 0\n\tlea ecx, [esp-1]\n\tmov edx, 1\n\tint 0x80\n\n\tcmp byte [esp-1], '-'\n\tjne check_positive\n\tmov bl, -1\n\tjmp read_number\n\ncheck_positive:\n\tcmp byte [esp-1], '+'\n\tjne read_number\n\nread_number:\n\txor eax, eax\n\nconvert_loop:\n\tmov eax, 3\n\tmov ebx, 0\n\tlea ecx, [esp-1]\n\tmov edx, 1\n\tint 0x80\n\n\tcmp byte [esp-1], 10\n\tje done\n\n\tsub byte [esp-1], '0'\n\timul eax, eax, 10\n\tmovzx ebx, byte [esp-1]\n\tadd eax, ebx\n\tjmp convert_loop\n\ndone:\n\timul eax, ebx\n\tmov [ecx], eax\n\tpop ebp\n\tret
// Esse usa as duas funções, a outra era nested
//INPUT:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tmov edx, [ebp+12]\n\tmov eax, 3\n\tmov ebx, 0\n\tint 80h\n\tpop ebp\n\tret\n\nstring_to_int:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tmov ebx, [ebp+12]\n\txor eax, eax\n\txor edx, edx\n\tmov esi, 1\n\n\tmovzx edx, byte [ecx]\n\tcmp edx, '-'\n\tjne check_positive\n\tmov esi, -1\n\tinc ecx\n\tjmp convert_loop\n\ncheck_positive:\n\tcmp edx, '+'\n\tjne convert_loop\n\tinc ecx\n\nconvert_loop:\n\tmovzx edx, byte [ecx]\n\ttest edx, edx\n\tjz done\n\tcmp edx, 10\n\tje done\n\tsub edx, '0'\n\timul eax, eax, 10\n\tadd eax, edx\n\tinc ecx\n\tjmp convert_loop\n\ndone:\n\timul eax, esi\n\tmov [ebx], eax\n\tpop ebp\n\tret


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

struct ReturnValues objCodeTreatment(char* filename) {
    FILE *file;
    struct AddressObjCode objCode[100];
    //struct programAssemblyInventado program1[100];
    //struct dadosAssemblyInventado dadosAI[100];
    struct ReturnValues retValues;
    char line[1000], *token;
    int *listOfOperands[100] = {NULL};
    int opIndex = 0;
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
            objCode[address].address = address;
            objCode[address].objCode = atoi(token);
            if (address == 0){
                opcodeBool = 1;                        // Se for o endereço 00 sempre vamos assumir que é um opcode!!!
            }
            if (isInListINT(address, listOfOperands) == 1){
                strcpy(retValues.program1[retValues.lineCount].operand1, token);
                for (int i = 0; i < retValues.dadosIndex; i++){
                    if (address == retValues.dadosAI[retValues.dadosIndex].addressVariableDeclared){
                        retValues.dadosAI[retValues.dadosIndex].addressReferred = atoi(token);
                    }
                }
                retValues.lineCount++;
                breakAddressing = 1;
            }
            if (objCode[address].objCode > 0 && objCode[address].objCode < 15 && opcodeBool == 1 && breakAddressing == 0){
                strcpy(retValues.program1[retValues.lineCount].opcode, token);
                // Caso de ADD, SUB, MUL, DIV, STORE, INPUT, OUT
                if ((objCode[address].objCode > 0 && objCode[address].objCode < 5) || (objCode[address].objCode > 9 && objCode[address].objCode < 14) ){
                    quantOperandos = 1;
                }
                // Caso de JMP, JMPN, JMPP, JPMZ
                if (objCode[address].objCode > 4 && objCode[address].objCode < 9){
                    quantOperandos = 1;
                    jmpModule = 1;
                }
                // Caso de COPY
                if (objCode[address].objCode == 9){
                    quantOperandos = 2;
                }
                // Caso de STOP
                if (objCode[address].objCode == 14){
                    quantOperandos = 0;
                    retValues.lineCount += 1;
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
                        for (int i = 0; i < retValues.dadosIndex; i++){
                            if (atoi(token) == retValues.dadosAI[i].addressVariableDeclared){
                                if (strcmp(retValues.dadosAI[i].type, "CONST") == 0){
                                    strcpy(retValues.dadosAI[i].type, "SPACE");
                                }
                                found = 1;
                            }
                        }
                        if (found == 0){
                            retValues.dadosAI[retValues.dadosIndex].addressVariableDeclared = atoi(token);
                            if (atoi(retValues.program1[retValues.lineCount].opcode) == 11 || atoi(retValues.program1[retValues.lineCount].opcode) == 12){
                                strcpy(retValues.dadosAI[retValues.dadosIndex].type, "SPACE");
                            } else {
                                strcpy(retValues.dadosAI[retValues.dadosIndex].type, "CONST");
                            }
                            retValues.dadosIndex++;
                        }

                    } else {
                        for (int i = 0; i < retValues.dadosIndex; i++){
                            if (atoi(token) == retValues.dadosAI[i].addressReferred){
                                found = 1;
                            }
                        }
                        if (found == 0){
                            retValues.dadosAI[retValues.dadosIndex].addressVariableDeclared = address;
                            strcpy(retValues.dadosAI[retValues.dadosIndex].type, "LABEL");
                            retValues.dadosAI[retValues.dadosIndex].addressReferred = atoi(token);
                            retValues.dadosIndex++;
                        }
                    }
                    found = 0;
                }
                if (quantOperandos == 2){
                    strcpy(retValues.program1[retValues.lineCount].operand2, token);
                    quantOperandos -= 1;
                    opcodeBool = 0;
                } else if (quantOperandos == 1){
                    strcpy(retValues.program1[retValues.lineCount].operand1, token);
                    quantOperandos -= 1;
                    retValues.lineCount += 1;
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
    for (int c = 0; c < retValues.dadosIndex; c++){
        printf("%d %d %s\n",retValues.dadosAI[c].addressVariableDeclared, retValues.dadosAI[c].addressReferred, retValues.dadosAI[c].type);
    }
    printf("\n");
    for (int b = 0; b < retValues.lineCount; b++){
        printf("%d %s|%s|%s\n", b, retValues.program1[b].opcode, retValues.program1[b].operand1, retValues.program1[b].operand2);
    }

    // Free allocated memory when done
    for (int i = 0; i < opIndex; i++) {
        free(listOfOperands[i]);
    }

    retValues.lineCount = retValues.lineCount;
    return retValues;
}

void inventadoToIA32(struct programAssemblyInventado program1[100], struct dadosAssemblyInventado dadosAI[100], int lineCount, int dadosIndex){
    char line[1000] = "", operand1[100] = "", operand2[100] = "", inputBuffer[100] = "inputBuffer", varTemp[100] = "", varTemp2[100] = "";
    char sectionData[10000] = "section .data\n", sectionBss[10000] = "section .bss\n\tinputBuffer resb 10\n", sectionText[10000] = "section .text\n\tglobal _start\n_start:\n";
    int opcode = -1, address = 0;
    for (int i = 0; i < lineCount; i++){
        if (strcmp(program1[i].opcode, "") != 0){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == address){
                    printf("dados!!!\n");             // Checar se é uma variável para ser guardada na section data ou na bss
                }
            }
            address += 1;
        }
        if (strcmp(program1[i].operand1, "") != 0){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == address && strcmp(dadosAI[j].type, "LABEL") != 0){
                    printf("dados!!!\n");
                    strcpy(varTemp, "\tvariable");
                    sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                    strcat(varTemp, varTemp2);
                    strcat(varTemp, " dd 0\n");
                    strcat(sectionData, varTemp);
                }
            }
            address += 1;
        }
        if (strcmp(program1[i].operand2, "") != 0 ){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == address && strcmp(dadosAI[j].type, "LABEL") != 0){
                    printf("dados!!!\n");
                    strcpy(varTemp, "\tvariable");
                    sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                    strcat(varTemp, varTemp2);
                    strcat(varTemp, " dd 0\n");
                    strcat(sectionData, varTemp);
                }
            }
            address += 1;
        }
        opcode = atoi(program1[i].opcode);
        if (opcode == dicionarioIA32[atoi(program1[i].opcode)-1].opcodeAI){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == atoi(program1[i].operand1)){
                    if (strcmp(dadosAI[j].type, "LABEL") == 0){
                        strcpy(varTemp, "label");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand1, varTemp);
                    } else if (strcmp(dadosAI[j].type, "SPACE") == 0 || strcmp(dadosAI[j].type, "CONST") == 0) {
                        strcpy(varTemp, "variable");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand1, varTemp);
                    }
                }
                if (dadosAI[j].addressVariableDeclared == atoi(program1[i].operand2)){
                    if (strcmp(dadosAI[j].type, "LABEL") == 0){
                        strcpy(varTemp, "label");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand2, varTemp);
                    } else if (strcmp(dadosAI[j].type, "SPACE") == 0 || strcmp(dadosAI[j].type, "CONST") == 0) {
                        strcpy(varTemp, "variable");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand2, varTemp);
                    }
                }
            }
            switch (opcode){
                case 9:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, operand1, operand2);
                    break;
                case 12:
                case 13:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, inputBuffer, operand1, inputBuffer);
                    break;
                case 14:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, "");
                    break;
                default:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, operand1);
                    break;
            }
            strcat(sectionText, line);
            //sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, );
            //printf("%s", dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32);
            printf("%s", line);
        }
    }
    printf("LINE COUNT NA INVENTADO PARA IA32 %d\n", lineCount);
    printf("%s%s%s", sectionData, sectionBss, sectionText);
}



int main(int argc, char** argv) {
    char* filename = argv[1];
    struct ReturnValues result;
    // Chamar a função
    result = objCodeTreatment(filename);
    inventadoToIA32(result.program1, result.dadosAI, result.lineCount, result.dadosIndex);
    return 0;
}
