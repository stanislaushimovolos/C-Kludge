//
// Created by Tom on 12.11.2017.
//
#pragma once

#include <stdio.h>

struct Node;

typedef struct
{
    struct Node *root;
    int eventFlag;
    int nodeAmount;
} Tree;

typedef struct Node
{
    Tree *myTree;
    char *content;
    double value;

    struct Node *left;
    struct Node *right;
    struct Node *parent;
    int type;


} Node;

int countNodes(Node *node);

int constructNode(Node **node);

int treeConstruct(Tree *tree);

int dumpRecNode(const Node *n, FILE *outFile);

int printRecNode(const Node *n, FILE *outFile);

int compareTrees(Node *firstRoot, Node *secondRoot);

int compareNodes(const Node *firstNode, const Node *secondNode);

int printTree(const Tree *tree,
              const char *outputFileName, int (*func)(const Node *node, FILE *out));


void destructNode(Node *node);

void destructTree(Tree *tree);

void destructNodeRec(Node *node);

void visitTreePost(Node *node, int func(Node *node));

void visitTreePref(Node *node, int func(Node *node));

void connectLeft(Node *mainNode, Node *leftNode);

void connectRight(Node *mainNode, Node *rightNode);

Node *copyTree(const Node *node, Tree *newTree);

Node *createSimpleNode(int type, Tree *newTree);

Node *createNumericalNode(int type, double value, Tree *newTree);

Node *createLiteralNode(int type, const char *content, Tree *newTree);


