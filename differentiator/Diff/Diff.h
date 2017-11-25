//
// Created by Tom on 22.11.2017.
//

#pragma  once

#include "../Tree_t/Tree.h"
enum {
	number,
	operator_,
	variable

};

struct Elem {
	char type;
	char *content;
};

const char inputFilename[] = "../base/base.txt";
const char outFilename[] = "../dumps/base.txt";
const char outFilenameForDotDump[] = "../picture/dotDump.txt";
const char outFilenameForDotPrint[] = "../picture/dotPrint.txt";

char *getBufferFromFileGetSzOfBuf (const char *_inputFileName, size_t *sizeOfBuffer = NULL);

char contentAnalyze (Node * node);

