//
// Created by stanissslaf on 4/28/18.
//

#ifndef LANGUAGE_NASM_H
#define LANGUAGE_NASM_H

#include "../RecDescent/Descent.h"

const int elem_sz = 8;

struct param
{
    char *paramName;
    int paramNum;
};


int compileN(Tree *tree, FILE *output);

bool
compileRecFuncN(const Node *node, const param *CurrentVariables, long ArgAmount, FILE *output);


#endif //LANGUAGE_NASM_H
