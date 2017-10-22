/*
    This header file contains the functions to maintain
	and process a linked list.
*/

//  Singly-Linked List ADT Type Definitions

typedef struct node
{
	void*        dataPtr;
	struct node* link;
} NODE;

typedef struct
{
	int   count;
	NODE* pos;
	NODE* head;
	NODE* rear;
	int    (*compare) (void* argu1, void* argu2);
} LIST;

//  List ADT Prototype Declarations: public functions
	LIST* createList   (int (*compare)
	                   (void* argu1, void* argu2));

	LIST* destroyList  (LIST* list, void (*process)(void **dataOut));

	int   addNode   (LIST* pList, void* dataInPtr);

	int   removeNode   (LIST*  pList,
	                    void*  keyPtr,
	                    void** dataOutPtr);

	int   searchList   (LIST*  pList,
	                    void*  pArgu,
	                    void** pDataOut);

	int   retrieveNode (LIST*  pList,
	                    void*  pArgu,
	                    void** dataOutPtr);

	int   traverse     (LIST*  pList,
	                    int    fromWhere,
	                    void** dataOutPtr);

	int   listCount    (LIST*  pList);
	int   emptyList    (LIST*  pList);
	int   fullList     (LIST*  pList);


    int traverseMod (LIST*  pList,
              void (*process)(void *dataPtr));
