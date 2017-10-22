/***********************************************************************************************************
		The functions here cover all the AVL Tree operations, and in some cases incorporate processes from 
		the QUEUE ADT as well. I've made the assumption that each tree will support only one kind of data.
		I've also made modifications to the given AVL ADT to cover duplicate data. These changes primarily
		affect the Insert, Delete, and Search (Retrieve) functions, and require the inclusion of the 
		queue ADT.

		NOTE: some of these functions have gone through several changes in the process of development. The
		latest version of Retrieve (Search/GetNextResult) makes much of the current overly-complex delete
		function redundant, but in the interest of not introducing problems so close to delivery, I've left
		the cumbersome (but functional) Delete function in place.
			-CreateTree - builds a tree with the default data members as well as:
				-a pointer to a function for comparing the key the tree is sorted on
				-a pointer to a default freeData function (in case the stored data has dynamically allocated members)
				-a pointer to a default getNew function for inserting new data into the tree
				-a QUEUE of search results
				-an integer (boolean) flag to indicate whether the tree allows duplicate keys or not.

			-InsertNew
				-creates a new item of the tree's default type and inserts it into the tree
				-rebalances the tree as necessary

			-Insert
				-inserts provided data into the tree
				-rebalances the tree as necessary

			-Search/GetNextResult/FlushResults
				-Clears out any lingering searchResults from previous searches
				-These three functions replace the standard Retrieve function and deal with the difficulties
					of searching for data in a tree that may have multiple matches
				-If the tree doesn't allow duplicates, Search stores the first in the tree's searchResults queue and exits
				-If the tree allows duplicates, when search finds a match, it searches the left subtree adding
					any matches it finds to the searchResult queue, then adds the root to the queue, then
					searches the right subtree adding any matches to the queue.
				-Returns the count of located items
			-GetNextResult
				-dequeues an item from the tree's searchResult queueu and returns it to the caller
				-if underflow, returns 0
			-FlushResults
				-cleans any remaining searchResults out of a given tree's searchResults queue
			
			-Delete
				-locates a node containing data matching a provided target value
				-calls a confirmation function to determine if it is the correct node to delete
				-deletes the node, or continues to search, first left subtree, then right subtree
				-when a node is deleted, it's dataPtr is immediately returned back up the call stack to the original caller
				-if no confirmation function is provided, or if the tree does not allow duplicates, the first
					located match is deleted automatically, and a pointer to its data returned
				-if no matches are found, 0 is returned
			-DeleteAt
				-behaves similarly to delete, but on locating a match, it also compares the address against
					the target data.
				-if searchCriteria and address match, the node is deleted. Success is returned.
				-if searchCriteria match, but address does not, search left subtree, then right subtree.
				-if no matches found, return failure (0)

			-Filter
				-Traverse with the addition of an extra compare function that limits which items will be processed

			-PrintNested
				-prints the contents of the tree in nested format (diagnostic)

			-GetFirst
				-walks the left edge of the tree and returns data stored in the left-most descendant of the root

			-GetLast
				-same as GetFirst on the right edge.

			-DestroyTree
				-similar to the destroy function in the books AVL ADT implementation
				-with the addition of first flushing the searchResults queue, then freeing the queue head
************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "AVL_ADT.h"

static int		_insert         (TREE_NODE **root, TREE_NODE *newPtr, int (*compare)(void *arg1, void *arg2),
									int allowDup, int *taller);

static void		insLeftBal		(TREE_NODE **root, int *taller);
static void		insRightBal		(TREE_NODE **root, int *taller);

static void		*_delete        (TREE_NODE **root, void *dataPtr, int (*compare)(void *arg1, void *arg2),
									void (*freeData)(void *arg1), int confirm(void *dataPtr),
									enum destConst destroyData, int atAddress, int *shorter);

static void		*_deleteDup        (TREE_NODE **root, void *dataPtr, int (*compare)(void *arg1, void *arg2),
									void (*freeData)(void *arg1), int confirm(void *dataPtr),
									enum destConst destroyData, int atAddress, int *shorter);

static void		dltRightBal		(TREE_NODE **root, int *shorter);
static void		dltLeftBal		(TREE_NODE **root, int *shorter);


static void		rotateRight		(TREE_NODE **root);
static void		rotateLeft		(TREE_NODE **root);


static int		_retrieve		(TREE_NODE *root, void *target, TREE *tree);

static int		_retrieveDup	(TREE_NODE *root, void *target, TREE *tree);

static void		_traverse       (TREE_NODE *root, void (*process)(void *dataPtr));


static void		_printNested    (TREE_NODE *root, void (*print)(void *dataPtr), int showNums);


static int		_filter         (TREE_NODE *root, int (*compare)(void *arg1, void *arg2),
									void (*process)(void *arg1), void *target, int count);


static void		_destroy        (TREE_NODE *root, enum destConst destroyData,
									void (*freeData)(void *dataPtr));


/****** CreateTree *******************************************************************
    Allocates dynamic memory for an AVL tree head node, sets default and user-supplied
	values, and returns its address to caller.
        PRE     compare is address of compare function used when two nodes need to be
                compared
				freeData is type-specific function to free memory. If application type
					does not have externally allocated members, this can just be the 
					standard free fuction
				getNew is a pointer to a type-specific function to retrieve data members
				for a new tree member
        POST    head allocated or error returned
        RETURN  head node pointer; null if overflow
**************************************************************************************/
TREE *CreateTree(int (*compare)(void  *argu1, void *argu2),
                     void (*freeData)(void *arg1),
                     void *(*getNew)(void)) 
{
//Local Declarations
    TREE *tree;

//Statements
    tree = (TREE*) malloc(sizeof(TREE));
    if (tree)
    {
        tree->root = NULL;
  		tree->count = 0;
        tree->compare = compare;
        tree->allowDup = 1;
        tree->freeData = freeData;
        tree->getNew  = getNew;
		tree->searchResults = createQueue();
    }//if

    return tree;
}//BST_Create


/****** DestroyTree **************************************************************************
    Deletes all data in tree and recycles memory. The nodes are deleted by calling a recursive
    function to traverse the tree in inorder sequence.
        Pre     tree is a pointer to a valid tree
				destroy is enumeration with two possible values: DESTROY (1) or PRESERVE(0)
					indicating whether stored data is to be destroyed along with the nodes in
					the tree
        Post    All nodes and head structure deleted - if DESTROY, data recycled as well
        Return  null head pointer
**********************************************************************************************/
TREE *DestroyTree(TREE *tree, enum destConst destroyData)
{
//Statements
    if(tree)  {
		flushQueue(tree->searchResults);
		free(tree->searchResults);
        _destroy(tree->root, destroyData, tree->freeData);
	}

    //All nodes deleted. Free structure
	
    free(tree);
    return NULL;
}//DestroyTree


/****** InsertNew *********************************************************************
    This function calls the tree's getNew function, wraps the created data in a TREE_NODE
	and inserts it into the	tree.
        Pre     tree is pointer to an AVL tree structure with a getNew function member
        POST    data inserted or memory overflow
        RETURN  pointer to newly inserted node -or- NULL
***************************************************************************************/
void  *InsertNew (TREE *tree)
{
    //Local Declarations
    void *dataPtr;
    TREE_NODE *newPtr;
	int taller;

//Statements
    dataPtr = tree->getNew();
    newPtr = (TREE_NODE*) malloc(sizeof(TREE_NODE));
    if(!newPtr)
        return 0;

    newPtr->right = NULL;
    newPtr->left = NULL;
    newPtr->dataPtr = dataPtr;

    if(_insert(&tree->root, newPtr, tree->compare, tree->allowDup, &taller))  {
        (tree->count)++;
        return dataPtr;
    }
    else  {
        return NULL;
    }
}//BST_InsertNew


/****** Insert ************************************************************************
    This function wraps past data in a TREE_NODE and inserts it into the tree.
        Pre     tree is pointer to AVL tree structure
				dataPtr is void pointer containing address of data to be inserted
        POST    data inserted or memory overflow
        RETURN success (true) or overflow (false)
***************************************************************************************/
int Insert(TREE *tree, void *dataPtr)
{
//Local Declarations
    TREE_NODE *newPtr;
	int result = 0;
	int taller;

//Statements
    newPtr = (TREE_NODE*) malloc(sizeof(TREE_NODE));
    if(!newPtr)
        return 0;

    newPtr->right = NULL;
    newPtr->left = NULL;
    newPtr->dataPtr = dataPtr;
	newPtr->bal = EH;

    if(tree->count == 0)  {
        tree->root = newPtr;
		result = 1;
	}
    else
        result =  _insert(&tree->root, newPtr, tree->compare, tree->allowDup, &taller);

	if(result)
	{
		(tree->count)++;
	}

    return result;
}//BST_Insert


/****** Search ************************************************************************
	Search navigates a tree and locates all nodes matching the search criteria and
	storing them in a queue for later retrieval by the GetNextResult function
        PRE     tree has been created (may be null)
                target is pointer to data structure containing the key to be located
        POST    Tree searched and queue populated
        RETURN  number of matching items located and enqueued.
***************************************************************************************/
int Search(TREE *tree, void *target)
{
//Statements
	if(tree && tree->root)  {
		flushQueue(tree->searchResults);
		if (tree->allowDup)  {
			_retrieveDup(tree->root, target, tree);
		}
		else  {
			_retrieve(tree->root, target, tree);
		}
	}

	return queueCount(tree->searchResults);
}//Search


/****** GetNextResult *****************************************************************
	function returns a pointer to the next data stored in tree->searchResults, said data
	having been previously located and enqueued by a call to the Search function
***************************************************************************************/
void *GetNextResult (TREE *tree)
{
//Local Declarations
	void *itemOut;
//Statements
	if (tree && queueCount(tree->searchResults))
	{
		dequeue(tree->searchResults, &itemOut);
		return itemOut;
	}//if tree exists and searchResult queue is not empty

	return NULL;
}//GetNextResult


/****** FlushSearch *******************************************************************
	Function clears out remaining search results in a tree
		PRE		tree is a pointer to a valid TREE structure
		POST	flush all search results stored in tree->searchResults (queue)
***************************************************************************************/
void FlushSearch(TREE *tree)
{
//Statements
	flushQueue(tree->searchResults);

	return;
}//FlushSearch


/****** Traverse **********************************************************************
    Process tree using inorder traversal.
        PRE     Tree has been created (may be null)
                process "visits" nodes during traversal
        POST    Nodes processed in LNR (inorder) sequence
***************************************************************************************/
void Traverse (TREE *tree, void (*process)(void *dataPtr))
{
//Statements
        _traverse (tree->root, process);
        return;
}//Traverse


/****** _Filter *******************************************************************************
    Interface function to print all nodes matching a user-provided criteria
        PRE     tree is a valid BST (may be empty)
                print is a pointer to a type-specific print function (application)
        POST    tree printed in hierarchical format
        RETURN  failure (0) if empty or null tree
                success (1) otherwise
**********************************************************************************************/
int Filter  (TREE *tree,
             int      (*compare)(void *arg1, void *arg2),
             void     (*process)(void *dataPtr),
             void     *filter)
{
//Statements
    if(!tree || !tree->count)
        return 0;
      
    return _filter(tree->root, compare, process, filter, 0);
}//Filter


/****** Delete *************************************************************************
    This function deletes a node from the tree and rebalances it if necessary
        PRE     tree initialized -- null tree is OK
                dltKey is pointer to data structure containing key to be deleted
        POST    node deleted and its space recycled -or- an error code is returned
        RETURN  success (true) or Not found (false)
****************************************************************************************/
int Delete (TREE *tree, void *dltKey, int confirm(void *dataPtr), enum destConst destroy, void **dataOut)
{
//Local Declarations
	int  shorter = 0;

//Statements
	if(!tree || !tree->count)  {
		*dataOut = NULL;
		return 0;
	}
	if (tree->allowDup)  {
		if((*dataOut = _deleteDup(&tree->root, dltKey, tree->compare, tree->freeData, confirm, destroy, 0, &shorter)))  {
			(tree->count)--;
			return 1;
		}
	}
	else  {
		if((*dataOut = _delete(&tree->root, dltKey, tree->compare, tree->freeData, confirm, destroy, 0, &shorter)))  {
			(tree->count)--;
			return 1;
		}
	}
    
    *dataOut = NULL;
    return 0;
}//Delete


/****** DeleteAt ***********************************************************************
    This function deletes a node with a specific address from the tree and rebalances it
    if necessary
        PRE     tree initialized -- null tree is OK
                dltKey is pointer to data structure containing key to be deleted
        POST    node deleted and its space recycled -or- an error code is returned
        RETURN  success (true) or Not found (false)
****************************************************************************************/
int DeleteAt (TREE *tree, void *dltKey, enum destConst destroy)
{
//Local Declarations
   // void *dataPtr;    CHANGE
	int  shorter = 0;

//Statements
	if(!tree || !tree->count)  {
		return 0;
	}
	if(tree->allowDup)  {
		if((_deleteDup(&tree->root, dltKey, tree->compare, tree->freeData, NULL, destroy, 1, &shorter)))  {
			(tree->count)--;
			return 1;
		}
	}
	else  {
			if((_delete(&tree->root, dltKey, tree->compare, tree->freeData, NULL, destroy, 1, &shorter)))  {
			(tree->count)--;
			return 1;
		}
	}

    return 0;
}//DeleteAt


/****** PrintNested **************************************************************************
    print a tree's contents in nested order (RLN)
        PRE     tree is a valid BST (may be empty)
                print is a pointer to a type-specific print function (application)
        POST    tree printed in hierarchical format
        RETURN  failure (0) if empty or null tree
                success (1) otherwise
**********************************************************************************************/
int PrintNested (TREE *tree, void (*print)(void *dataPtr), int showNums)
{
//Statements
    if(!tree || !tree->count)
        return 0;

    _printNested(tree->root, print, showNums);

    return 1;
}//PrintNested


/****** EmptyTree *********************************************************************
    Returns true if tree is empty; false if any data.
        PRE     Tree has been created. (may be null)
        RETURNS true if tree empty, false if any data
***************************************************************************************/
int EmptyTree (TREE *tree)
{
//Statements
    return (tree->count == 0);
}//EmptyTree


/****** FullTree **********************************************************************
    If there is no room for another node, returns true
        PRE     tree has been created
        RETURNS true if no room for another insert
                false if room
***************************************************************************************/
int FullTree (TREE *tree)
{
//Local Declarations
    TREE_NODE *newPtr;

//Statements
    newPtr = (TREE_NODE*) malloc(sizeof(*(tree->root)));
    if(newPtr)
    {
        free(newPtr);
        return 0;
    }
    else
        return 1;
}//FullTree


/****** TreeCount ****************************************************************************
    Returns number of nodes in tree.
        PRE     tree has been created
        RETURNS tree count
**********************************************************************************************/
int TreeCount (TREE *tree)
{
//Statements
    return (tree->count);
}//TreeCount


/****** allowDup *************************************************************************
    toggle the allowDuplicates option off or on for a tree
        PRE     tree is a pointer to a valid tree
        POST    tree->allowDup set to true (nonzero) or false(zero)
******************************************************************************************/
void allowDup(TREE *tree, int value)
{
//Statements
    tree->allowDup = value;
    return;
}//allowDup


/****** _insert ***********************************************************************
    This function uses recursion to insert the new data into a leaf node in the BST tree
        PRE     Application has called BST_Insert, which passes root, data pointer,
                    compare function, and value of "allowDuplicates" property (0 or nonzero).
        POST    Data have been inserted
        RETURN  pointer to [potentially] new root
***************************************************************************************/
static int _insert(TREE_NODE **root, TREE_NODE *newPtr, int (*compare)(void *arg1, void *arg2),
						int allowDup, int *taller)
{
//Local Declarations
	int result;

//Statements
    if(!*root) {
        *root = newPtr;
		*taller = 1;
        return 1;
    }

    //Locate null subtree for insertion
	if (compare(newPtr->dataPtr, (*root)->dataPtr) < 0)  {
		//newData < root -- go left
        result = _insert(&(*root)->left, newPtr, compare, allowDup, taller);
		if(*taller)  {
			//left subtree is taller
			switch((*root)->bal)
			{
			case LH:	//was left high -- rotate
						insLeftBal(root, taller); //insLeftBal determines single or double rotation
						break;
			case EH:	//was EH -- now LH
						(*root)->bal = LH;
						break;
			case RH:	//was RH -- now EH
						(*root)->bal = EH;
						*taller = 0;
						break;
			}//switch
			//return result;
		}
		return result;
    }
    else if (compare(newPtr->dataPtr, (*root)->dataPtr) > 0)  {
		//newData > rootData
        result = _insert(&(*root)->right, newPtr, compare, allowDup, taller);
		if (*taller)  {
		//	right subtree is taller
			switch ((*root)->bal)
			{
			case LH:	//Was LH -- now EH
						(*root)->bal = EH;
						*taller = 0;
						break;
			case EH:	//Was balanced -- now RH
						(*root)->bal = RH;
						break;
			case RH:	//Was RH -- now out of balance
						insRightBal(root, taller);
						break;
			}
		}
		return result;
    }
    else if(allowDup)  {
		//newData == root data AND duplicates allowed -- insert to the right
        result = _insert(&(*root)->right, newPtr, compare, allowDup, taller);
		if (*taller)  {
			//right subtree is taller
			switch ((*root)->bal)
			{
			case LH:	//Was LH -- now EH
						(*root)->bal = EH;
						*taller = 0;
						break;
			case EH:	//Was balanced -- now RH
						(*root)->bal = RH;
						break;
			case RH:	//Was RH -- now out of balance
						insRightBal(root, taller);
						break;
			}	
			return result;
		}
		return result;
    }
   return 0;
}//_insert


/****** insLeftBal *******************************************************************
	Tree out-of-balance on the left. This function rotates the tree to the right.
		PRE		The tree is left high
		POST	Balance restored; return potentially new root
**************************************************************************************/
static void insLeftBal(TREE_NODE **root, int *taller)
{
//Local Declarations
	TREE_NODE *rightTree;
	TREE_NODE *leftTree;

//Statements
	leftTree = (*root)->left;
	switch(leftTree->bal)
	{
	case LH:	//Left High - Rotate Right
				(*root)->bal = EH;
				leftTree->bal = EH;
				rotateRight(root);
				*taller = 0;
				break;
	case EH:	//This is an error - 
				//function should never be called unless left branch of root
				//is left-high or right high
				printf("\n\aError in insLeftBal\n");
				exit(100);
	case RH:	//Right high: requires doulbe rotation: first left, then right
				rightTree = leftTree->right;
				switch(rightTree->bal)
				{
				case LH:	(*root)->bal = RH;
							leftTree->bal = EH;
							break;
				case EH:	(*root)->bal = EH;
							leftTree->bal = EH;
							break;
				case RH:	(*root)->bal = EH;
							leftTree->bal = LH;
							break;
				}
				rightTree->bal = EH;
				//Rotate Left
				rotateLeft(&(*root)->left);

				//Rotate Right
				rotateRight(root);
				*taller = 0;
	}
	return;
}//insLeftBal


/****** insRightBal *****************************************************************
		Tree out-of-balance on the right. This function rotates the tree to the left.
		PRE		The tree is right high
		POST	Balance restored; return potentially new root
**************************************************************************************/
static void insRightBal(TREE_NODE **root, int *taller)
{
//Local Declarations
	TREE_NODE *leftTree;
	TREE_NODE *rightTree;

//Statements
	rightTree = (*root)->right;

	switch(rightTree->bal)
	{
	case LH:	//Left high: Double rotation required
				leftTree = rightTree->left;
				switch(leftTree->bal)
				{
				case LH:	(*root)->bal = EH;
							rightTree->bal = RH;
							break;
				case EH:	(*root)->bal = EH;
							rightTree->bal = EH;
							break;
				case RH:	(*root)->bal = LH;
							rightTree->bal = EH;
							break;
				}//switch
				leftTree->bal = EH;
				//Rotate right
				rotateRight(&(*root)->right);
				//Rotate left
				rotateLeft (root);
				*taller = 0;
				break;
	case EH:	//This is an error... this function should never be called 
				//if root is even high
				printf("\n\aError in insRightBal\n");
				exit(100);
				break;
	case RH:	//Right High - rotate left
				(*root)->bal = EH;
				rightTree->bal = EH;
				rotateLeft(root);
				*taller = 0;
				break;
	}

	return;
}//insertRightBal


/****** _delete ***************************************************************************
    FUNCTION SPECIFIC TO DUPLICATE-REFUSING AVL TREES
	Function locates a node matching the requested criteria. If node matching requested
	criteria not found, return NULL. If atAddress is true, compare the addresses of target
	data and root. If atAddress is NOT true, prompt the user for confirmation. If atAddress
	is false, or the user refuses confirmation, return NULL. If confirm function pointer
	is NULL, delete first located node without confirmation. AFTER DELETING NODE - if
	destroy = DESTROY(0) destroy the stored data using freeData. If	destroy = PRESERVE(1),
	do not destroy data. Return the address of the deleted data, regardless
	of whether the node was deleted or not. The public function Delete will prevent the
	freed data from being passed to the calling application.
        PRE     tree initialized -- null tree OK
                dataPtr contains key of node to be deleted
				compare is pointer to compare function used to navigate the tree
				freeData is pointer to type-specific function to free memory --
					if application data does not have external dynamically allocated members
					the standard function free() will suffice.
				confirm is a pointer to function to confirm deletion. If NULL, function will
					delete the first located matching data
				destConst is an enumeration with possible values DESTROY and PRESERVE
				atAddress is an integer (or boolean) flag to indicate whether the data to be
					deleted resides at a specific address, or is based only on search criteria
        POST    node is deleted and its space recycled -or- if key not found, tree is 
                unchanged
                success is true if deleted, false if not
        RETURN  pointer to root
*******************************************************************************************/
static void* _delete(TREE_NODE **root, void *dataPtr, int (*compare)(void *arg1, void *arg2),
                        void (*freeData)(void *arg1), int confirm(void *dataPtr),
						enum destConst destroy, int atAddress, int *shorter)
{
//Local Declarations
    TREE_NODE *dltPtr;
    TREE_NODE *exchPtr;
    void* holdPtr;
	void *result = NULL;

//Statements
    if(!*root)
    {
		*shorter = 0;
        return NULL;
    }//if

    if (compare (dataPtr, (*root)->dataPtr) < 0) {
        result = _delete(&(*root)->left, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter);
		if (*shorter)  {
			dltRightBal(root, shorter);
		}
		return result;
	}
    else if (compare(dataPtr, (*root)->dataPtr) > 0)  {
        result = _delete(&(*root)->right, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter);
		if(*shorter)  {
			dltLeftBal(root, shorter);
		}
		return result;
	}
    else
        //Node matching search criteria found

		//if address-specific search and address doesn't match...
        if(atAddress && (*root)->dataPtr != dataPtr)  { 
				//tree disallows duplicate keys -- target not in tree
				*shorter = 0;
				return 0;
        }
		//if searching by criteria only AND a confirmation function exists...
		if (confirm && !confirm((*root)->dataPtr))  { 
			// ...and current data is disconfirmed as the correct data to delete...
			// ...return failure -- tree disallows duplicate keys
			*shorter = 0;
			return 0;
		}
		// In all other cases, the data matches, and is confirmed as the correct
		// data to delete, or no confirmation is necessary...
		// ...proceed with delete


        // test for leaf node
        dltPtr = *root;
    if (!(*root)->left)
    //No left subtree
    {
        *root = (*root)->right;
		*shorter = 1;
        if(destroy)  {
            freeData(dltPtr->dataPtr);
            holdPtr = dltPtr->dataPtr;
        }
        else  {
            holdPtr = dltPtr->dataPtr;
        }
        free(dltPtr);
        return holdPtr;
    }
    else
        if (!(*root)->right)
        //Only left subtree
        {
            *root = (*root)->left;
			*shorter = 1;
            if(destroy)  {
                freeData(dltPtr->dataPtr);
                holdPtr = dltPtr->dataPtr;
            }
            else  {
                holdPtr = dltPtr->dataPtr;
            }
            free(dltPtr);
            return holdPtr;
        }
        else
        //delete node has two subtrees
        {
            exchPtr = (*root)->left;
            //Find largest node on left subtree
            while(exchPtr->right)
                exchPtr = exchPtr->right;

            //Exchange data
            holdPtr = (*root)->dataPtr;
            (*root)->dataPtr = exchPtr->dataPtr;
            exchPtr->dataPtr = holdPtr;
            result = _delete(&(*root)->left, exchPtr->dataPtr, compare, freeData, confirm, destroy, atAddress, shorter);
			if(*shorter)  {
				dltRightBal(root, shorter);
			}
			return result;
        }//else

}//_delete


/****** _deleteDup ************************************************************************
    FUNCTION SPECIFIC TO DUPLICATE-ALLOWING AVL TREES
	Function locates a node matching the requested criteria. If node matching requested
	criteria not found, return NULL. If atAddress is true, compare the addresses of target
	data and root. If atAddress is NOT true, prompt the user for confirmation. If atAddress
	is false, or the user refuses confirmation, recursively search left subtree, then right
	subtree for more matching data. If confirm function pointer is NULL, delete first
	located node without confirmation. AFTER DELETING NODE - if destroy = DESTROY(0) destroy
	the stored data using freeData. If	destroy = PRESERVE(1), do not destroy data. Return
	the address of the deleted data, regardless of whether the node was deleted or not. The
	public function Delete will prevent the freed data from being passed to the calling
	application.
        PRE     tree initialized -- null tree OK
                dataPtr contains key of node to be deleted
				compare is pointer to compare function used to navigate the tree
				freeData is pointer to type-specific function to free memory --
					if application data does not have external dynamically allocated members
					the standard function free() will suffice.
				confirm is a pointer to function to confirm deletion. If NULL, function will
					delete the first located matching data
				destConst is an enumeration with possible values DESTROY and PRESERVE
				atAddress is an integer (or boolean) flag to indicate whether the data to be
					deleted resides at a specific address, or is based only on search criteria
        POST    node is deleted and its space recycled -or- if key not found, tree is 
                unchanged
                success is true if deleted, false if not
        RETURN  pointer to root
****************************************************************************************/
static void* _deleteDup(TREE_NODE **root, void *dataPtr, int (*compare)(void *arg1, void *arg2),
                        void (*freeData)(void *arg1), int confirm(void *dataPtr),
						enum destConst destroy, int atAddress, int *shorter)
{
//Local Declarations
    TREE_NODE *dltPtr;
    TREE_NODE *exchPtr;
    void* holdPtr;
	void *result = NULL;

//Statements
    if(!*root)
    {
		*shorter = 0;
        return NULL;
    }//if

    if (compare (dataPtr, (*root)->dataPtr) < 0) {
        result = _deleteDup(&(*root)->left, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter);
		if (*shorter)  {
			dltRightBal(root, shorter);
		}
		return result;
	}
    else if (compare(dataPtr, (*root)->dataPtr) > 0)  {
        result = _deleteDup(&(*root)->right, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter);
		if(*shorter)  {
			dltLeftBal(root, shorter);
		}
		return result;
	}
    else
        //Node matching search criteria found

		//if address-specific search...
        if(atAddress)  { //NOTE: else should only function on !atAddress, not on atAddress && matched addresses
			if ((*root)->dataPtr != dataPtr)  { // ...and unmatching address
				if ((result = _deleteDup(&(*root)->left, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter)))  {
					if (*shorter)  {
					// ...if found and deleted on the left, rebalance the tree if necessary
						dltRightBal(root, shorter);
					}
				}
				// ...if not found on the left, check on the right...
				else if ((result = _deleteDup(&(*root)->right, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter)))  {
					if (*shorter)  {
						dltLeftBal(root, shorter);
					}
				}
				return result;
			}
            //... if addresses match, allow deletion to take place farther down the code
        }
		//else if searching by criteria only AND a confirmation function exists...
		else if (confirm && !confirm((*root)->dataPtr))  { 
			// ...and current data is disconfirmed as the correct data to delete...
			// ...first look on the left side...
			
			if ((result = _deleteDup(&(*root)->left, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter)))  {
				// ...if found and deleted on the left, rebalance the tree if necessary
				if (*shorter)  {
					dltRightBal(root, shorter);
				}
				//...and return the result
				return result;
			}
			// ...if not found on the left, check on the right...
			else if ((result = _deleteDup(&(*root)->right, dataPtr, compare, freeData, confirm, destroy, atAddress, shorter)))  {
				if(*shorter)  {
					dltLeftBal(root, shorter);
				}
				// ...and return the result
				return result;
			}
			else  {				//DISCONFIRMED BASE CASE
				// ...otherwise, data does not exist in the left subree or the right subtree and the root has been
				//disconfirmed as the appropriate node to delete, return failure. 
				return NULL;
			}
		}
		// In all other cases, the data matches, and is confirmed as the correct
		// data to delete, or no confirmation is necessary...
		// ...proceed with delete


        // test for leaf node
        dltPtr = *root;
    if (!(*root)->left)
    //No left subtree
    {
        *root = (*root)->right;
		*shorter = 1;
        if(destroy)  {
            freeData(dltPtr->dataPtr);
            holdPtr = dltPtr->dataPtr;
        }
        else  {
            holdPtr = dltPtr->dataPtr;
        }
        free(dltPtr);
        return holdPtr;  //SUCCESSFUL DELETE BASE CASE
    }
    else
        if (!(*root)->right)
        //Only left subtree
        {
            *root = (*root)->left;
			*shorter = 1;
            if(destroy)  {
                freeData(dltPtr->dataPtr);
                holdPtr = dltPtr->dataPtr;
            }
            else  {
                holdPtr = dltPtr->dataPtr;
            }
            free(dltPtr);
            return holdPtr;  //SUCCESSFUL DELETE BASE CASE
        }
        else
        //delete node has two subtrees
        {
            exchPtr = (*root)->left;
            //Find largest node on left subtree
            while(exchPtr->right)
                exchPtr = exchPtr->right;

            //Exchange data
            holdPtr = (*root)->dataPtr;
            (*root)->dataPtr = exchPtr->dataPtr;
            exchPtr->dataPtr = holdPtr;
            result = _deleteDup(&(*root)->left, exchPtr->dataPtr, compare, freeData, NULL, destroy, atAddress, shorter);
			if(*shorter)  {
				dltRightBal(root, shorter);
			}
			return result;
        }//else

}//_deleteDup


/****** dltRightBal *******************************************************************
	The tree is shorter after a deletion on the left branch.
	If necessary, balance the tree by rotating.
		PRE		tree is shorter
		POST	balance restored
***************************************************************************************/
static void dltRightBal(TREE_NODE **root, int *shorter)
{
//Local Declarations
	TREE_NODE *rightTree;
	TREE_NODE *leftTree;

//Statements
	switch((*root)->bal)
	{
	case LH:	//Deleted left -- now balanced
				(*root)->bal = EH;
				break;
	case EH:	//Now Right High
				(*root)->bal = RH;
				*shorter = 0;
				break;
	case RH:	//Right High - rotate left
				rightTree = (*root)->right;
				if (rightTree->bal == LH)  {
					//Double rotation required
					leftTree = rightTree->left;
					switch(leftTree->bal)
					{
					case LH:	(*root)->bal = EH;
								rightTree->bal = RH;
								break;
					case EH:	(*root)->bal = EH;
								rightTree->bal = EH;
								break;
					case RH:	(*root)->bal = LH;
								rightTree->bal = EH;
								break;
					}

					leftTree->bal = EH;

					//Rotate right then left
					rotateRight(&(*root)->right);
					rotateLeft(root);
				}//if rightTree->bal == LH
				else  {
					//single rotation only
					switch(rightTree->bal)
					{
					case LH:	//cannot occur
					case EH:	(*root)->bal = RH;
								rightTree->bal = LH;
								*shorter = 0;
								break;
					case RH:	(*root)->bal = EH;
								rightTree->bal = EH;
								break;
					}//switch rightTree->bal
					rotateLeft(root);
				}//else
	}//switch

	return;
}//deleteRightBalance


/****** dltLeftBal *******************************************************************
	The [sub]tree is shorter after a deletion on the right branch.
	If necessary, balance the tree by rotating.
		PRE		tree is shorter
		POST	balance restored
***************************************************************************************/
static void dltLeftBal(TREE_NODE **root, int *shorter)
{
//Local Declarations
	TREE_NODE *leftTree;
	TREE_NODE *rightTree;

//Statements
	switch((*root)->bal)
	{
	case LH:	//Left high - rotate Right
				leftTree = (*root)->left;
				if (leftTree->bal == RH)  {
					//Double rotation required
					rightTree = leftTree->right;
					switch(rightTree->bal)
					{
					case LH:	leftTree->bal = EH;
								(*root)->bal = RH;
								break;
					case EH:	leftTree->bal = EH;
								(*root)->bal = EH;
								break;
					case RH:	leftTree->bal = LH;
								(*root)->bal = EH;
								break;
					}

					rightTree->bal = EH;

					//rotate left, then right
					rotateLeft(&(*root)->left);
					rotateRight(root);
				}//if (leftTree->bal == RH)
				else  {
					//Single Rotation Only
					switch(leftTree->bal)
					{
					case LH:	leftTree->bal = EH;
								(*root)->bal = EH;
								break;
					case EH:	leftTree->bal = RH;
								(*root)->bal = LH;
								break;
					case RH:;	//cannot occur
					}// switch leftTree->bal
					rotateRight(root);
				}//else
				break;
	case EH:	//Now Left High
				(*root)->bal = LH;
				*shorter = 0;
				break;
	case RH:	//Deleted right -- now balanced
				(*root)->bal = EH;
				break;
	}//switch

	return;
}//deleteLeftBalance


/****** rotateRight ********************************************************************
	This function exchanges pointers to rotate the tree to right
		PRE		root points to tree to be rotated
		POST	node rotated and root updated
****************************************************************************************/
static void rotateRight(TREE_NODE **root)
{
//Local Declarations
	TREE_NODE *tempPtr;

//Statements
	tempPtr = (*root)->left;
	(*root)->left = tempPtr->right;
	tempPtr->right = (*root);

	*root = tempPtr;

	return;
}//rotateRight /*


/****** rotateLeft *********************************************************************
	This function exchanges pointers to rotate the tree to the left
		PRE		root points to tree to be rotated
		POST	node rotated and root updated
****************************************************************************************/
static void rotateLeft(TREE_NODE **root)
{
//Local Declarations
	TREE_NODE *tempPtr;

//Statements
	tempPtr = (*root)->right;
	(*root)->right = tempPtr->left;
	tempPtr->left = (*root);

	*root = tempPtr;

	return;
}//rotateLeft /*


/****** _retrieve **********************************************************************
	FUNCTION SPECIFIC TO DUPLICATE-REFUSING TREES
	Searches tree for nodes matching the criteria contained in target. When a matching
	node is found, it is stored in a TREE structure, matches, and the function returns 
	success (1) to the caller. If the function reaches a NULL node, the data is not in
	the tree and the function returns failure (0). Function always returns a TREE
	regardless of the number of results returned. This is so that the public function
	Retrieve can return multiple values  when called on a duplicate-allowing AVL tree.
        PRE     root is pointer to the root TREE_NODE of the current [sub]tree
                target is pointer to data structure containing key to be located
				compare is a pointer to the compare function used to navigate the tree
				matches is a pointer to the output TREE in which the matching results
				will be	stored 
        POST    tree searched; TREE containing matching data returned
        RETURN  Address of data in matching node
                If not found, NULL returned
****************************************************************************************/
static int _retrieve(TREE_NODE *root, void *target, TREE *tree)
{
//Local Declarations
	//int result;

//Statements
    if (root)
    {
        if (tree->compare(target, root->dataPtr) < 0)
			return _retrieve(root->left, target, tree);
        else if (tree->compare(target, root->dataPtr) > 0)
            return _retrieve(root->right, target, tree);
        else
            //found equal key
			enqueue(tree->searchResults, root->dataPtr);
		return 1;
    }//if root
    else
        //Data not in tree
        return 0;
}//_retrieve


/****** _retrieveDup **********************************************************************
	FUNCTION SPECIFIC TO DUPLICATE-ALLOWING TREES
	Searches tree for nodes matching the criteria contained in target. When a matching
	node is found, it is stored in a TREE structure, matches, success is set to 1, and the 
	fuction continues to search first the left subtree, then the right for further matches. 
	If the function reaches a NULL node, the data is not it returns failure (0). Successful
	calls further up the recursion stack will still return 1Function always returns a TREE
	regardless of the number of results returned. 
        PRE     root is pointer to the root TREE_NODE of the current [sub]tree
                target is pointer to data structure containing key to be located
				compare is a pointer to the compare function used to navigate the tree
				matches is a pointer to the output TREE in which the matching results
				will be	stored 
        POST    tree searched; TREE containing matching data returned
        RETURN  Address of data in matching node
                If not found, NULL returned
****************************************************************************************/
static int _retrieveDup(TREE_NODE *root, void *target, TREE *tree)
{
//Local Declarations
	int result;

//Statements
    if (root)
    {
        if (tree->compare(target, root->dataPtr) < 0)
			return _retrieveDup(root->left, target, tree);
        else if (tree->compare(target, root->dataPtr) > 0)
            return _retrieveDup(root->right, target,tree);
        else
            //found equal key
			result = _retrieveDup(root->left, target, tree);
			enqueue(tree->searchResults, root->dataPtr);
            result  |= _retrieveDup(root->right, target, tree);
			return result;
    }//if root
    else
        //Data not in tree
        return 0;
}//_retrieveDup


/****** _traverse *********************************************************************
    Inorder tree traversal. to process a node, we use the function passed when traversal
    was called.
        PRE     Tree has been created (may be null)
        POST    All nodes processed
***************************************************************************************/
static void _traverse (TREE_NODE *root, void (*process) (void *dataPtr))
{
//Statements
    if(root)
    {
        _traverse(root->left, process);
        process(root->dataPtr);
        _traverse(root->right, process);
    }
    return;
}//_traverse


/****** _printNested *************************************************************************
    internal printNested function - Inorder traversal of tree, printing each level with
    progressive indent
        PRE     tree is a valid BST containing data
                print is a pointer to a type-specific print function (application)
        POST    tree printed in hierarchical format
**********************************************************************************************/
static void _printNested(TREE_NODE *root, void (*print)(void *dataPtr), int showNums)
{
//Local Declarations
    static int level = 0;
	int currentLevel;
    int i;

//Statements

    if(root)  {
		currentLevel = ++level;
        _printNested(root->right, print, showNums);
        level = currentLevel;
		for (i = 1; i < currentLevel; i++)  {
            printf("   ");
		}
			if(showNums) printf("%d. ", currentLevel);
			//printf(" Bal: %d  ", root->bal);
			print(root->dataPtr);
        _printNested(root->left, print, showNums);
    }

    level = 0;
    return;
}//_printNested


/****** _filter *******************************************************************************
    internal process criteria-matched elements function. Traverse tree and process only those
    members that satisfy the compare function
        PRE     root is pointer to an element of the tree
                compare is a pointer to a type-specific compare function (supplied by application)
                process is a pointer to an applicateion-supplied function
				target is a pointer to target data to match
        POST    elements of tree satisfying compare function printed
**********************************************************************************************/
static int _filter  (TREE_NODE *root,
                     int  (*compare)(void *arg1, void *arg2),
                     void (*process)(void *arg1),
                     void *target,
                     int count)
{
//Statements
    if(root)  {
        count += _filter(root->left, compare, process, target, count);
        if(!compare(root->dataPtr, target))  {
            process(root->dataPtr);
            count++;
        }
        count += _filter(root->right, compare, process, target, count);
    }

    return count;
}//_filter


/****** _destroy *****************************************************************************
    Deletes all nodes in tree and recycles memory  using an inorder traversal. If destroy is
	true, also destroys data stored in nodes
        PRE     root is pointer to valid TREE_NODE
				destroyData is an enum destConst with two possible values DESTROY(1) or PRESERVE(0)
				freeData is a pointer to an application-supplied function to recycle stored data
					If stored data structures do not contain external dynamically allocated
					members, the standard free() function will suffice.
        POST    All TREE_NODEs recycled. If DESTROY, data also recycled.
        RETURN  null head pointer
**********************************************************************************************/
static void _destroy(TREE_NODE *root, enum destConst destroyData, void (*freeData)(void *dataPtr))
{
//Statements
    if(root)
    {
        _destroy(root->left, destroyData, freeData);
        if(destroyData) {
            freeData(root->dataPtr);
        }
        _destroy(root->right, destroyData, freeData);
        free(root);
		return;
    }//if
	return;
}//_destroy



/****** GetFirst *****************************************************************************
	Function walks down the left side of a tree until the first node without a left child is
	found and returns its data
		PRE		tree is a valid TREE structure.
		RETURN	left-most descendant -or- NULL if empty tree/null tree
**********************************************************************************************/
void *GetFirst (TREE *tree)
{
//Local Declarations
	TREE_NODE *current;

//Statements
	if(tree && tree->root)  {
		current = tree->root;
		while(current->left)  {
			current = current->left;
		}
		return current;
	}

	return NULL;
}//GetFirst


/****** GetLast ******************************************************************************
	Function walks down the right side of a tree until the first node without a right child is
	found and returns its data
		PRE		tree is a valid TREE structure.
		RETURN	right-most descendant -or- NULL if empty tree/null tree
**********************************************************************************************/
void *GetLast (TREE *tree)
{
//Local Declarations
	TREE_NODE *current;

//Statements
	if(tree && tree->root)  {
		current = tree->root;
		while(current->right)  {
			current = current->right;
		}
		return current->dataPtr;
	}

	return NULL;
}//GetLast

