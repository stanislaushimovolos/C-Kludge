//
// Created by stanissslaf on 4/28/18.
//

#include "Nasm.h"
#include "HexTranslation.h"


int compileN(Tree *tree, FILE *output)
{
    if (tree && tree->root) {

        tree->labelNum = 0;
        fprintf(output, "\n%%include \"INCLUDE.asm\"\n"
                        "section\t.text\n"
                        "global _start\n"
                        "\n"
                        "\n"
                        "_start :\n"
                        "\n"
                        "call f%d\n"
                        "\tmov rax, 60\n"
                        "\txor rdi, rdi\n"
                        "\tsyscall\n\n", tree->MainFlag);

        auto CurNode = tree->root;
        while (CurNode) {
            if (CurNode->Left) {
                auto argAmount = strtol(CurNode->Left->Left->Left->content, nullptr, 10);
                Node *paramNode = CurNode->Left->Left->Right;

                auto paramArr = (param *) calloc((size_t) argAmount + 1, sizeof(param));

                for (int i = 0; i < argAmount; i++) {
                    paramArr[i].paramName = paramNode->Left->content;
                    paramArr[i].paramNum = i;
                    paramNode = paramNode->Right;
                }
                fprintf(output, "f%s:\n"
                                "\tpush rbp\n"
                                "\tmov rbp, rsp\n"
                                "\tsub rsp, %ld\n", CurNode->Left->Left->content, (argAmount) * elem_sz);

                // tree->funcName = CurNode->Left->content;
                compileRecFuncN(CurNode->Left->Right, paramArr, argAmount, output);

                fprintf(output, "\tadd rsp, %ld\n"
                                "\tpop rbp\n"
                                "\tret\n\n", elem_sz * argAmount);

                CurNode = CurNode->Right;
            }
        }
        return tree->errFlag;
    }

    return false;
}


bool
compileRecFuncN(const Node *node, const param *CurrentVariables, long ArgAmount, FILE *output)
{

    switch (node->type) {
        case Opera: {
            if (node->Left)
                compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Right)
                compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            break;

        }
        case call : {

            long argAmount = strtol(node->Right->Left->content, nullptr, 10);
            Node *paramNode = node->Left;

            for (int i = 0; i < argAmount; i++) {

                compileRecFuncN(paramNode->Left, CurrentVariables, ArgAmount, output);
                fprintf(output, "\tpop rax\n");
                fprintf(output, "\tmov [rsp - %d], rax\n", (3 + i) * elem_sz);
                paramNode = paramNode->Right;
            }

            fprintf(output, "\tcall f%s\n", node->Right->content);
            break;
        }

        case Add : {

            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            if (node->Right->type == call)
                fprintf(output, "\tpush rax\n");

            fprintf(output, "\tFLD qword [rsp]\n"
                            "\tFLD qword [rsp  + 8]\n"
                            "\tadd rsp, 8\n"
                            "\tFADDP\n"
                            "\tFSTP qword  [rsp]\n");
            break;
        }

        case Sub: {

            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            if (node->Right->type == call)
                fprintf(output, "\tpush rax\n");

            fprintf(output, "\tFLD qword [rsp + 8]\n"
                            "\tFLD qword [rsp]\n"
                            "\tadd rsp, 8\n"
                            "\tFSUB\n"
                            "\tFSTP qword  [rsp]\n");
            break;
        }

        case Mul: {
            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            if (node->Right->type == call)
                fprintf(output, "\tpush rax\n");


            fprintf(output, "\tFLD qword [rsp]\n"
                            "\tFLD qword [rsp  + 8]\n"
                            "\tadd rsp, 8\n"
                            "\tFMUL\n"
                            "\tFSTP qword  [rsp]\n");
            break;

        }

        case Div : {
            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            if (node->Right->type == call)
                fprintf(output, "\tpush rax\n");

            fprintf(output, "\tFLD qword [rsp + 8]\n"
                            "\tFLD qword [rsp]\n"
                            "\tadd rsp, 8\n"
                            "\tFDIVP\n"
                            "\tFSTP qword  [rsp]\n");
            break;

        }

        case number: {
            char buf[23] = {0};
            buf[0] = '0';
            buf[1] = 'x';
            double num = strtod(node->content, nullptr);
            convertHexToStr(num, buf + 2);
            fprintf(output, "\tmov r14, %s\n"
                            "\tpush r14\n", buf);
            break;
        }

        case charConst: {
            int counter = 0;
            int unknownFuncFlag = 1;

            for (counter = 0; counter < ArgAmount; counter++) {
                if (strcmp(node->content, CurrentVariables[counter].paramName) == 0) {
                    unknownFuncFlag = 0;
                    break;
                }
            }

            if (unknownFuncFlag) {
//                printf("Undeclared variable %s in function %s\n", node->content, node->myTree->funcName);
                node->myTree->errFlag = true;
                return false;
            }

            fprintf(output, "\tpush qword [rbp -%d]\n", elem_sz * (counter + 1));
            break;
        }

        case write : {
            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);

            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            fprintf(output, "\tmovsd xmm0, qword [rsp]\n"
                            "\tpop rax\n"
                            "\tcall print\n");
            break;
        }

        case Sqrt: {
            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);

            if (node->Left->type == call)
                fprintf(output, "\tpush rax\n");

            fprintf(output, "\tFLD qword [rsp]\n"
                            "\tFSQRT\n"
                            "\tFSTP qword  [rsp]\n");
            break;
        }

        case Assign: {
            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            if (node->Right->type == call)
                fprintf(output, "\tpush rax\n");

            int counter = 0;
            while (CurrentVariables[counter].paramName) {
                if (strcmp(node->Left->content, CurrentVariables[counter].paramName) == 0)
                    break;
                counter++;
            }

            fprintf(output, "\tpop rax\n");
            fprintf(output, "\tmov [rbp -%d], rax\n", elem_sz * (counter + 1));
            break;
        }

        case _if: {

            int ElseLabel = node->myTree->labelNum - 2;
            int ExitLabel = node->myTree->labelNum - 3;


            compileRecFuncN(node->Left->Left, CurrentVariables, ArgAmount, output);
            compileRecFuncN(node->Left->Right, CurrentVariables, ArgAmount, output);

            fprintf(output, "\tFLD qword [rsp]\n"
                            "\tFLD qword [rsp + 8]\n"
                            "\tadd rsp, 16\n"
                            "\tfcompp\n"
                            "\tfstsw ax\n"
                            "\tsahf\n");


            switch (node->Left->type) {

                case equal : {
                    fprintf(output,
                            "\tjne j%d\n", node->myTree->labelNum - 2);
                    break;
                }

                case notEqual: {
                    fprintf(output,
                            "\tje j%d\n", node->myTree->labelNum - 2);
                    break;
                }

                case biggerOrEq: {
                    fprintf(output,
                            "\tjb j%d\n", node->myTree->labelNum - 2);
                    break;

                }

                case bigger: {
                    fprintf(output,
                            "\tjbe j%d\n", node->myTree->labelNum - 2);
                    break;
                }

                case smaller: {
                    fprintf(output,
                            "\tjae j%d\n", node->myTree->labelNum - 2);
                    break;
                }

                case smallerOrEq: {
                    fprintf(output,
                            "\tja j%d\n", node->myTree->labelNum - 2);
                    break;
                }

                default:
                    return false;
            }

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            fprintf(output, "\tjmp j%d\n", ExitLabel);
            fprintf(output, "j%d :\n", ElseLabel);

            break;
        }

        case _while: {

            int ExitLabel = node->myTree->labelNum;
            int StartLabel = node->myTree->labelNum + 1;
            node->myTree->labelNum += 3;

            fprintf(output, "j%d :\n", StartLabel);

            compileRecFuncN(node->Left->Left, CurrentVariables, ArgAmount, output);
            compileRecFuncN(node->Left->Right, CurrentVariables, ArgAmount, output);

            fprintf(output, "\tFLD qword [rsp]\n"
                            "\tFLD qword [rsp + 8]\n"
                            "\tadd rsp, 16\n"
                            "\tfcompp\n"
                            "\tfstsw ax\n"
                            "\tsahf\n");


            switch (node->Left->type) {

                case equal : {
                    fprintf(output,
                            "\tjne j%d\n", ExitLabel);
                    break;
                }

                case notEqual: {
                    fprintf(output,
                            "\tje j%d\n", ExitLabel);
                    break;
                }

                case biggerOrEq: {
                    fprintf(output,
                            "\tjb j%d\n", ExitLabel);
                    break;

                }

                case bigger: {
                    fprintf(output,
                            "\tjbe j%d\n", ExitLabel);
                    break;
                }

                case smaller: {
                    fprintf(output,
                            "\tjae j%d\n", ExitLabel);
                    break;
                }

                case smallerOrEq: {
                    fprintf(output,
                            "\tja j%d\n", ExitLabel);
                    break;
                }

                default:
                    return false;
            }

            compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);
            fprintf(output, "\tjmp j%d\n", StartLabel);
            fprintf(output, "j%d :\n", ExitLabel);

            break;
        }

        case _else: {

            int exitLabel = node->myTree->labelNum;
            node->myTree->labelNum += 3;

            if (node->Left)
                compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            if (node->Right)
                compileRecFuncN(node->Right, CurrentVariables, ArgAmount, output);

            fprintf(output, "j%d :\n", exitLabel);

            break;
        }

        case _return: {
            compileRecFuncN(node->Left, CurrentVariables, ArgAmount, output);
            fprintf(output, "\tpop rax\n");
            break;
        }

        case read: {

            int counter = 0;
            while (CurrentVariables[counter].paramName) {
                if (strcmp(node->Left->content, CurrentVariables[counter].paramName) == 0)
                    break;
                counter++;
            }

            fprintf(output, "\tcall scan\n"
                            "\tmovq rax, xmm0\n");
            fprintf(output, "\tmov [rbp -%d], rax\n", elem_sz * (counter + 1));
        }


        default:
            return false;
    }
    return true;
}


