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
    char codigoIA32[200];
};

struct OpcodesAssembly tabelaDeInstrucoes[17] = {
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
    {"STOP", 14, 1, "Suspende a execução", 0},
    {"S_INPUT", 15, 3, "mem(OP) <- Entrada em string", 2},
    {"S_OUTPUT", 16, 3, "saída em string <- mem(OP)", 2}
};

struct InventadoParaIA32 dicionarioIA32[16] = {
    {1, "add eax, [%s]\n"},    // ADD
    {2, "sub eax, [%s]\n"},    // SUB
    {3, "imul eax, [%s]\n\tjo OVERFLOW\n"},    // MUL
    {4, "cdq\n\tidiv dword [%s]\n\tjo OVERFLOW\n"},    // DIV
    {5, "jmp %s\n"},    // JMP
    {6, "cmp eax,0\n\tjl %s\n"},    // JMPN
    {7, "cmp eax,0\n\tjg %s\n"},    // JMPP
    {8, "cmp eax,0\n\tje %s\n"},    // JMPZ
    {9, "mov eax, [%s]\n\tmov [%s], eax\n"},    // COPY: a variável 1 é a OP1 e a variável 2 é a OP2
    {10, "mov eax, [%s]\n"},    // LOAD: a variável é o que você quer carregar
    {11, "mov [%s], eax\n"},    // STORE: a variável que você quer guardar, lembrando que tem que ter algo no acumulador
    {12, "push dword %s\n\tpush dword %s\n\tcall INPUT\n\tpush 16\n\tpush %s\n\tcall clear_buffer\n"},    // INPUT: o primeiro %s é a variável que você quer guardar, 2o é a variável do inputBuffer, e a 3a é o buffer novamente
    {13, "push %s\n\tpush dword [%s]\n\tcall OUTPUT\n\tpush 16\n\tpush %s\n\tcall clear_buffer\n"},    // OUTPUT: o primeiro %s é o buffer, o segundo é o que você quer imprimir, e o terceiro é o buffer de novo
    {14, "mov eax, 1\n\txor ebx, ebx\n\tint 0x80\n"},    // STOP
    {15, "push dword %d\n\tpush dword [%s]\n\tcall S_INPUT\n"},    //S_INPUT: primeiro é o número de bytes, o segundo a variável a ser lida
    {16, "push dword %d\n\tpush dword [%s]\n\tcall S_OUTPUT\n"}    //S_OUTPUT: primeiro é o número de bytes, o segundo é a variável a ser impressa
};

char funcoes[3700] ="S_INPUT:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tmov edx, [ebp+12]\n\tmov eax, 3\n\tmov ebx, 0\n\tint 80h\n\tpush eax\n\tcall bytes_read_written\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, newline\n\tmov edx, 1\n\tint 0x80\n\tpop eax\n\tpop ebp\n\tret\n\nSTRING_TO_INT:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tmov ebx, [ebp+12]\n\txor eax, eax\n\txor edx, edx\n\tmov esi, 1\n\tmovzx edx, byte [ecx]\n\tcmp edx, '-'\n\tjne check_positive\n\tmov esi, -1\n\tinc ecx\n\tjmp convert_loop\ncheck_positive:\n\tcmp edx, '+'\n\tjne convert_loop\n\tinc ecx\nconvert_loop:\n\tmovzx edx, byte [ecx]\n\ttest edx, edx\n\tjz done\n\tcmp edx, '0'\n\tjb done\n\tcmp edx, '9'\n\tja done\n\tsub edx, '0'\n\timul eax, eax, 10\n\tadd eax, edx\n\tinc ecx\n\tjmp convert_loop\ndone:\n\timul eax, esi\n\tmov [ebx], eax\n\tpop ebp\n\tret\n\nINPUT:\n\tpush ebp\n\tmov ebp, esp\n\tpush dword 16\n\tpush dword [ebp+8]\n\tcall S_INPUT\n\tadd esp, 8\n\tpush dword [ebp+12]\n\tpush dword [ebp+8]\n\tcall STRING_TO_INT\n\tadd esp, 8\n\tpop ebp\n\tret\n\nOUTPUT:\n\tpush ebp\n\tmov ebp, esp\n\tpush dword [ebp+12]\n\tpush dword [ebp+8]\n\tcall INT_TO_STRING\n\tadd esp, 8\n\tpush dword 16\n\tpush dword [ebp+12]\n\tcall S_OUTPUT\n\tadd esp, 8\n\tpop ebp\n\tret\n\nINT_TO_STRING:\n\tpush ebp\n\tmov ebp, esp\n\tmov eax, [ebp+8]\n\tmov ebx, [ebp+12]\n\tmov edi, ebx\n\tadd edi, 14\n\tmov byte [edi], 0\n\tdec edi\n\tcmp eax, 0\n\tje print_zero\n\tjs handle_negative\nconvert_loop_its:\n\txor edx, edx\n\tdiv dword [ten]\n\tadd dl, '0'\n\tmov [edi], dl\n\tdec edi\n\ttest eax, eax\n\tjnz convert_loop_its\n\tinc edi\n\tjmp done_its\nhandle_negative:\n\tneg eax\n\tjmp convert_loop_its\ndone_its:\n\tcmp byte [edi-1], '-'\n\tjne end_negative\n\tmov byte [edi], '-'\n\tdec edi\nend_negative:\n\tinc edi\n\tmov eax, edi\n\tpop ebp\n\tret\nprint_zero:\n\tmov byte [ebx], '0'\n\tmov byte [ebx+1], 0\n\tmov eax, ebx\n\tpop ebp\n\tret\n\nS_OUTPUT:\n\tpush ebp\n\tmov ebp, esp\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, newline\n\tmov edx, 1\n\tint 0x80\n\tmov ecx, [ebp+8]\n\tmov edx, [ebp+12]\n\tmov eax, 4\n\tmov ebx, 1\n\tint 0x80\n\tpush eax\n\tcall bytes_read_written\n\tpop eax\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, newline\n\tmov edx, 1\n\tint 0x80\n\tpop ebp\n\tret\n\nOVERFLOW:\n\tmov eax, 4\n\tmov ebx, 1\n\tlea ecx, [overflow_msg]\n\tmov edx, 18\n\tint 0x80\n\nclear_buffer:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tmov edx, [ebp+12]\n\txor eax, eax\n\tclear_loop:\n\tmov byte [ecx], al\n\tinc ecx\n\tdec edx\n\tjnz clear_loop\n\tpop ebp\n\tret\n\nbytes_read_written:\n\tpush ebp\n\tmov ebp, esp\n\tmov ecx, [ebp+8]\n\tpush countBytesBuffer1\n\tpush dword ecx\n\tcall INT_TO_STRING\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, msg1\n\tmov edx, 22\n\tint 0x80\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, countBytesBuffer1\n\tmov edx, 14\n\tint 0x80\n\tmov eax, 4\n\tmov ebx, 1\n\tmov ecx, msg2\n\tmov edx, 7\n\tint 0x80\n\tpush 16\n\tpush countBytesBuffer1\n\tcall clear_buffer\n\tmov esp, ebp\n\tpop ebp\n\tret\n";

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
                    if (address == retValues.dadosAI[i].addressVariableDeclared){
                        retValues.dadosAI[i].addressReferred = atoi(token);
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
                                if (atoi(retValues.program1[retValues.lineCount].opcode) == 11 || atoi(retValues.program1[retValues.lineCount].opcode) == 12){
                                    if (strcmp(retValues.dadosAI[i].type, "CONST") == 0){
                                        strcpy(retValues.dadosAI[i].type, "SPACE");
                                    }
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

void inventadoToIA32(struct programAssemblyInventado program1[100], struct dadosAssemblyInventado dadosAI[100], int lineCount, int dadosIndex, char* filename){
    char line[1000] = "", lineTemp[1000] = "", operand1[100] = "", operand2[100] = "", inputBuffer[100] = "inputBuffer", varTemp[100] = "", varTemp2[100] = "";
    char sectionData[10000] = "section .data\n\tinputBuffer db 16 dup(0)\n\toverflow_msg db \"Overflow detectado!\", 0xA, 0\n\tcountBytesBuffer1 db 16 dup(0)\n\tmsg1 db \"<Foram lidos/escritos \", 0\n\tmsg2 db \" bytes>\",0\n\tnewline db 0xA\n\tten dd 10\n", sectionBss[10000] = "section .bss\n", sectionText[10000] = "section .text\n\tglobal _start\n";
    int opcode = -1, address = 0, labelAtLine = 0;
    for (int i = 0; i < lineCount; i++){
        strcpy(lineTemp, ""); strcpy(line, "");
        if (address == 0){
            strcat(sectionText, funcoes);
            strcat(sectionText, "\n_start:\n");
        }
        if (strcmp(program1[i].opcode, "") != 0){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressReferred == address && strcmp(dadosAI[j].type, "LABEL") == 0){
                    strcpy(varTemp, "LABEL");
                    sprintf(varTemp2, "%d", dadosAI[j].addressReferred);
                    strcat(varTemp, varTemp2);
                    strcat(varTemp, ": ");
                    strcat(lineTemp, varTemp);
                    labelAtLine = 1;
                }
            }
            address += 1;
        }
        if (labelAtLine == 0){
            strcat(lineTemp, "\t");
            labelAtLine = 0;
        }
        if (strcmp(program1[i].operand1, "") != 0){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == address && strcmp(dadosAI[j].type, "LABEL") != 0){
                    if (strcmp(dadosAI[j].type, "CONST") == 0){
                        strcpy(varTemp, "\tVARIABLE");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcat(varTemp, " dd ");
                        sprintf(varTemp2, "%d", dadosAI[j].addressReferred);
                        strcat(varTemp, varTemp2);
                        strcat(varTemp, "\n");
                        strcat(sectionData, varTemp);
                    }
                    if (strcmp(dadosAI[j].type, "SPACE") == 0){
                        strcpy(varTemp, "\tVARIABLE");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcat(varTemp, " resd 1\n");
                        strcat(sectionBss, varTemp);
                    }
                }
            }
            address += 1;
        }
        if (strcmp(program1[i].operand2, "") != 0 ){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == address && strcmp(dadosAI[j].type, "LABEL") != 0){
                    strcpy(varTemp, "\tVARIABLE");
                    sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                    strcat(varTemp, varTemp2);
                    strcat(varTemp, " dd 0\n");
                    strcat(line, varTemp);
                }
            }
            address += 1;
        }
        opcode = atoi(program1[i].opcode);
        if (opcode == dicionarioIA32[atoi(program1[i].opcode)-1].opcodeAI){
            for (int j = 0 ; j < dadosIndex; j++){
                if (dadosAI[j].addressVariableDeclared == atoi(program1[i].operand1)){
                    if (strcmp(dadosAI[j].type, "LABEL") == 0){
                        strcpy(varTemp, "LABEL");
                        sprintf(varTemp2, "%d", dadosAI[j].addressReferred);
                        strcat(varTemp, varTemp2);
                        strcpy(operand1, varTemp);
                    } else if (strcmp(dadosAI[j].type, "SPACE") == 0 || strcmp(dadosAI[j].type, "CONST") == 0) {
                        strcpy(varTemp, "VARIABLE");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand1, varTemp);
                    }
                }
                if (dadosAI[j].addressReferred == atoi(program1[i].operand1)){
                    if (strcmp(dadosAI[j].type, "LABEL") == 0){
                        strcpy(varTemp, "LABEL");
                        sprintf(varTemp2, "%d", dadosAI[j].addressReferred);
                        strcat(varTemp, varTemp2);
                        strcpy(operand1, varTemp);
                    }
                }
                if (dadosAI[j].addressVariableDeclared == atoi(program1[i].operand2)){
                    if (strcmp(dadosAI[j].type, "SPACE") == 0 || strcmp(dadosAI[j].type, "CONST") == 0) {
                        strcpy(varTemp, "VARIABLE");
                        sprintf(varTemp2, "%d", dadosAI[j].addressVariableDeclared);
                        strcat(varTemp, varTemp2);
                        strcpy(operand2, varTemp);
                    }
                }
            }
            switch (opcode){
                case 9:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, operand2, operand1);
                    break;
                case 12:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, operand1, inputBuffer, inputBuffer);
                    break;
                case 13:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, inputBuffer, operand1, inputBuffer);
                    break;
                case 14:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, "");
                    break;
                case 15:
                case 16:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, atoi(operand2), operand1);
                    break;
                default:
                    sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, operand1);
                    break;
            }
            strcat(lineTemp, line);
            strcat(sectionText, lineTemp);
            //sprintf(line, dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32, );
            //printf("%s", dicionarioIA32[atoi(program1[i].opcode)-1].codigoIA32);
            //printf("%s", lineTemp);
            if (labelAtLine == 1){
                labelAtLine = 0;
            }
        }
    }
    printf("LINE COUNT NA INVENTADO PARA IA32 %d\n", lineCount);
    printf("%s%s%s", sectionData, sectionBss, sectionText);
    // Escrita em arquivo o código
    strcat(filename, ".s");
    FILE *fileIA32 = fopen(filename, "w");

    if (fileIA32 == NULL) {
        perror("Error opening file! The code could not be writtened at the file!");
    }

    fprintf(fileIA32, "%s%s%s", sectionData, sectionBss, sectionText);

    fclose(fileIA32);
}



int main(int argc, char** argv) {
    char* filename = argv[1];
    char* filename2 = filename;
    struct ReturnValues result;
    // Chamar a função
    result = objCodeTreatment(filename);
    char *filenameNoEXT = strtok(filename2, ".");
    inventadoToIA32(result.program1, result.dadosAI, result.lineCount, result.dadosIndex, filenameNoEXT);
    return 0;
}
