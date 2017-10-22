/*************************************************************************************
    Header file for binary search tree (BST) ADT. Contains structural definitions and
    protoypes for BST.
        Written by: Iain S. Davis
        Date:       23 Apr 2013

		
**************************************************************************************/
#include "queue_ADT.h"


//Structure Declarations
enum destConst {PRESERVE = 0, NO = 0, FALSE = 0, DESTROY = 1, YES = 1, TRUE = 1};
enum balanceFactor{RH = -1, EH = 0, LH = 1};

typedef struct tree_node
{
    void*					dataPtr;
    struct tree_node*		left;
    struct tree_node*		right;
	enum   balanceFactor	bal;
}TREE_NODE;

typedef struct
{
    int count;
    int  allowDup;
    int  (*compare) (void *arg1, void *arg2);
    void (*freeData)(void *arg1);
    void *(*getNew)(void);
    TREE_NODE *root;
	QUEUE *searchResults;
} TREE;

//Prototype Declarations
TREE    *CreateTree         (int (*compare)(void  *argu1, void *argu2),
                                void (*freeData)(void *arg1),
                                void *(*getNew)(void));

TREE    *DestroyTree        (TREE *tree, enum destConst destroyData);

void    *InsertNew          (TREE *tree);
int     Insert              (TREE *tree, void *dataPtr);

int		Search				(TREE *tree, void *target);
void	*GetNextResult		(TREE *tree);
void	FlushSearch			(TREE *tree);

void    Traverse            (TREE *tree, void (*process)(void* dataPtr));

int     Filter              (TREE *tree, int (*compare)(void *arg1, void *arg2),
                                void (*process)(void *dataPtr), void *filter);


int      Delete            (TREE *tree, void *dltKey, int confirm(void *dataPtr), enum destConst destroy,
								void **dataOut);
int      DeleteAt          (TREE *tree, void *dltKey, enum destConst destroy);



int     PrintNested         (TREE *tree, void (*print)(void *dataPtr), int showNums);
int     EmptyTree           (TREE *tree);
int     FullTree            (TREE *tree);
int     TreeCount           (TREE *tree);

void	allowDup			(TREE *tree, int value);

void*   GetFirst			(TREE *tree);
void*   GetLast		        (TREE *tree);





