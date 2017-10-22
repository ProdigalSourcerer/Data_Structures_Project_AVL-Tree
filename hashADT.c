/*
    This file contains the definitons of the functions to maintain
	and process a BST:

    Public Functions:
		HASH_Create
		HASH_Destroy
		HASH_Insert
		HASH_Delete
		HASH_Retrieve
		HASH_Traverse
		HASH_Empty
		HASH_Load
		HASH_Count

    Change Log:
        _05/24 @4.30pm: All the functions should work.
*/

#include <stdlib.h>
#include "hashADT.h"
/**	================= HASH_Create ================
	   Pre
	   Post
*/
HASH* HASH_Create(int (*getHashKey)(void* argu1, int hashSize),
                    int (*compare) (void* argu1, void* argu2),
                    int maxSize)
{
    int i;

    HASH *pTemp = NULL;
    pTemp = (HASH *)malloc(sizeof(HASH));
    if(pTemp){
        pTemp->count = 0;
        pTemp->maxSize = maxSize;
        pTemp->compare = compare;
        pTemp->getHashKey = getHashKey;
        pTemp->longestList = 0;
        pTemp->hashList = (LIST **)malloc(maxSize * sizeof(LIST *));
        if(pTemp->hashList){
            for(i = 0; i < maxSize; i++){
                pTemp->hashList[i] = createList(compare);
            }
        }

    }

    return pTemp;
}
/**	================= BST_Create ================
	   Pre
	   Post: _Destroy the nodes only, the data will not be touched.
			 _If you want the data to be destroy you need to have a function to destroy it and uncomment the						line 284 in linkedListADT.c
*/
HASH* HASH_Destroy (HASH* pHash,
                    void (*process)(void **dataOut))
{
    int i;
    for(i = 0; i < pHash->maxSize; i++){
            destroyList(pHash->hashList[i], process);
    }

    free(pHash->hashList);
    free(pHash);

    return NULL;
}
/**	================= HASH_Insert ================
	   Pre
	   Post: 0  - successful.
             1  - failed.                      //CHANGE request JW return 0 if success, 1 -1 both fail right?i/
             -1 - if dup key.
*/
bool  HASH_Insert(HASH* pHash, void* dataPtr)
{
    int hashKey = 0;
    int success = 0;  //status  addNode +1 if dupe, -1 if other fail 0 if succest

    //hashKey = getHashKey(dataPtr);
	hashKey = pHash->getHashKey(dataPtr, pHash->maxSize);  //jw edited from line above
    success = addNode(pHash->hashList[hashKey], dataPtr);
    if(!(success)){ //
        (pHash->count)++;
        HASH_GetLongestList(pHash);
         return 0;
    }else if(success == 1)
        return -1;
    else
	   	  return 1;


}
/**	================= HASH_Delete ================
	   Pre
	   Post: _return NULL if data not found.
             _return dataOutPtr if found and the node in the linked-list is deleted.
*/
void* HASH_Delete(HASH* pHash, void* dltKey)
{
    void *dataOutPtr = NULL;
    int hashKey = 0;

    hashKey = pHash->getHashKey(dltKey, pHash->maxSize);
    if(!(emptyList(pHash->hashList[hashKey]))){
        if(removeNode(pHash->hashList[hashKey], dltKey, &dataOutPtr))
            return dataOutPtr;
    }
    HASH_GetLongestList(pHash);
    return NULL;
}
/**	================= HASH_Retrieve ================
	   Pre
	   Post: get the node by the given target
*/
void* HASH_Retrieve (HASH* pHash, void* keyPtr)
{
    void *dataOutPtr = NULL;
    int hashKey = 0;

    hashKey = pHash->getHashKey(keyPtr, pHash->maxSize);
    if(!(emptyList(pHash->hashList[hashKey]))){
        if(retrieveNode(pHash->hashList[hashKey], keyPtr, &dataOutPtr))
            return dataOutPtr;
    }
    return NULL;
}
/**	=================  HASH_Traverse ================
	   Pre
	   Post: traver the whole hash table.
*/
void  HASH_Traverse (HASH* pHash,
                  //  void (*process)(void* dataPtr, int index)) CHANGE JW removed int index requirement
				  void (*process)(void* dataPtr)) //CHANGE JW
{
    int i;
    for(i = 0; i < pHash->maxSize; i++){
        if(!emptyList(pHash->hashList[i])){
           traverseMod(pHash->hashList[i], process);
        }
    }
    return;
}

/**	================= HASH_Empty ================
	   Pre
	   Post:_true if the HASH table is empty
            _false if the HASH table is not empty
*/
bool HASH_Empty (HASH* pHash)
{
    return !(pHash->count);
}
/**	================= HASH_Load ================
	   Pre
	   Post _ return the percentage of the HASH table load.
*/
double HASH_Load  (HASH* pHash)
{
    int i, cnt;
    for(i = 0, cnt = 0; i < pHash->maxSize; i++){
        if(!emptyList(pHash->hashList[i])){
            cnt++;
        }
    }
    return((double)(cnt)/(double)(pHash->maxSize)) * 100.;
}
/**	================= HASH_Count ================
	   Pre
	   Post: _ return all the elements in the hash table.
*/
int  HASH_Count (HASH* pHash)
{
    return (pHash->count);
}


/**	=================  HASH_Tesing ================
	   Pre
	   Post
*/
void  HASH_Testing (HASH* pHash,
				  void (*processIndex)(void *index),
                  void (*processData)(void* dataPtr))
{
    int i;
    for(i = 0; i < pHash->maxSize; i++){
        if(!emptyList(pHash->hashList[i])){
			processIndex(&i);
        	traverseMod(pHash->hashList[i], processData);
		}
    }
    return;
}

/**	=================  HASH_SaveFile ================
	   Pre
	   Post
*/
void  HASH_SaveFile (HASH* pHash,
				  char *fileName,
                  void (*processData)(FILE *fpOut, void* dataPtr))
{
    FILE *fpOut = NULL;
    fpOut = fopen(fileName, "w");
    NODE *pNode = NULL;
    int i;
    for(i = 0; i < pHash->maxSize; i++){
        if(!emptyList(pHash->hashList[i])){
           pNode = pHash->hashList[i]->head;
           while(pNode){
            processData(fpOut, pNode->dataPtr);
            pNode = pNode->link;
           }
        }
    }
    fclose(fpOut);
    return;
}
/**	=================  HASH_GetLongestList ================
	   Pre
	   Post
*/
int HASH_GetLongestList(HASH *pHash)
{
    int longestList = 0;
    int temp = 0;
    int i;
    for(i = 0; i < pHash->maxSize; i++){
        if(!emptyList(pHash->hashList[i]))
            if((temp = listCount(pHash->hashList[i])) > longestList){
                longestList = temp;
        }
    }

    return pHash->longestList = longestList;
}




/************************************BRENDA************************************************/

/****************************************************************
 * This function takes the address of a pointer to a hash table 
 * creates a new hash table of larger size (two times the size of prior
 * then next prime, populates with member of original hash,
 * destroys original hash, and assigns address of new hash to prior hash
 * pointer.
 *
 * Pre: hash populated
 * Post: new hash allocated and populated, old hash destroyed
 * ***********************************************************/



void HASH_ReHash (HASH** pHash, int (*getPrime)(int))
{
	HASH* tHash;
	void* dataPtr;
	int i, hashSize = getPrime((*pHash)->maxSize * 2);  //TODO next prime

	tHash = HASH_Create((*pHash)->getHashKey, (*pHash)->compare, hashSize);

	for (i = 0; i < (*pHash)->maxSize; i++)
	{
		while (!emptyList((*pHash)->hashList[i]))
		{
			//	dataPtr      -> data to be popped off and put in new hashlist
			removeNode((*pHash)->hashList[i], (*pHash)->hashList[i]->head->dataPtr, &dataPtr);
			HASH_Insert(tHash, dataPtr);
		}
	}
	HASH_Destroy(*pHash, NULL);
	*pHash = tHash;
	return;
}












