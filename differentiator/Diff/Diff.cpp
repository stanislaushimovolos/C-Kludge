//
// Created by Tom on 22.11.2017.
//
#include <assert.h>
#include <iostream>
#include <cstring>
#include "../Tree_t/Tree.h"

#include "Diff.h"


char *getBufferFromFileGetSzOfBuf (const char *_inputFileName, size_t *sizeOfBuffer) {

	assert (_inputFileName);

	size_t sizeOfBuffer1 = 0;
	FILE *file = fopen (_inputFileName, "r");

	assert (file);


	fseek (file, 0, SEEK_END);
	sizeOfBuffer1 = (size_t) ftell (file);
	fseek (file, 0, SEEK_SET);

	char *helpBuffer = (char *) calloc (sizeOfBuffer1 + 1, sizeof (char));
	fread (helpBuffer, 1, sizeOfBuffer1, file);
	fclose (file);

	if (sizeOfBuffer != NULL)
		*sizeOfBuffer = sizeOfBuffer1;

	return helpBuffer;
}

bool elemConstruct (Elem *elem) {
	elem->content = NULL;
	elem->type = 0;
}

bool elemDestruct (Elem *elem) {
	free (elem->content);
	elem->type = 0;
}

#define DEF_CMD_OPERATOR(operator_, number)             \
    case operator_ :                                    \
     {                                                  \
        node->type = number;                            \
        break;                                          \
    }


char contentAnalyze (Node *node) {
	char *NumHelper = (node->content);

	strtod (node->content, &NumHelper);

	if (NumHelper != node->content)
		node->type = number;

	else if (strlen (node->content)) {

		switch (node->content[0]) {

#include "MathFunc.h"

			default:
				break;
		}
	}

}