//
// Created by stanissslaf on 5/2/18.
//

#ifndef LANGUAGE_ELFCOMPILE_H
#define LANGUAGE_ELFCOMPILE_H

#include "../RecDescent/Descent.h"
#include "Opcodes.h"

const int elem_sz = 8;
const int startElf_sz = 2000;
const int startCall_amount = 50;
const int startFunc_amount = 50;
const int startJump_amount = 50;
const int startLabel_amount = 50;

const size_t header_sz = 0x80;

const char PrintObj[] = "Stdio/Print.o";
const char ScanObj[] = "Stdio/Scan.o";

#define byte 1
#define word 2
#define d_word 4
#define q_word 8

struct Call
{
    size_t own_address;
    size_t func_num;
};

struct Jump
{
    size_t own_address;
    size_t label_num;

};

struct Compiler
{
    Tree tree;
    char *bin_code;
    char *curb;
    size_t cur_sz;
    int labelNum;

    size_t PrintFuncAddress;
    size_t ScanFuncAddress;

    size_t cur_call;
    size_t cur_func;
    size_t *func_arr;
    Call *call_arr;

    size_t cur_jmp;
    size_t *label_arr;
    Jump *jmp_arr;

    const char **cur_param_Arr;
    long cur_arg_Amount;

    size_t bin_code_capacity;
    size_t call_arr_capacity;
    size_t call_func_arr_capacity;
    size_t jmp_arr_capacity;
    size_t jmp_labels_capacity;
};

bool destructComp(Compiler *comp);

bool constructComp(Compiler *comp);

bool compile(Compiler *comp, FILE *output);

bool getSysFunc(Compiler *comp);

char *getFunc(const char *filename, size_t *sizeOfBuf);

bool
compileRecFunc(const Node *node, Compiler *comp);

bool setLabels(Compiler *comp);

bool compCheck(Compiler *comp);

int makeHeader(Compiler *comp);


#endif //LANGUAGE_ELFCOMPILE_H
