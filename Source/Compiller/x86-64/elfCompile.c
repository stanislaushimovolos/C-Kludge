//
// Created by stanissslaf on 5/2/18.
//

#include "elfCompile.h"
#include "../Nasm/HexTranslation.h"

#define current_call ((comp->call_arr)[comp->cur_call])
#define current_call_inc  ((comp->call_arr)[comp->cur_call++])

bool compile(Compiler *comp, FILE *output)
{

    if (!output) {
        printf("Error, unacceptable output file in function: %s\n", __PRETTY_FUNCTION__);
        return false;
    }

    if (!comp) {
        printf("Error, nullptr as a parameter in function: %s\n", __PRETTY_FUNCTION__);
        return false;
    }

    // set the pointer after the header

    comp->curb = comp->bin_code + header_sz;
    comp->cur_sz = header_sz;

    // Make the entry point :

    current_call.own_address = comp->cur_sz;                    //
    push_bytes(CALL, byte);                                     //      call main
    current_call_inc.func_num = (size_t) comp->tree.MainFlag;   //
    push_bytes(0, d_word);                                      //

    push_bytes(0x000000003cb8, 5);                              //      mov rax, 60
    push_bytes(Xor_rdi_rdi, 3);                                 //      xor rdi, rdi
    push_bytes(Syscall, word);                                  //      syscall


    // get system library of input/output

    getSysFunc(comp);

    auto CurNode = comp->tree.root;

    while (CurNode) {
        if (CurNode->Left) {
            auto argAmount = strtol(CurNode->Left->Left->Left->content, nullptr, 10);
            Node *paramNode = CurNode->Left->Left->Right;

            auto paramArr = (const char **) calloc((size_t) argAmount + 1, sizeof(char *));

            for (int i = 0; i < argAmount; i++) {
                paramArr[i] = paramNode->Left->content;
                paramNode = paramNode->Right;
            }

            comp->cur_param_Arr = paramArr;
            comp->cur_arg_Amount = argAmount;

            compCheck(comp);
            comp->func_arr[comp->cur_func] = comp->cur_sz;
            comp->cur_func++;


            // make a prologue

            push_bytes(Push_rbp, byte);          //  push rbp                   ; save rbp
            push_bytes(Mov_rbp_rsp, 3);          //  mov rbp, rsp
            SUB_RSP(argAmount *
                            elem_sz);            //  sub rsp arg_amount*arg_sz  ; memory allocation for local variables


            // compile a body of function

            compileRecFunc(CurNode->Left->Right, comp);

            // make "an epilogue"

            ADD_RSP(elem_sz * argAmount);         //  add rsp amount* sizeof(arg) ; align stack
            push_bytes(Pop_rbp, byte);            //  pop rbp                     ; restore old rbp
            push_bytes(Ret, byte);                //  ret

            CurNode = CurNode->Right;

            free(paramArr);
        }
    }

    comp->curb = comp->bin_code;
    makeHeader(comp);

    setLabels(comp);

    fwrite(comp->bin_code, sizeof(char), comp->cur_sz, output);
}


#define call_address ((comp->call_arr)[i]).own_address
#define func_address (comp->func_arr)[((comp->call_arr)[i]).func_num]

#define jmp_address  ((comp->jmp_arr)[i]).own_address
#define label_address (comp->label_arr)[((comp->jmp_arr)[i]).label_num]


bool setLabels(Compiler *comp)
{
    if (!comp) {
        printf("Error, nullptr as a parameter in function: %s\n", __PRETTY_FUNCTION__);
        return false;
    }

    for (int i = 0; i < comp->cur_call; i++) {
        comp->curb = call_address + comp->bin_code + 1;

        if (call_address < func_address)
            push_bytes(func_address - call_address - 5, d_word);

        else
            push_bytes((0x100000000 - call_address + func_address - 5), d_word);
    }

    for (int i = 0; i < comp->cur_jmp; i++) {
        comp->curb = jmp_address + comp->bin_code;

        if (jmp_address < label_address)
            push_bytes(-jmp_address + label_address - 4, d_word);

        else
            push_bytes((0x100000000 - jmp_address + label_address - 4), d_word);
    }

    return true;
}

#undef call_address
#undef func_address
#undef jmp_address
#undef label_address


#define binary_op(asm_func)                                         \
do{                                                                 \
    compileRecFunc(node->Left, comp);                               \
    if (node->Left->type == call)                                   \
    push_bytes(Push_rax, byte);                                     \
                                                                    \
    compileRecFunc(node->Right, comp);                              \
    if (node->Right->type == call)                                  \
    push_bytes(Push_rax, byte);                                     \
                                                                    \
    push_bytes(Fld_Q_Word_Second_arg, d_word);                      \
    push_bytes(Fld_Q_Word_First_arg, 3);                            \
                                                                    \
    ADD_RSP(elem_sz);                                               \
                                                                    \
    push_bytes(asm_func, word);                                     \
    push_bytes(FSTP_Q_Word_RSP, 3);                                 \
} while(0)


bool
compileRecFunc(const Node *node, Compiler *comp)
{

    compCheck(comp);

    switch (node->type) {
        case Opera: {
            if (node->Left)
                compileRecFunc(node->Left, comp);
            if (node->Right)
                compileRecFunc(node->Right, comp);
            break;
        }

        case Add : {
            binary_op(FADDP);
            break;
        }

        case Sub : {
            binary_op(FSUBP);
            break;
        }

        case Div : {
            binary_op(FDIVP);
            break;
        }

        case Mul : {
            binary_op(FMULP);
            break;
        }

#undef binary_op

        case Sqrt: {
            compileRecFunc(node->Left, comp);

            if (node->Left->type == call)
                push_bytes(Push_rax, byte);

            push_bytes(Fld_Q_Word_First_arg, 3);
            push_bytes(FSQRT, word);
            push_bytes(FSTP_Q_Word_RSP, 3);

            break;
        }

        case charConst: {
            int counter = 0;
            int unknownFuncFlag = 1;

            long ArgAmount = comp->cur_arg_Amount;
            const char **CurrentVariables = comp->cur_param_Arr;
            for (counter = 0; counter < ArgAmount; counter++) {
                if (strcmp(node->content, CurrentVariables[counter]) == 0) {
                    unknownFuncFlag = 0;
                    break;
                }
            }

            if (unknownFuncFlag) {
                node->myTree->errFlag = true;
                return false;
            }

            PUSH_Q_RBP(elem_sz * (counter + 1));
            break;
        }

        case call : {
            long argAmount = strtol(node->Right->Left->content, nullptr, 10);
            Node *paramNode = node->Left;

            for (int i = 0; i < argAmount; i++) {

                compileRecFunc(paramNode->Left, comp);
                push_bytes(Pop_rax, byte);
                MOV_RSP_SUB_RAX ((3 + i) * elem_sz);
                paramNode = paramNode->Right;
            }

            current_call.own_address = comp->cur_sz;
            push_bytes(CALL, byte);
            current_call_inc.func_num = (size_t) strtol(node->Right->content, nullptr, 10);
            push_bytes(0, d_word);

            break;
        }


        case number: {
            double num = strtod(node->content, nullptr);
            push_bytes(mov_r14, word);
            memcpy(comp->curb, &num, 8);
            comp->curb += 8;
            comp->cur_sz += 8;
            push_bytes(Push_r14, word);
            break;
        }

        case Assign: {
            compileRecFunc(node->Right, comp);
            if (node->Right->type == call)
                push_bytes(Push_rax, byte);

            int counter = 0;
            const char **CurrentVariables = comp->cur_param_Arr;

            while (CurrentVariables[counter]) {
                if (strcmp(node->Left->content, CurrentVariables[counter]) == 0)
                    break;
                counter++;
            }

            push_bytes(Pop_rax, byte);
            MOV_RBP_SUB_RAX(elem_sz * (counter + 1));
            break;
        }


        case write : {
            compileRecFunc(node->Left, comp);

            if (node->Left->type == call)
                push_bytes(Push_rax, byte);

            push_bytes(0x2404100ff2, 5);    //movsd xmm0, qword [rsp]
            push_bytes(Pop_rax, byte);
            push_bytes(CALL, byte);
            push_bytes((0x100000000 - (comp->cur_sz) + comp->PrintFuncAddress - 4), d_word);
            break;
        }

        case read : {

            int unknownFuncFlag = 1;
            int counter = 0;

            long ArgAmount = comp->cur_arg_Amount;
            const char **CurrentVariables = comp->cur_param_Arr;

            for (counter = 0; counter < ArgAmount; counter++) {
                if (strcmp(node->Left->content, CurrentVariables[counter]) == 0) {
                    unknownFuncFlag = 0;
                    break;
                }
            }

            if (unknownFuncFlag) {
                node->myTree->errFlag = true;
                return false;
            }

            push_bytes(CALL, byte);
            push_bytes((0x100000000 - (comp->cur_sz) + comp->ScanFuncAddress - 4), d_word);
            push_bytes(0xc07e0f4866, 5);            //movq rax, xmm0
            MOV_RBP_SUB_RAX(elem_sz * (counter + 1));

            break;


        }

        case _return: {
            compileRecFunc(node->Left, comp);
            push_bytes(Pop_rax, byte);
            break;
        }

        case _else: {

            int exitLabel = comp->labelNum;
            comp->labelNum += 3;

            if (node->Left)
                compileRecFunc(node->Left, comp);
            if (node->Right)
                compileRecFunc(node->Right, comp);

            comp->label_arr[exitLabel] = comp->cur_sz;

            break;
        }


#define jmp_instr_if(cmd)                                                           \
do{                                                                                 \
    push_bytes(cmd, word);                                                          \
    comp->jmp_arr[comp->cur_jmp].own_address = comp->cur_sz;                        \
    push_bytes(0, d_word);                                                          \
                                                                                    \
    comp->jmp_arr[comp->cur_jmp].label_num = (size_t) (comp->labelNum - 2);         \
    comp->cur_jmp++;                                                                \
}while (0)


#define jmp_instr_while(cmd)                                                        \
do {                                                                                \
    push_bytes(cmd, word);                                                          \
    comp->jmp_arr[comp->cur_jmp].own_address = comp->cur_sz;                        \
    push_bytes(0, d_word);                                                          \
                                                                                    \
    comp->jmp_arr[comp->cur_jmp].label_num = (size_t) (ExitLabel);                  \
    comp->cur_jmp++;                                                                \
                                                                                    \
} while (0)

        case _if: {

            int ElseLabel = comp->labelNum - 2;
            int ExitLabel = comp->labelNum - 3;

            compileRecFunc(node->Left->Left, comp);
            compileRecFunc(node->Left->Right, comp);


            push_bytes(Fld_Q_Word_First_arg, 3);
            push_bytes(Fld_Q_Word_Second_arg, d_word);

            ADD_RSP(elem_sz * 2);

            push_bytes(FCOMMP, word);

            push_bytes(FNSTSW_AX, word);
            push_bytes(SAHF, byte);

            switch (node->Left->type) {

                case equal : {
                    jmp_instr_if(jne);
                    break;
                }

                case notEqual: {
                    jmp_instr_if(je);
                    break;
                }

                case biggerOrEq: {
                    jmp_instr_if(jb);
                    break;
                }

                case bigger: {
                    jmp_instr_if(jbe);
                    break;;
                }

                case smaller: {
                    jmp_instr_if(jae);
                    break;
                }

                case smallerOrEq: {
                    jmp_instr_if(ja);
                    break;

                }

#undef jmp_instr_if

                default:
                    return false;

            }

            compileRecFunc(node->Right, comp);

            push_bytes(jmp, byte);
            comp->jmp_arr[comp->cur_jmp].own_address = comp->cur_sz;
            push_bytes(0, d_word);

            comp->jmp_arr[comp->cur_jmp].label_num = (size_t) ExitLabel;
            comp->cur_jmp++;

            comp->label_arr[ElseLabel] = comp->cur_sz;

            break;
        }

        case _while: {

            int ExitLabel = comp->labelNum;
            int StartLabel = comp->labelNum + 1;
            comp->labelNum += 3;

            comp->label_arr[StartLabel] = comp->cur_sz;


            compileRecFunc(node->Left->Left, comp);
            compileRecFunc(node->Left->Right, comp);

            push_bytes(Fld_Q_Word_First_arg, 3);
            push_bytes(Fld_Q_Word_Second_arg, d_word);

            ADD_RSP(elem_sz * 2);

            push_bytes(FCOMMP, word);

            push_bytes(FNSTSW_AX, word);
            push_bytes(SAHF, byte);

            switch (node->Left->type) {

                case equal : {
                    jmp_instr_while(jne);
                    break;
                }

                case notEqual: {
                    jmp_instr_while(je);
                    break;
                }

                case biggerOrEq: {
                    jmp_instr_while(jb);
                    break;

                }

                case bigger: {
                    jmp_instr_while(jbe);
                    break;
                }

                case smaller: {
                    jmp_instr_while(jae);
                    break;
                }

                case smallerOrEq: {
                    jmp_instr_while(ja);
                    break;
                }

                default:
                    return false;
            }

#undef jmp_instr_while

            compileRecFunc(node->Right, comp);
            push_bytes(jmp, byte);
            comp->jmp_arr[comp->cur_jmp].own_address = comp->cur_sz;
            push_bytes(0, d_word);

            comp->jmp_arr[comp->cur_jmp].label_num = (size_t) StartLabel;
            comp->cur_jmp++;

            comp->label_arr[ExitLabel] = comp->cur_sz;

            break;
        }

        default:
            return false;
    }

    return true;
}


bool getSysFunc(Compiler *comp)
{
    size_t func_sz = 0;
    char *funcBuf = getFunc(PrintObj, &func_sz);

    if (!funcBuf)
        return false;

    comp->PrintFuncAddress = comp->cur_sz;

    memcpy(comp->curb, funcBuf, func_sz);
    comp->curb += func_sz;
    comp->cur_sz += func_sz;
    free(funcBuf);

    funcBuf = getFunc(ScanObj, &func_sz);

    if (!funcBuf)
        return false;

    comp->ScanFuncAddress = comp->cur_sz;

    memcpy(comp->curb, funcBuf, func_sz);
    comp->curb += func_sz;
    comp->cur_sz += func_sz;

    return true;
}


char *getFunc(const char *filename, size_t *sizeOfBuf)
{
    if (!filename || !sizeOfBuf)
        return nullptr;


    size_t sizeOfBuffer1 = 0;
    FILE *file = fopen(filename, "rb");

    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    sizeOfBuffer1 = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    auto helpBuffer = (char *) calloc(sizeOfBuffer1 + 1, sizeof(char));

    if (!helpBuffer)
        return nullptr;

    fread(helpBuffer, 1, sizeOfBuffer1, file);
    fclose(file);

    *sizeOfBuf = sizeOfBuffer1;
    return helpBuffer;
}


bool constructComp(Compiler *comp)
{
    if (!comp)
        return false;

    comp->bin_code = (char *) calloc(startElf_sz, sizeof(char));
    comp->call_arr = (Call *) calloc(startCall_amount, sizeof(Call));
    comp->func_arr = (size_t *) calloc(startFunc_amount, sizeof(int));
    comp->jmp_arr = (Jump *) calloc(startJump_amount, sizeof(Jump));
    comp->label_arr = (size_t *) calloc(startLabel_amount, sizeof(int));

    if (!comp->bin_code ||
        !comp->call_arr ||
        !comp->func_arr ||
        !comp->jmp_arr ||
        !comp->label_arr)
        return false;

    comp->cur_jmp = 0;
    comp->cur_sz = 0;
    comp->cur_call = 0;
    comp->labelNum = 0;
    comp->cur_func = 0;

    comp->bin_code_capacity = startElf_sz;
    comp->call_arr_capacity = startCall_amount;
    comp->call_func_arr_capacity = startFunc_amount;
    comp->jmp_arr_capacity = startJump_amount;
    comp->jmp_labels_capacity = startLabel_amount;

    treeConstruct(&comp->tree);
    return true;
}


bool compCheck(Compiler *comp)
{
    if (!comp)
        return false;

    if (comp->cur_sz > comp->bin_code_capacity / 2) {
        comp->bin_code_capacity *= 2;
        comp->bin_code_capacity++;
        comp->bin_code = (char *) realloc(comp->bin_code, comp->bin_code_capacity);
        comp->curb = comp->bin_code + comp->cur_sz;
    }

    if (comp->cur_jmp > comp->jmp_arr_capacity / 2) {
        comp->jmp_arr_capacity *= 2;
        comp->jmp_arr_capacity++;
        comp->jmp_arr = (Jump *) realloc(comp->jmp_arr, comp->jmp_arr_capacity * sizeof(Jump));
    }

    if (comp->cur_call > comp->call_arr_capacity / 2) {
        comp->call_arr_capacity *= 2;
        comp->call_arr_capacity++;
        comp->call_arr = (Call *) realloc(comp->call_arr, comp->call_arr_capacity * sizeof(Call));
    }

    if (comp->labelNum > comp->jmp_labels_capacity / 2) {
        comp->jmp_labels_capacity *= 2;
        comp->jmp_labels_capacity++;
        comp->label_arr = (size_t *) realloc(comp->label_arr, comp->jmp_labels_capacity * sizeof(size_t));
    }

    if (comp->cur_func > comp->call_func_arr_capacity / 2) {
        comp->call_func_arr_capacity *= 2;
        comp->call_func_arr_capacity++;
        comp->func_arr = (size_t *) realloc(comp->func_arr, comp->call_func_arr_capacity * sizeof(size_t));
    }

    return true;

}


bool destructComp(Compiler *comp)
{
    if (!comp)
        return false;

    destructTree(&(comp->tree));
    free(comp->bin_code);
    free(comp->call_arr);
    free(comp->func_arr);
    free(comp->jmp_arr);
    free(comp->label_arr);

    comp->bin_code = nullptr;
    comp->label_arr = nullptr;
    comp->curb = nullptr;
    comp->call_arr = nullptr;
    comp->func_arr = nullptr;
    comp->jmp_arr = nullptr;
    return true;
}


int makeHeader(Compiler *comp)
{
    size_t cur_sz = comp->cur_sz;
    push_bytes(0x7f, byte);              // signature
    push_bytes(0x45, byte);              //  'E'
    push_bytes(0x4c, byte);              //  'L'
    push_bytes(0x46, byte);              //  'F'

    push_bytes(0x02, byte);              // 64-bit file
    push_bytes(0x01, byte);              // little endian
    push_bytes(0x01, byte);              // ELF version
    push_bytes(0x0, byte);               // Unix system

    push_bytes(0x0, q_word);             // padding bytes
    push_bytes(0x02, word);              // executable file

    push_bytes(0x3e, word);              // x86-64

    push_bytes(0x01, d_word);            // version of format

    push_bytes(0x400080, q_word);        // set the entry point
    push_bytes(0x40, q_word);            // offset of the headers table
    push_bytes(0x0, q_word);             //

    push_bytes(0x00, d_word);            // flags

    push_bytes(0x40, word);              // size of header
    push_bytes(0x38, word);              //
    push_bytes(0x01, word);              // num of sections

    push_bytes(0x00, word);              //
    push_bytes(0x00, word);              // num of section headers
    push_bytes(0x00, word);              // e_shstrndx

    push_bytes(0x01, d_word);            // PT_LOAD
    push_bytes(0x05, d_word);            // executable

    push_bytes(0x00, q_word);            // p_offset
    push_bytes(0x400000, q_word);        // virtual address of the segment
    push_bytes(0x400000, q_word);        // physical --------//-----------
    push_bytes(cur_sz, q_word);          // size of the segment in file
    push_bytes(cur_sz, q_word);          //                     in memory
    push_bytes(0x02000, q_word);         // p_align
}


#undef current_call
#undef current_call_inc
#undef push_bytes
#undef ADD_RSP
#undef SUB_RSP
#undef MOV_RBP_SUB_RAX
#undef MOV_RSP_SUB_RAX
#undef PUSH_Q_RBP
#undef db
#undef dw
#undef dd
#undef dq
