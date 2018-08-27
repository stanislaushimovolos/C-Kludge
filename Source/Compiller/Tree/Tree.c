#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "Tree.h"


void connectRight(Node *mainNode, Node *rightNode)
{
    assert (rightNode);
    assert (mainNode);

    mainNode->right = rightNode;
    rightNode->myTree = mainNode->myTree;
    rightNode->parent = mainNode;
    mainNode->myTree->nodeAmount++;
}


void connectLeft(Node *mainNode, Node *leftNode)
{
    assert (leftNode);
    assert (mainNode);

    mainNode->left = leftNode;
    leftNode->myTree = mainNode->myTree;
    leftNode->parent = mainNode;
    mainNode->myTree->nodeAmount++;
}


void visitTreePost(Node *node, int func(Node *node))
{

    if (node->left)
        visitTreePost(node->left, func);
    if (node->right)
        visitTreePost(node->right, func);
    func(node);
}

void visitTreePref(Node *node, int func(Node *node))
{
    func(node);
    if (node->left)
        visitTreePost(node->left, func);
    if (node->right)
        visitTreePost(node->right, func);
}


#define RETURN_COND(cond) return (cond) ? 0 : 1;

int constructNode(Node **node)
{
    Node *elem = (Node *) calloc(1, sizeof(Node));
    *node = elem;
    RETURN_COND (*node)
}


Node *copyTree(const Node *node, Tree *newTree)
{
    assert (node);
    Node *retNode;
    constructNode(&retNode);

    retNode->type = node->type;
    retNode->value = node->value;

    if (node->content)
    {
        retNode->content = (char *) calloc(strlen(node->content) + 1, sizeof(char));
        strcpy(retNode->content, node->content);
    }
    retNode->myTree = node->myTree;
    if (node->left)
    {
        retNode->left = copyTree(node->left, newTree);
        retNode->left->parent = retNode;
        retNode->left->myTree = newTree;
    }
    if (node->right)
    {
        retNode->right = copyTree(node->right, newTree);
        retNode->right->parent = retNode;
        retNode->right->myTree = newTree;
    }
    return retNode;
}


int treeConstruct(Tree *tree)
{
    assert (tree);
    tree->root = NULL;
    tree->eventFlag = 0;
    tree->nodeAmount = 0;
    return 0;
}


void destructNode(Node *node)
{
    free(node->content);
    free(node);
}


void destructTree(Tree *tree)
{
    if (tree->root)
    {
        destructNodeRec(tree->root);
        (tree->root) = NULL;
    }
}

void destructNodeRec(Node *node)
{
    if (node->left)
        destructNodeRec(node->left);
    if (node->right)
        destructNodeRec(node->right);
    destructNode(node);
}


Node *createLiteralNode(int type, const char *content, Tree *newTree)
{
    assert (content);
    assert (newTree);

    Node *mainNode = NULL;
    constructNode(&mainNode);

    mainNode->myTree = newTree;
    mainNode->content = (char *) calloc(strlen(content) + 1, sizeof(char));

    strcpy(mainNode->content, content);
    mainNode->type = type;
    return mainNode;
}


Node *createSimpleNode(int type, Tree *newTree)
{
    assert(newTree);

    Node *mainNode = NULL;
    constructNode(&mainNode);
    mainNode->myTree = newTree;

    mainNode->type = type;
    return mainNode;
}


Node *createNumericalNode(int type, double value, Tree *newTree)
{
    assert(newTree);

    Node *mainNode = {0};
    constructNode(&mainNode);
    mainNode->type = type;

    mainNode->value = value;
    return mainNode;
}


int dumpRecNode(const Node *n, FILE *outFile)
{
    assert(n);
    assert(outFile);

    fprintf(outFile,
            "Node%p [shape = record,  color = orange, label = \" {{ parent | '%p' } | { self | %p } "
            "| { content | %s } | { type |  '%d' }| { value | '%lg' } | { {right | %p} | { left | %p }} }\"]",
            n,
            n->parent,
            n,
            n->content, n->type, n->value, n->right, n->left);

    if (n->left)
        dumpRecNode(n->left, outFile);
    if (n->right)
        dumpRecNode(n->right, outFile);
    if (n->parent)
        fprintf(outFile, "\nNode%p -> Node%p\n", n->parent, n);
    return 0;
}


int printRecNode(const Node *n, FILE *outFile)
{
    assert(n);
    assert(outFile);

    fprintf(outFile,
            "Node%p [shape = record,  color = blue, label = \"{ { content | %s } | { type | '%d' }| { value | '%lg' }}\"]",
            n,
            n->content, n->type, n->value);

    if (n->left)
        printRecNode(n->left, outFile);
    if (n->right)
        printRecNode(n->right, outFile);
    if (n->parent)
        fprintf(outFile, "\nNode%p -> Node%p\n", n->parent, n);
    return 0;
}


int printTree(const Tree *tree, const char *outputFileName, int (*func)(const Node *node, FILE *out))
{
    FILE *outputFile = fopen(outputFileName, "w");
    fprintf(outputFile, "digraph dump\n" "{\n");

    func(tree->root, outputFile);

    fprintf(outputFile, "}");
    fclose(outputFile);

    return 0;
}


int compareNodes(const Node *firstNode, const Node *secondNode)
{
    assert(firstNode);
    assert(secondNode);

    if (firstNode->type != secondNode->type ||
        firstNode->value != secondNode->value)
        return 0;

    if (firstNode->content && secondNode->content)
        if (strcmp(firstNode->content, secondNode->content) != 0)
            return 0;
        else
            return 1;

    if (!firstNode->content && !secondNode->content)
        return 1;
    return 0;
}


int compareTrees(Node *firstRoot, Node *secondRoot)
{
    assert(firstRoot);
    assert(secondRoot);

    int res = compareNodes(firstRoot, secondRoot);

    if (!res)
        return 0;

    if (firstRoot->left)
    {
        if (!secondRoot->left)
            return 0;

        res = compareTrees(firstRoot->left, secondRoot->left);
        if (!res)
            return 0;
    }
    else if (secondRoot->left)
        return 0;

    if (firstRoot->right)
    {
        if (!secondRoot->right)
            return 0;

        res = compareTrees(firstRoot->right, secondRoot->right);
        if (!res)
            return 0;
    }
    else if (secondRoot->right)
        return 0;

    return 1;
}


#undef PRINTOUT_GLOBAL