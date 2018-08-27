//
// Created by Tom on 12.12.2017.
//

#ifndef LANGUAGE_DESCENT_H
#define LANGUAGE_DESCENT_H

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../Tree/Tree.h"

static const char functionFlag[] = "def";
static const int startFuncNumber = 150;

enum
{
    Number,
    CharConst,
    Add,
    Sub,
    Mul,
    Assign,
    If,
    Else,
    While,
    Call,
    Bigger,
    Smaller,
    Equal,
    NotEqual,
    SmallerOrEq,
    BiggerOrEq,
    Div,
    Param,
    Function,
    Read,
    Write,
    Sqrt,
    Return,
    Init,
    Linker = 66,
};


typedef struct func
{
    int varNumber;
    int argNumber;
    char *name;
    Tree subTree;
} func;


typedef struct parser
{
    char *code;
    Tree *tree;
    func *functions;

    int curLine;
    int labelNum;
    int curCodePos;
    int curFunction;
    int numberOfMainFunc;

    char **CurrentVariables;
    int argAmount;
    FILE *bin_code;

} parser;


Node *getAddSub(parser *pars);

Node *getMulDiv(parser *pars);

Node *getMathBranches(parser *pars);

Node *getFuncBranches(parser *pars);

Node *getNumber(parser *pars);

Node *getAssign(parser *pars);

Node *getCommand(parser *pars);

Node *getId(parser *pars);

Node *getCond(parser *pars);

Node *getBody(parser *pars);

Node *getArguments(parser *pars, int *argAmount);

Node *getElse(parser *pars);

int analyzeData(Node *node);

int deleteLinkers(Node *node);

int getTree(parser *pars);

int getNewFunc(parser *pars);

int destructParser(parser *pars);

int getBuf(parser *data, const char *inputFileName);

int constructParser(parser *pars, const char *inputFileName);


#endif //LANGUAGE_DESCENT_H
