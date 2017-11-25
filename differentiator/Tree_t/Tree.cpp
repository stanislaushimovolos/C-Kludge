//
// Created by Tom on 22.11.2017.
//
#include <iostream>
#include <assert.h>
#include <cstring>

#include "../Diff/Diff.h"
#include "Tree.h"

void connectRight (Node *mainNode, Node *rightNode) {

	assert (rightNode);
	assert (mainNode);

	mainNode->Right = rightNode;
	rightNode->myTree = mainNode->myTree;
	rightNode->Parent = mainNode;
	mainNode->myTree->nodeAmount++;
}


void connectLeft (Node *mainNode, Node *leftNode) {

	assert (leftNode);
	assert (mainNode);

	mainNode->Left = leftNode;
	leftNode->myTree = mainNode->myTree;
	leftNode->Parent = mainNode;
	mainNode->myTree->nodeAmount++;
}

#define RETURN_COND(cond)           \
                                    \
if (cond)                           \
    return 0;                       \
return 1;                           \


int nodeConstruct (Node **node) {

	Node *elem = (Node *) calloc (1, sizeof (Node));

	elem->Left = NULL;
	elem->Right = NULL;
	elem->myTree = NULL;
	elem->Parent = NULL;
	elem->content = NULL;
	elem->type = 0;
	*node = elem;

	RETURN_COND (*node)


}


int nodeSetName (Node *node, char *content) {
	node->content = (char *) calloc (strlen (content) + 1, sizeof (char));
	strcpy ((char *) node->content, content);

	RETURN_COND (node->content)
}

int treeConstruct (Tree *tree) {

	assert (tree);

	nodeConstruct (&(tree->root));
	tree->root->myTree = tree;
	tree->nodeAmount = 1;
	tree->root->Parent = NULL;

	RETURN_COND (tree->root)
}


void destructNode (Node *node) {
	free (node->content);
	free (node);
}


void destructTree (Tree *tree) {
	if (tree->root) {
		destructTreeRec (tree->root);
		(tree->root) = NULL;
	}
}


void destructTreeRec (Node *node) {

	if (node->Left)
		destructNode (node->Left);

	if (node->Right)
		destructNode (node->Left);

	destructNode (node);
}


bool TargetFlag = 0;

Node *TreeSearcher (const Tree *const tree, char *target) {

	TargetFlag = 0;

	return TreeSearcherRec (tree->root, &target);
}


Node *TreeSearcherRec (Node *node, char **target) {

	static Node *Helper = {};

	if (strcmp (*target, (char *) node->content) == 0) {
		TargetFlag = 1;
		return node;
	}

	if (node->Right) {
		Helper = TreeSearcherRec (node->Right, target);
		if (TargetFlag) {
			return Helper;
		}
	}

	if (node->Left) {
		Helper = TreeSearcherRec (node->Left, target);
		if (TargetFlag) {
			return Helper;
		}
	}

	return 0;
}


int readBase (char **retBuffer, const char *_inputFilename) {

	assert (retBuffer);
	size_t sizeOfBuf = 0;

	char *buffer = getBufferFromFileGetSzOfBuf (_inputFilename, &sizeOfBuf);
	char *secBuf = (char *) calloc (sizeOfBuf, sizeof (char));
	int counterMain = 0;
	int counterPilot = 0;

	while (buffer[counterMain]) {
		if (buffer[counterMain] == '(' ||
		    buffer[counterMain] == ')' ||
		    buffer[counterMain] == SEPARATOR) {

			if (buffer[counterMain] == SEPARATOR) {
				secBuf[counterPilot++] = buffer[counterMain++];
				while ((buffer[counterMain] != SEPARATOR)) {
					secBuf[counterPilot++] = buffer[counterMain++];
				}

				secBuf[counterPilot++] = buffer[counterMain++];
				continue;
			}

			secBuf[counterPilot++] = buffer[counterMain];
			counterMain++;
			continue;
		}

		counterMain++;
	}

	free (buffer);

	*retBuffer = secBuf;

	RETURN_COND (*buffer)
}


#undef RETURN_COND


int createBase (char *base, Node *node) {

	assert (node);

	base += 2;

	char *separatorPtr = 0;
	size_t lengthOfContent = 0;

	separatorPtr = strchr (base, SEPARATOR);

	lengthOfContent = (size_t) (separatorPtr - base);

	node->content = (char *) calloc (lengthOfContent + 1, sizeof (char));

	memcpy (node->content, base, lengthOfContent);

	contentAnalyze (node);

	base += lengthOfContent + 1;

	createBaseRec (node, &base);

}


#define RETURN_CONDITION    \
    if ((*base)[0] == ')') {\
    (*base) += 1;           \
                            \
    if ((*base)[0] == '(')  \
        return 1;           \
    return 0;               \
}


#define NODE_ADDING                                                             \
    *base += 2;                                                                 \
                                                                                \
    nodeConstruct (&nodeHelp);                                                  \
                                                                                \
    separatorPtr = strchr (*base, SEPARATOR);                                   \
                                                                                \
    lengthOfContent = (size_t) (separatorPtr - *base);                          \
                                                                                \
    nodeHelp->content = (char *) calloc (lengthOfContent + 1, sizeof (char));   \
                                                                                \
    memcpy (nodeHelp->content, *base, lengthOfContent);                         \
                                                                                \
    *base += lengthOfContent + 1;                                               \


int createBaseRec (Node *node, char **base) {

	assert (node);

	static char *separatorPtr = 0;
	static size_t lengthOfContent = 0;
	static int rightSideChecker = 0;

	Node *nodeHelp;

	RETURN_CONDITION

	NODE_ADDING

	contentAnalyze (nodeHelp);

	connectLeft (node, nodeHelp);

	rightSideChecker = createBaseRec (nodeHelp, base);


	if (rightSideChecker == 1) {

		NODE_ADDING

		connectRight (node, nodeHelp);

		rightSideChecker = createBaseRec (nodeHelp, base);

	}
	RETURN_CONDITION

}


#undef RETURN_CONDITION

#undef NODE_ADDING

#define PRINTOUT_REC(function)                                                         \
    if (n->Left != NULL)                                                               \
        function (n->Left, NodeCounter, outPictureFile1);                              \
    if (n->Right != NULL)                                                              \
       function (n->Right, NodeCounter, outPictureFile1);                              \
    if (n->Parent != NULL)                                                             \
        fprintf (outPictureFile1, "\nNode%p -> Node%p\n", n->Parent, n);


#define CYCLE_CHEACK                                                            \
    static int CycleChecker;                                                    \
    if (n->myTree->nodeAmount == (*NodeCounter - 1) || CycleChecker == 1) {     \
        CycleChecker = 1;                                                       \
    return CycleChecker;                                                        \
    } else                                                                      \
    CycleChecker = 0;


int dumpRecNode (const Node *const n, int *NodeCounter, FILE *outPictureFile1) {
	fprintf (outPictureFile1,
	         "Node%p [shape = record,  color = red, label = \"{ { %s| '%p' } | { Parent| '%p' }  | { type | '%d' } | ",
	         n, (char *) n->content, n, n->Parent, n->type);

	fprintf (outPictureFile1, "{ Left = %p ", n->Left);
	fprintf (outPictureFile1, "| Right = %p } }\"]\n", n->Right);

	(*NodeCounter)++;

	CYCLE_CHEACK

	PRINTOUT_REC(dumpRecNode)

	return CycleChecker;
}


int printRecNode (const Node *const n, int *NodeCounter, FILE *outPictureFile1) {

	if (n)

		fprintf (outPictureFile1, "Node%p [shape = record,  color = blue, label = \" { %s }\"] ",
		         n, (char *) n->content);
	(*NodeCounter)++;

	CYCLE_CHEACK

	PRINTOUT_REC(printRecNode);

	return CycleChecker;
}


#undef CYCLE_CHEACK


int printTreeFile (const Tree *const tree, const char *outFileName) {
	FILE *outBaseFile = fopen (outFileName, "w");
	int NodeCounterRec = 0;
	printTreeFileRec (tree->root, &(tree->nodeAmount), &NodeCounterRec, outBaseFile);
}

int printTreeFileRec (const Node *const Node, const int *const NodeAmount, int *NodeCounterRec, FILE *outBaseFile1) {

	static int tabsAmount = -1;
	static int CycleChecker;

	if (*NodeAmount == (*NodeCounterRec) || CycleChecker == 1) {
		CycleChecker = 1;
		return CycleChecker;
	} else
		CycleChecker = 0;

	if (Node) {
		(*NodeCounterRec)++;
		tabsAmount++;
		for (int i = 0; i < tabsAmount; i++)
			fprintf (outBaseFile1, "\t");

		fprintf (outBaseFile1, "(%c%s%c\n", SEPARATOR, (char *) Node->content, SEPARATOR);
		printTreeFileRec (Node->Left, NodeAmount, NodeCounterRec, outBaseFile1);
		printTreeFileRec (Node->Right, NodeAmount, NodeCounterRec, outBaseFile1);

		for (int i = 0; i < tabsAmount; i++)
			fprintf (outBaseFile1, "\t");
		fprintf (outBaseFile1, ")\n");
		tabsAmount--;
	}

	return CycleChecker;
}


#undef PRINTOUT_REC


#define PRINTOUT_GLOBAL(function)                                           \
    assert (tree->root);                                                    \
                                                                            \
                                                                            \
    int NodeCounter = 0;                                                    \
                                                                            \
    fprintf (outPictureFile, "digraph dump\n" "{\n");                       \
                                                                            \
    NodeCounter= function (tree->root, &NodeCounter, outPictureFile) ;      \
                                                                            \
    fprintf (outPictureFile, "}");                                          \
                                                                            \
    fclose (outPictureFile);                                                \



int dumpTreePicture (const Tree *const tree, const char *outFileName) {

	FILE *outPictureFile = fopen (outFileName, "w");

	PRINTOUT_GLOBAL (dumpRecNode)

	system ("C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\dot\\bin\\dot -Tpng "
			        "C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\picture\\dotDump.txt -o "
			        "C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\picture\\Dump.png");

	return NodeCounter;

}

int printTree (const Tree *const tree, const char *outFileName) {


	FILE *outPictureFile = fopen (outFileName, "w");

	PRINTOUT_GLOBAL (printRecNode)

	system ("C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\dot\\bin\\dot -Tpng "
			        "C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\picture\\dotPrint.txt -o "
			        "C:\\Users\\Tom\\CLionProjects\\CPU\\differentiator\\picture\\print.png");;

	return NodeCounter;

}

#undef PRINTOUT_GLOBAL

NodeWay *createArrOfParents (const Tree *const tree, Node *CurrentNode, int *NodeCounter) {


	NodeWay *parentArr = (NodeWay *) calloc (sizeof (NodeWay), (size_t) tree->nodeAmount);

	while (CurrentNode != tree->root) {

		(parentArr[*NodeCounter]).node = CurrentNode;

		if (CurrentNode == CurrentNode->Parent->Right)
			(parentArr[*NodeCounter + 1]).state = 1;

		else
			(parentArr[*NodeCounter + 1]).state = 0;

		CurrentNode = CurrentNode->Parent;

		(*NodeCounter)++;

	}
	if (CurrentNode == tree->root)
		(parentArr[*NodeCounter]).node = CurrentNode;

	return parentArr;
}