//
// Created by Tom on 17.12.2017.
//

#pragma once

#include "../Tree/Tree.h"
#include "../RecursiveDescent/Descent.h"

int compile(parser *pars, const char *fileName);

void compileRecFunc(Node *node, parser *pars, int curExitPoint);