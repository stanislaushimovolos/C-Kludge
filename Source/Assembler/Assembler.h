//
// Created by superstraz on 7/19/18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

/*!
 * @file Assembler.h
 * @brief Contains function's headers and some constants.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-7-19
 */

/*!
  @def NDEBUG
   Release version so asserts were turned off.
*/

#define NDEBUG

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*!
 * @def ASM_VERSION
 * Current version of the assembler (important for the stack machine).
 */
/*!
 * @def ASM_NAME
 * Name of current version of the assembler.
 */

/*!
* @def SIGNATURE
* Author's initials.
*/

#define ASM_VERSION "v3.0"
#define SIGNATURE "SA"
#define ASM_NAME "ASM"


/*!
* @def INIT_LABEL_NUMBER
* Initial size of label's array.
*/

#define INIT_LABEL_NUMBER 128


static const char defaultInput[] = "../../../LanguageRef/AsmDump/asmCode.txt";
static const char defaultOutput[] = "factorialCore.txt";
static const char labelSymbol[] = ":";

static const char *const errList[] = {"",
                                      "Incorrect arguments in function.",
                                      "Certain problems related to opening file.",
                                      "System couldn't allocate memory.",
                                      "Strtoul couldn't interpret a token as a number.",
                                      "A label (without value) was the last token.",
                                      "One label was used twice.",
                                      "Non exist label.",
                                      "Incorrect Push operand.",
                                      "Operand name is unknown."
};

/**
* @brief The list of possible errors
*/

enum error_codes
{
    ARGUMENTS_ERR = 1,      //!< Means that function argument or some of them are equal NULL.
    OPENFILE_ERR,           //!< Means that some problem was detected in process of opening file.
    MEMORY_ERR,             //!< Means that system couldn't allocate memory.
    LABEL_VAL_ERR,          //!< Means that a number wasn't found after the labels symbol ( : )
    LABEL_EOF,              //!< Means that a label was the last token (without number of this label).
    LABEL_REPEAT,           //!< Means that one label value was used twice.
    LABEL_NON_EXIST,        //!< Means that there was jump to non-existent label
    PUSH_ERR,               //!< Means that operand of PUSH command is incorrect.
    UNKNOWN_OPERAND,        //!< Means that token has unknown name
    OUT_OF_ERR_RANGE_ERR    //!< Means that num of the error is too large

};

/*!
      @brief The structure which keeps tokens, information about labels, etc.

      It contains the original assembly code, size of this code,
      an array of tokens and  labels.
 */


typedef struct
{
    char *buffer;               //!<buffer with input data
    char **tokens;              //!<array of tokens to which source data was splitted

    size_t *labels;       //!<array of assembly labels (each of them keeps information about its position in tokens)
    size_t tokensNumber;  //!<size of @ref tokens array
    size_t labelsNumber;  //!<size of @ref labels array

    double *binaryCode;         //!<keeps opcodes of stack machine (for more information look \ref Commands.h)
    size_t size;                //!<size of @ref buffer

} code_t;


/*!
    @brief Starts all other functions. Does and error checking.
    @param[in] data  data @ref code_t object.
    @param argv, argc command line's arguments.
    @return Returns an error code.
*/
int assemble(code_t *data, int argc, const char *argv[]);


/*!
    @brief Shows information about @ref code_t object in the standard output.
    @param[in] data  data @ref code_t object.
    @return Returns an error code.
*/
int dumpCode_t(code_t *data);


/*!
    @brief Writes tokens in the standard output.
    @param[in] data  data @ref code_t object.
    @return Returns an error code.
*/
int displayTokens(code_t *data);

/*!
 * @brief Evaluates assembly commands and the stack machine's opcodes.
 * @param[in] data @ref code_t object.
 * @return Returns an error code.
 */

int compile(code_t *data);

/*!
    @brief Reads data from an input file .
    @param[in] data  @ref code_t object.
    @param[in] inputFilename a name of the input file.
    @return Returns an error code.
*/
int getBuf(code_t *data, const char *inputFile);


/*!
    @brief Splits the data into tokens and creates an array of labels.
    @param[in] data  @ref code_t object.
    @param[in] label symbol of the labels.
    @return Returns an error code.
*/
int processAsmCode(code_t *data, const char *label);


/*!
    @brief Writes binary code in the text file for the stack machine (look @ref Commands.h and @ref Cpu.h).
    @param[in] data  data @ref code_t object.
    @param[in] outputFile the name of the output text file.
    @return Returns an error code.
*/
int writeCode(code_t *data, const char *outFilename);


/*!
    @brief Returns an error code and prints information about the error(standard output + standard error output).
    @param[in] err_num  error code.
    @param[in] usr_msg information about the error which is available for user.
    @param[in] _func name of the function where an error was detected.
    @param[in] _line number of a code line where an error was detected.
    @param[in] _file a name of  file where an error was detected.
    @return returns an error code.
*/
int throw_error(unsigned int err_num, const char *usr_msg, const char *err_msg, const char *_func, int _line,
                const char *_file);


/*!
    @brief Destroys a @ref code_t structure.
    @param[in] data code_t object.
*/
void destructCode_t(code_t *code);


/*!
    @brief Counts the amount of tokens in the  input data.
    @param[in] buffer input data.
    @return number of tokens.
*/
size_t countTokens(const char *buffer);

#endif //ASSEMBLER_ASSEMBLER_H
