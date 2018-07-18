//
// Created by Tom on 18.10.2017.
//

#ifndef ASSEMBLER_STACK_H
#define ASSEMBLER_STACK_H

#endif //ASSEMBLER_STACK_H

#include <iostream>

#pragma once

#define stack_err(err) stack_errlist[err]

#define MAGIC_CANARY_NUMBER 666

#define createStack(object, type, format, sizeOfStack)  \
    Stack<type> object (sizeOfStack);                   \
    object.setTypeName (#type);                         \
    object.setTypeFormat (format);

/*
 * Error codes
 */


enum {
	NO_ERRORS = 0,
	PUSH_ERROR,
	POP_ERROR,
	CONSTRUCT_ERROR
};
/*
 * Explanation of errors codes
 */

const char *const stack_errlist[] = {
		"Program finished with no errors",
		"Stack overflow or another push error",
		"Stack underflow or another pop error",
		"Construct problem"
};

template<typename T>
class Stack {

private:

	int canary1 = MAGIC_CANARY_NUMBER;

	int hash = 0;

	/**Pointer on the first element of the stack
	*/
	T *data = NULL;

	/**Pointer on the data which is using for count hash
	 */
	char *rawData = NULL;

	const char *typeName;
	const char *typeFormat;

	size_t maxSize = 0;
	size_t size = 0;
	size_t rawSize = 0;

	int canary2 = MAGIC_CANARY_NUMBER;


public:

	Stack (size_t sizeOfStack) {
		maxSize = sizeOfStack;

		rawSize = sizeOfStack * sizeof (T) + 2 * sizeof (int);
		rawData = (char *) calloc (rawSize, 1);
		data = (T *) (rawData + sizeof (int));

#define canary3 *((int *) rawData)
#define canary4 *((int *) (rawData + rawSize -  sizeof (int)))

		canary3 = MAGIC_CANARY_NUMBER;
		canary4 = MAGIC_CANARY_NUMBER;
		hash = calcHash ();
		OK (CONSTRUCT_ERROR, data, rawData);
	}

	/** Set name of the type
 	*/

	void setTypeName (const char *stackTypeName) {
		typeName = stackTypeName;
	}

	/**  Set the format for printf
 	*/

	void setTypeFormat (const char *stackTypeFormat) {

		typeFormat = stackTypeFormat;
	}

	/**  count the hash of stack
 	*  @returns the hash
 	*/

	int calcHash () {

#ifndef RELEASE
		int _hash = 0;

		for (size_t i = sizeof (int); i < rawSize - sizeof (int); i++) {
			if (i % 2 == 0)
				_hash += 2 * rawData[i];
			else
				_hash += rawData[i];
		}
		_hash += maxSize + rawSize + size;
		return _hash;
#endif

#ifdef RELEASE

		return 0;
#endif
	}


	/**  Stack cheacking
 	*/

	bool OK (int errnum, bool condition) {

		if ((hash != calcHash ()) ||
		    (canary1 != MAGIC_CANARY_NUMBER) ||
		    (canary2 != MAGIC_CANARY_NUMBER) ||
		    (canary3 != MAGIC_CANARY_NUMBER) ||
		    (canary4 != MAGIC_CANARY_NUMBER) ||
		    !condition) {
			fprintf (stderr, "\n%s\n", stack_err (errnum));
			dump ();
			return 1;
		}
		return 0;
	}

	bool OK (int errnum, bool condition1, bool condition2) {

		if ((hash != calcHash ()) ||
		    (canary1 != MAGIC_CANARY_NUMBER) ||
		    (canary2 != MAGIC_CANARY_NUMBER) ||
		    (canary3 != MAGIC_CANARY_NUMBER) ||
		    (canary4 != MAGIC_CANARY_NUMBER) ||
		    !condition1 ||
		    !condition2) {
			fprintf (stderr, "\n%s\n", stack_err (errnum));
			dump ();
			return 1;
		}
		return 0;
	}

#undef canary3
#undef canary4
#undef MAGIC_CANARY_NUMBER
#undef stack_err

	/**  print all information about stack
 	*/

	void dump () {

		fprintf (stdout, "Type of stack is %21s\n", typeName);
		fprintf (stdout, "Max size of stack is %17d\n", maxSize);
		fprintf (stdout, "Size of stack is %21d\n", size);
		fprintf (stdout, "Stack hash is:%24d\n", calcHash ());
		fprintf (stdout, "Stack elemetns:\n");

		if (typeFormat == "%s" && typeName != "char*") {
			fprintf (stderr, "Wrong input of format\n");
		} else {
			for (int i = 0; i < size; i++) {
				fprintf (stdout, "%6d ", i);
				fprintf (stdout, typeFormat, data[i]);
				fprintf (stdout, "\n");
			}
		}
	}


	/**  Add element to the stack
 	*/

	bool push (const T &value) {

		if (OK (PUSH_ERROR, ((size) + 1) <= maxSize))
			return 1;
		*(data + size) = value;
		size++;

		hash = calcHash ();
		if (OK (PUSH_ERROR, 1))
			return 1;
		return 0;
	}

	/** Take element from stack
	 *  and put int to @value
 	*/

	bool pop (T &value) {

		if (OK (POP_ERROR, size > 0))
			return 1;

		size--;
		hash = calcHash ();

		if (OK (POP_ERROR, 1))
			return 1;

		value = data[size];
	}

	bool pop () {

		if (OK (POP_ERROR, size > 0))
			return 1;

		size--;
		hash = calcHash ();

		if (OK (POP_ERROR, 1))
			return 1;
	}

	~Stack () {
		free (rawData);
		data = NULL;
		rawData = NULL;
	}
};
