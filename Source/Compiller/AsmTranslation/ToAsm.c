//
// Created by Tom on 17.12.2017.
//
#include "ToAsm.h"
#include "../Tree/Tree.h"


int compile(parser *pars, const char *fileName)
{
    pars->bin_code = fopen(fileName, "w");
    if (!pars->bin_code)
    {
        printf("Couldn't open file:\n%s\n", fileName);
        return 1;
    }
    fprintf(pars->bin_code, "call %d\nend\n", pars->numberOfMainFunc);

    pars->labelNum = pars->curFunction + 1;
    Node *node = NULL;

    for (int i = 0; i < pars->curFunction; i++)
    {
        node = pars->functions[i].subTree.root;
        fprintf(pars->bin_code, ": %d\n", i);
        pars->labelNum++;

        pars->argAmount = pars->functions[i].varNumber;
        Node *paramNode = node->left;
        pars->CurrentVariables = (char **) calloc((size_t) pars->argAmount + 1, sizeof(char *));

        for (int j = 0; j < pars->argAmount; j++)
        {
            pars->CurrentVariables[j] = paramNode->left->content;
            paramNode = paramNode->right;
        }
        compileRecFunc(node->right, pars, 0);
        free(pars->CurrentVariables);

        fprintf(pars->bin_code, "ret\n\n");
    }
    fclose(pars->bin_code);

    return 0;
}


void compileRecFunc(Node *node, parser *pars,
                    int curExitPoint)
{
    if (node)
        switch (node->type)
        {
            case Linker:
            {
                if (node->type == Linker)
                {
                    if (node->left)
                        compileRecFunc(node->left, pars, curExitPoint);
                    if (node->right)
                        compileRecFunc(node->right, pars, curExitPoint);
                }
                break;
            }
            case Call:
            {
                int argAmount = pars->functions[(int) (node->left->value)].argNumber;
                Node *paramNode = node->right;

                int counter = 0;

                for (int i = 0; i < argAmount; i++)
                {
                    compileRecFunc(paramNode->left, pars, curExitPoint);
                    fprintf(pars->bin_code, "push %d\n"
                                            "push %d\n"
                                            "add\n"
                                            "push ax\n"
                                            "add\n"
                                            "pop cx\n"
                                            "pop [cx]\n", counter, pars->argAmount);

                    paramNode = paramNode->right;
                    counter++;
                }
                fprintf(pars->bin_code, "push ax\n"
                                        "push %d\n"
                                        "add\n"
                                        "pop ax\n"
                                        "call %lg\n"
                                        "push ax\n"
                                        "push %d\n"
                                        "sub\n"
                                        "pop ax\n", pars->argAmount, node->left->value, pars->argAmount);
                break;
            }
            case CharConst:
            {
                int counter = 0;
                int unknownFuncFlag = 1;

                for (int i = 0; i < pars->argAmount; i++)
                {
                    if (strcmp(node->content, pars->CurrentVariables[counter]) == 0)
                    {
                        unknownFuncFlag = 0;
                        break;
                    }
                    counter++;
                }
                if (unknownFuncFlag)
                {
                    printf("Undeclared variable %s\n", node->content);
                    return;
                }

                fprintf(pars->bin_code, "push ax\n"
                                        "push %d\n"
                                        "add\n"
                                        "pop cx\n"
                                        "push [cx]\n", counter);
                break;
            }
            case Else:
            {
                int exitLabel = pars->labelNum;
                pars->labelNum++;
                compileRecFunc(node->left, pars, exitLabel);

                node = node->right;
                while (node)
                {
                    compileRecFunc(node->left, pars, exitLabel);
                    node = node->right;
                }
                fprintf(pars->bin_code, ": %d\n", exitLabel);
                break;
            }
            case If:
            {
                int exitLabel = pars->labelNum;
                pars->labelNum++;

                compileRecFunc(node->left->left, pars, curExitPoint);
                compileRecFunc(node->left->right, pars, curExitPoint);
                compileRecFunc(node->left, pars, curExitPoint);
                fprintf(pars->bin_code, " %d\n", exitLabel);

                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "jmp %d\n", curExitPoint);
                fprintf(pars->bin_code, ": %d\n", exitLabel);

                break;
            }
            case While:
            {
                int exitLabel = pars->labelNum;
                pars->labelNum++;
                int returnLabel = pars->labelNum;
                pars->labelNum++;

                fprintf(pars->bin_code, ": %d\n", returnLabel);
                compileRecFunc(node->left->left, pars, curExitPoint);
                compileRecFunc(node->left->right, pars, curExitPoint);
                compileRecFunc(node->left, pars, curExitPoint);
                fprintf(pars->bin_code, " %d\n", exitLabel);

                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "jmp %d\n", returnLabel);
                fprintf(pars->bin_code, ": %d\n", exitLabel);

                break;
            }
            case Assign:
            {
                compileRecFunc(node->right, pars, curExitPoint);

                int counter = 0;
                while (pars->CurrentVariables[counter] != NULL)
                {
                    if (strcmp(node->left->content, pars->CurrentVariables[counter]) == 0)
                        break;
                    counter++;
                }
                fprintf(pars->bin_code, "push ax\n"
                                        "push %d\n"
                                        "add\n"
                                        "pop  cx\n"
                                        "pop  [cx]\n", counter);
                break;
            }
            case Read:
            {
                int counter = 0;
                while (pars->CurrentVariables[counter] != NULL)
                {
                    if (strcmp(node->left->content, pars->CurrentVariables[counter]) == 0)
                        break;
                    counter++;
                }
                fprintf(pars->bin_code, "in\n"
                                        "push ax\n"
                                        "push %d\n"
                                        "add\n"
                                        "pop  cx\n"
                                        "pop  [cx]\n", counter);
                break;
            }
            case Add:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "add \n");
                break;
            }
            case Sub:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "sub \n");
                break;
            }
            case Mul:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "mul \n");
                break;
            }
            case Div:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                compileRecFunc(node->right, pars, curExitPoint);
                fprintf(pars->bin_code, "div \n");
                break;
            }
            case Return:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                break;
            }
            case Write:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                fprintf(pars->bin_code, "out\n");
                break;
            }
            case Sqrt:
            {
                compileRecFunc(node->left, pars, curExitPoint);
                fprintf(pars->bin_code, "sqrt\n");
                break;
            }
            case Number:
            {
                fprintf(pars->bin_code, "push %lg\n", node->value);
                break;
            }
            case Equal:
            {
                fprintf(pars->bin_code, "jne");
                break;
            }
            case NotEqual:
            {
                fprintf(pars->bin_code, "je");
                break;
            }
            case Bigger:
            {
                fprintf(pars->bin_code, "jle");
                break;
            }
            case Smaller:
            {
                fprintf(pars->bin_code, "jbe");
                break;
            }
            case SmallerOrEq:
            {
                fprintf(pars->bin_code, "jb");
                break;
            }
            case BiggerOrEq:
            {
                fprintf(pars->bin_code, "jl");
                break;
            }
            default:
                break;
        }
}