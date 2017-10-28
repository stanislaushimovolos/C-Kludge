//
// Created by Tom on 28.10.2017.
//
#pragma once

#include <assert.h>


char *getBufferFromFileGetSzOfBuf (const char *_inputFileName, size_t *sizeOfBuffer = NULL) {

	assert (_inputFileName);

	size_t sizeOfBuffer1 = 0;
	FILE *file = fopen (_inputFileName, "r");

	assert(file);


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