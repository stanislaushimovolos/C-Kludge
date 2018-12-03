#include "Descent.h"
#include "../Tree/Tree.h"


int getTree(parser *pars)
{
    int result = getNewFunc(pars);
    while (result == 1)
        result = getNewFunc(pars);
    for (int i = 0; i < pars->curFunction; i++)
        deleteLinkers(pars->functions[i].subTree.root);

    if (result == -1)
        result = 1;
    return result;
}


#define   SKIP_SPACES                           \
while (isspace (pars->code[pars->curCodePos]))  \
{                                               \
    if (pars->code[pars->curCodePos] == '\n')   \
        pars->curLine++;                        \
    pars->curCodePos++;                         \
}


int getNewFunc(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    if (strncmp(pars->code + pars->curCodePos, functionFlag, sizeof(functionFlag) - 1) != 0)
        return 0;
    pars->curCodePos += sizeof(functionFlag) - 1;

    SKIP_SPACES;
    size_t tokenSize = 0;
    while (isalpha(pars->code[pars->curCodePos + tokenSize]))
        tokenSize++;

    for (int i = 0; i < pars->curFunction; i++)
    {
        if (strncmp(pars->functions[i].name, pars->code + pars->curCodePos, tokenSize) == 0)
        {
            pars->curCodePos += tokenSize;

            int positionKeeper = pars->curFunction;
            pars->curFunction = i;
            pars->tree = &pars->functions[pars->curFunction].subTree;

            int paramNum = 0;
            Node *functionParams = getArguments(pars, &paramNum);
            if (!functionParams)
                return -1;

            if (paramNum != pars->functions[i].argNumber)
            {
                printf("Error, different number of variable in prototype and in the function \"%s\"\nLine %d\n",
                       pars->functions[i].name, pars->curLine);
                destructNodeRec(functionParams);

                pars->curFunction = positionKeeper;
                return -1;
            }
            destructNodeRec(pars->functions[i].subTree.root->left);
            connectLeft(pars->functions[i].subTree.root, functionParams);

            if (pars->functions[i].subTree.root->right)
            {
                printf("Error, redeclaration of the function \"%s\"\nLine %d\n", pars->functions[i].name,
                       pars->curLine);

                pars->curFunction = positionKeeper;
                return -1;
            }
            Node *functionBody = getFuncBranches(pars);
            if (!functionBody)
            {
                printf("Error, expected body of the function \"%s\"\n", pars->functions[i].name);
                pars->curFunction = positionKeeper;
                return -1;
            }

            connectRight(pars->functions[i].subTree.root, functionBody);
            pars->curFunction = positionKeeper;
            return 1;
        }
    }

    func *currentFunction = &pars->functions[pars->curFunction];

    currentFunction->name = (char *) calloc(tokenSize + 1, sizeof(char));
    memcpy(currentFunction->name, pars->code + pars->curCodePos, tokenSize);
    pars->curCodePos += tokenSize;
    pars->tree = &currentFunction->subTree;

    int paramNum = 0;
    Node *functionParams = getArguments(pars, &paramNum);
    if (!functionParams)
    {
        free(currentFunction->name);
        return -1;
    }

    currentFunction->argNumber = paramNum;
    currentFunction->varNumber = paramNum;
    currentFunction->subTree.root = createSimpleNode(Function, pars->tree);
    connectLeft(currentFunction->subTree.root, functionParams);

    Node *functionBody = getFuncBranches(pars);
    if (functionBody)
        connectRight(currentFunction->subTree.root, functionBody);
    else
    {
        SKIP_SPACES;
        if (pars->code[pars->curCodePos] != ';')
        {
            printf("Error, expected \";\" after prototype of the function \"%s\"\nLine %d\n",
                   pars->functions[pars->curFunction].name, pars->curLine);
            return -1;
        }
        pars->curCodePos++;
    }
    if (strcmp(currentFunction->name, "main") == 0)
        pars->numberOfMainFunc = pars->curFunction;

    pars->curFunction++;
    return 1;
};


Node *getArguments(parser *pars, int *argAmount)
{
    assert(pars);
    SKIP_SPACES;

    int paramNum = 0;
    if (pars->code[pars->curCodePos] == '(')
    {
        pars->curCodePos++;

        Node *curParam = getAddSub(pars);
        if (!curParam)
        {
            SKIP_SPACES;
            if (pars->code[pars->curCodePos] != ')')
            {
                printf("Error, expected \")\"\nLine %d\n", pars->curLine);
                return NULL;
            }
            pars->curCodePos++;
            return createSimpleNode(Linker, pars->tree);
        }

        Node *primaryParamNode = createSimpleNode(Param, pars->tree);
        Node *retNode = primaryParamNode;

        connectLeft(primaryParamNode, curParam);
        Node *secondaryParamNode = NULL;

        paramNum++;

        SKIP_SPACES;
        while (pars->code[pars->curCodePos] == ',')
        {
            secondaryParamNode = createSimpleNode(Param, pars->tree);
            paramNum++;
            pars->curCodePos++;

            curParam = getAddSub(pars);

            connectLeft(secondaryParamNode, curParam);
            connectRight(primaryParamNode, secondaryParamNode);

            primaryParamNode = secondaryParamNode;
            SKIP_SPACES;
        }

        SKIP_SPACES;
        if (pars->code[pars->curCodePos] != ')')
        {
            printf("Error, expected \")\"\nLine %d\n", pars->curLine);
            return NULL;
        }
        pars->curCodePos++;
        if (argAmount != NULL)
            *argAmount = paramNum;

        return retNode;
    }
    return NULL;
}


Node *getBody(parser *pars)
{
    Node *curOperation = getCommand(pars);
    if (!curOperation)
        return NULL;

    Node *primaryLinker = createSimpleNode(Linker, pars->tree);
    Node *bodyNode = primaryLinker;

    connectLeft(primaryLinker, curOperation);
    SKIP_SPACES;

    Node *secondaryLinker = NULL;
    while (pars->code[pars->curCodePos] == ';')
    {
        pars->curCodePos++;

        curOperation = getCommand(pars);
        if (!curOperation)
            return bodyNode;

        secondaryLinker = createSimpleNode(Linker, pars->tree);
        connectRight(primaryLinker, secondaryLinker);

        connectLeft(secondaryLinker, curOperation);
        primaryLinker = secondaryLinker;
        SKIP_SPACES;
    }
    SKIP_SPACES;

    if (pars->code[pars->curCodePos] != ';')
    {
        destructNodeRec(bodyNode);
        printf("Error, expected \";\"\nLine %d\n", pars->curLine);
        return NULL;
    }
    return bodyNode;
}


Node *getFuncBranches(parser *pars)
{
    SKIP_SPACES;

    if (pars->code[pars->curCodePos] == '{')
    {
        pars->curCodePos++;
        Node *node = getBody(pars);
        if (!node)
            return NULL;

        SKIP_SPACES;
        if (pars->code[pars->curCodePos] != '}')
        {
            printf("Line %d Error, expected \"}\" \n", pars->curLine);
            return NULL;
        }

        pars->curCodePos++;
        return node;
    }
    return NULL;
}


Node *getElse(parser *pars)
{
    SKIP_SPACES;
    if (strncmp(pars->code + pars->curCodePos, "else", sizeof("else") - 1) != 0)
        return NULL;
    pars->curCodePos += sizeof("else") - 1;

    Node *elseNode = createSimpleNode(Else, pars->tree);

    Node *returnNode = elseNode;
    Node *secondaryNode = getFuncBranches(pars);

    if (!secondaryNode)
        secondaryNode = getCommand(pars);
    if (!secondaryNode)
        return NULL;

    Node *SepNode = createSimpleNode(Linker, pars->tree);
    connectRight(elseNode, SepNode);
    connectLeft(SepNode, secondaryNode);

    if (secondaryNode->type != If)
        return returnNode;

    SKIP_SPACES;
    while (strncmp(pars->code + pars->curCodePos, "else", sizeof("else") - 1) != 0)
    {
        pars->curCodePos += sizeof("else") - 1;

        connectRight(SepNode, createSimpleNode(Linker, pars->tree));
        SepNode = SepNode->right;

        elseNode = getCommand(pars);

        if (!elseNode)
            elseNode = getFuncBranches(pars);
        if (!elseNode)
            return NULL;

        connectLeft(SepNode, elseNode);

        if (elseNode->type != If)
            return returnNode;
    }

    SKIP_SPACES;
    return returnNode;
}


Node *getCommand(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    Node *node = getId(pars);
    if (!node)
        return NULL;

    switch (node->type)
    {
        case While:
        {
            Node *LeftNode = getCond(pars);
            if (!LeftNode)
                return NULL;

            connectLeft(node, LeftNode);
            Node *RightNode = getFuncBranches(pars);

            if (!RightNode)
                RightNode = getCommand(pars);

            if (!RightNode)
                return NULL;

            connectRight(node, RightNode);
            return node;
        }

        case If:
        {
            Node *conditionNode = getCond(pars);
            if (!conditionNode)
                return NULL;

            Node *bodyNode = getFuncBranches(pars);
            if (!bodyNode)
                bodyNode = getCommand(pars);
            //Проверочку бы
            connectLeft(node, conditionNode);
            connectRight(node, bodyNode);

            Node *elseNode = getElse(pars);
            if (elseNode)
            {
                connectLeft(elseNode, node);
                return elseNode;
            }
            else
                elseNode = createSimpleNode(Else, pars->tree);

            connectLeft(elseNode, node);
            return elseNode;
        }

        case Return:
        {
            Node *retNode = getAddSub(pars);
            connectLeft(node, retNode);
            return node;
        }
        case Init:
        {
            Node *NewVarNode = getId(pars);
            Node *ParamNode = pars->tree->root->left;

            while (ParamNode)
            {
                if (strcmp(NewVarNode->content, ParamNode->left->content) == 0)
                {
                    printf("Line %d %s was declared earlier\n", pars->curLine, NewVarNode->content);
                    return NULL;
                }
                ParamNode = ParamNode->right;
            }

            Node *NewParamNode = createSimpleNode(Param, pars->tree);
            connectLeft(NewParamNode, NewVarNode);

            if (pars->tree->root->left)
            {
                ParamNode = pars->tree->root->left;
                while (ParamNode->right)
                    ParamNode = ParamNode->right;

                connectRight(ParamNode, NewParamNode);
            }
            else
                connectLeft(pars->tree->root, NewParamNode);
            pars->functions[pars->curFunction].varNumber++;

            return createSimpleNode(Linker, pars->tree);
        }

        case Read:
        {
            SKIP_SPACES;
            if (pars->code[pars->curCodePos] != '(')
            {
                printf("Line %d Error, expected %s \"(\"\n", pars->curLine, node->content);
                return NULL;
            }
            else
            {
                pars->curCodePos++;
                Node *FuncName = getAddSub(pars);
                if (!FuncName)
                {
                    printf("Line %d Error, expected variable %s \n", pars->curLine, node->content);
                    return NULL;
                }
                SKIP_SPACES;
                if (pars->code[pars->curCodePos] != ')')
                {
                    printf("Line %d Error, expected %s \")\"\n", pars->curLine, node->content);
                    return NULL;
                }
                pars->curCodePos++;

                connectLeft(node, FuncName);
                return node;
            }
        }
        case Write:
        {
            SKIP_SPACES;
            if (pars->code[pars->curCodePos] != '(')
            {
                printf("Line %d Error, expected %s \"(\"\n", pars->curLine, node->content);
                return NULL;
            }
            else
            {
                pars->curCodePos++;
                Node *FuncName = getAddSub(pars);
                if (!FuncName)
                {
                    printf("Line %d Error, expected variable %s \n", pars->curLine, node->content);
                    return NULL;
                }
                SKIP_SPACES;
                if (pars->code[pars->curCodePos] != ')')
                {
                    printf("Line %d Error, expected %s \")\"\n", pars->curLine, node->content);
                    return NULL;
                }
                pars->curCodePos++;

                connectLeft(node, FuncName);
                return node;
            }
        }
        case CharConst:
        {
            Node *argumentsNode = getArguments(pars, NULL);
            if (argumentsNode)
            {
                // Проверочку бы (
                Node *callNode = createSimpleNode(Call, pars->tree);
                connectLeft(callNode, node);
                if (argumentsNode->type != Linker)
                    connectRight(callNode, argumentsNode);
                else
                    destructNode(argumentsNode);

                return callNode;
            }
            else
            {
                Node *assignNode = getAssign(pars);
                if (assignNode)
                {
                    connectLeft(assignNode, node);
                    return assignNode;
                }
            }
            return node;
        }
        case Call:
            return node;

        case Number:
            return node;
        default:
            return NULL;
    }
}


#define  DEF_CMD(operator_, number)                         \
   if (strcmp (#operator_, node->content) == 0)             \
     {                                                      \
        node->type = number;                                \
        return 0;                                           \
    }

int analyzeData(Node *node)
{
    assert(node);
    assert(node->content);

#include "Commands.h"

    node->type = CharConst;
    return 0;
}


Node *getId(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    size_t tokenSize = 0;
    while (isalpha(pars->code[pars->curCodePos + tokenSize]) || pars->code[pars->curCodePos + tokenSize] == '<' ||
           pars->code[pars->curCodePos + tokenSize] == '>' || pars->code[pars->curCodePos + tokenSize] == '=' ||
           pars->code[pars->curCodePos + tokenSize] == '!')
        tokenSize++;

    Node *node = NULL;

    if (tokenSize > 0)
    {
        constructNode(&node);
        node->content = (char *) calloc(sizeof(char), tokenSize + 1);
        memcpy(node->content, pars->code + pars->curCodePos, tokenSize);
        node->myTree = pars->tree;

        analyzeData(node);
        pars->curCodePos += tokenSize;

        if (node->type == CharConst)
        {
            Node *argumentsNode = getArguments(pars, NULL);
            if (argumentsNode)
            {
                Node *callNode = createSimpleNode(Call, pars->tree);
                if (argumentsNode->type != Linker)
                    connectRight(callNode, argumentsNode);

                int numberOfFunction = 0;
                for (int i = 0; i < pars->curFunction; i++)
                {
                    if (strcmp(node->content, pars->functions[i].name) == 0)
                    {
                        numberOfFunction = i;
                        break;
                    }
                }
                //Проверочку бы(
                connectLeft(callNode, createNumericalNode(Number, numberOfFunction,
                                                          pars->tree));

                destructNode(node);
                return callNode;
            }
        }

        if (node->type == Sqrt)
        {
            Node *body = getMathBranches(pars);
            connectLeft(node, body);
            return node;
        }
    }
    else
        node = getNumber(pars);

    return node;
}

#undef DEF_CMD


Node *getAssign(parser *pars)
{
    SKIP_SPACES;
    if ((pars->code)[pars->curCodePos] != '=')
        return NULL;

    Node *assignNode = createSimpleNode(Assign, pars->tree);
    pars->curCodePos++;

    Node *rvalue = getAddSub(pars);
    if (!rvalue)
    {
        destructNode(assignNode);
        printf("Line %d Error, expected rvalue after \"=\" \n", pars->curLine);
        return NULL;
    }

    connectRight(assignNode, rvalue);
    return assignNode;
}

Node *getCond(parser *pars)
{
    SKIP_SPACES;

    if (pars->code[pars->curCodePos] == '(')
    {
        pars->curCodePos++;
        Node *Left = getAddSub(pars);

        if (!Left)
            return NULL;

        Node *MainNode = getId(pars);
        if (!MainNode)
            return NULL;

        Node *Right = getAddSub(pars);
        if (!Right)
            return NULL;

        connectRight(MainNode, Right);
        connectLeft(MainNode, Left);

        if (pars->code[pars->curCodePos] != ')')
        {
            printf("Line %d Error, expected  %c \")\" \n", pars->curLine, pars->code[pars->curCodePos]);
            return 0;
        }
        pars->curCodePos++;
        return MainNode;
    }
    else
    {
        printf("Line %d Error, expected \"(\" %c \n", pars->curLine, pars->code[pars->curCodePos]);
        return 0;
    }
}


Node *getAddSub(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    //curLeftNode + other nodes (if there are any)
    Node *curLeftNode = getMulDiv(pars);
    if (!curLeftNode)
        return NULL;

    SKIP_SPACES;

    //plus or minus
    char op = 0;

    Node *curSignNode = NULL;
    Node *curRightNode = NULL;

    while (pars->code[pars->curCodePos] == '+' ||
           pars->code[pars->curCodePos] == '-')
    {
        SKIP_SPACES;
        op = pars->code[pars->curCodePos];
        pars->curCodePos++;

        switch (op)
        {
            case '+':
            {
                curSignNode = createSimpleNode(Add, pars->tree);
                break;
            }

            case '-':
            {
                curSignNode = createSimpleNode(Sub, pars->tree);
                break;
            }
            default:
                return NULL;
        }

        /*
         *                          ____________    ...
         *                         |            (next node)
         *                         | Left
         *                   ______|_______
         *                  |              |
         *           _______| curSignNode  |________
         *          |       |    + or -    |        |
         *          | Left  |______________|  Right |
         *  ________|__                           __|_________
         * |           |                         |            |
         * |curLeftNode|                         |curRightNode|
         * |___________|                         |____________|
         */


        connectLeft(curSignNode, curLeftNode);

        curRightNode = getMulDiv(pars);
        if (!curRightNode)
        {
            printf("Incorrect expression after \"%c\"\n", op);
            return NULL;
        }
        connectRight(curSignNode, curRightNode);

        //Push curSignNode below to connect new nodes.
        curLeftNode = curSignNode;
    }
    return curLeftNode;
}


Node *getMulDiv(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    //curLeftNode * other nodes (if there are any)
    Node *curLeftNode = getMathBranches(pars);
    if (!curLeftNode)
        return NULL;

    SKIP_SPACES;

    //mul or div
    char curOperator = 0;

    Node *curSignNode = NULL;
    Node *curRightNode = NULL;

    while (pars->code[pars->curCodePos] == '*' ||
           pars->code[pars->curCodePos] == '/')
    {
        SKIP_SPACES;
        curOperator = pars->code[pars->curCodePos];
        pars->curCodePos++;

        switch (curOperator)
        {
            case '*':
            {
                curSignNode = createSimpleNode(Mul, pars->tree);
                break;
            }
            case '/':
            {
                curSignNode = createSimpleNode(Div, pars->tree);
                break;
            }
            default:
                return NULL;
        }

        /*
         *                          ____________    ...
         *                         |            (next node)
         *                         | Left
         *                   ______|_______
         *                  |              |
         *           _______| curSignNode  |________
         *          |       |    / or *    |        |
         *          | Left  |______________|  Right |
         *  ________|__                           __|_________
         * |           |                         |            |
         * |curLeftNode|                         |curRightNode|
         * |___________|                         |____________|
         */


        connectLeft(curSignNode, curLeftNode);

        curRightNode = getMathBranches(pars);
        if (!curRightNode)
        {
            printf("Incorrect expression after %c\n", curOperator);
            return NULL;
        }
        connectRight(curSignNode, curRightNode);

        //Push curSignNode below to connect new nodes.
        curLeftNode = curSignNode;
    }
    return curLeftNode;
}


Node *getMathBranches(parser *pars)
{
    SKIP_SPACES;

    if (pars->code[pars->curCodePos] == '(')
    {
        pars->curCodePos++;
        Node *node = getAddSub(pars);

        if (pars->code[pars->curCodePos] != ')')
        {
            printf("Line %d Error, expected %c \")\" \n", pars->curLine, pars->code[pars->curCodePos]);
            return NULL;
        }
        pars->curCodePos++;
        return node;
    }

    return getId(pars);
}


Node *getNumber(parser *pars)
{
    assert(pars);
    SKIP_SPACES;

    int sizeOfNumber = 0;
    double val = 0;

    sscanf(pars->code + pars->curCodePos, "%lg%n", &val, &sizeOfNumber);

    if (sizeOfNumber == 0)
        return NULL;

    pars->curCodePos += sizeOfNumber;
    Node *number = createNumericalNode(Number, val, pars->tree);

    return number;
}

#undef SKIP_SPACES


int constructParser(parser *pars, const char *inputFileName)
{
    pars->curLine = 0;
    pars->curCodePos = 0;
    pars->curFunction = 0;
    pars->numberOfMainFunc = 0;

    pars->labelNum = 0;
    pars->functions = (func *) calloc(sizeof(func), startFuncNumber);

    int status = getBuf(pars, inputFileName);
    return status;
}


int destructParser(parser *pars)
{
    for (int i = 0; i < pars->curFunction; i++)
    {
        free(pars->functions[i].name);
        destructTree(&(pars->functions[i].subTree));
    }
    free(pars->functions);
    free(pars->code);
}

int getBuf(parser *data, const char *inputFileName)
{
    assert(data);
    assert (inputFileName);

    size_t l_buf_sz = 0;
    FILE *file = fopen(inputFileName, "r");

    if (!file)
    {
        printf("Couldn't open file:\n%s\n", inputFileName);
        return 1;
    }
    fseek(file, 0, SEEK_END);
    l_buf_sz = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    data->code = (char *) calloc(l_buf_sz + 1, sizeof(char));
    assert(data->code);

    fread(data->code, 1, l_buf_sz, file);
    fclose(file);

    return 0;
}


int deleteLinkers(Node *node)
{
    if (node)
        if (node->type == Linker)
        {
            if (!node->right && !node->left)
            {
                if (node->parent && node == node->parent->right)
                    node->parent->right = NULL;
                else if (node->parent)
                    node->parent->left = NULL;

                destructNode(node);
                return 0;
            }
        }
}
