/*	Header file for Hash. Contains
	   Written by:
	   Date:


    Change Log:
        _05/24 @4.30pm: All the functions should work.
*/
//#include <stdbool.h>
#include "linkListADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef enum { false, true} bool;

typedef struct
{
	 int   count;
	 int maxSize;
	 int  (*compare)(void* argu1, void* argu2);
	 int (*getHashKey)(void* argu1, int hashSize);
	 LIST**  hashList;
	 int longestList;
}HASH;

//	Prototype Declarations for public functions
    /**
        The compare function should look like this:

                    if(n1 > n2)
                        return 1;
                    else if(n1 < n2 )
                        return -1;
                    else
                        return 0;

        otherwise it won't work properly.
    */
	HASH* HASH_Create
	         (int (*getHashKey)(void* argu1, int hashSize),
              int (*compare) (void* argu1, void* argu2),
              int maxSize);                            //testing

	HASH* HASH_Destroy (HASH* pHash,
                        void (*process)(void **dataOut));     //added 05/24 @2.54 PM

	bool  HASH_Insert   (HASH* pHash, void* dataPtr);
	void* HASH_Delete   (HASH* pHash, void* dltKey);
	void* HASH_Retrieve (HASH* pHash, void* keyPtr);
	void  HASH_Traverse (HASH* pHash,
	          //          void (*process)(void* dataPtr, int index)); CHANGE JW removed index
			 		 	void (*process)(void* dataPtr));

	bool HASH_Empty (HASH* pHash);
	double HASH_Load  (HASH* pHash);
	int  HASH_Count (HASH* pHash);

    void  HASH_Testing (HASH* pHash,
				  void (*processIndex)(void *index),
                  void (*processData)(void* dataPtr));

    void  HASH_SaveFile (HASH* pHash,
				  char *fileName,
                  void (*processData)(FILE *fpOut, void* dataPtr));

    int HASH_GetLongestList(HASH *pHash);
/***********************  BRENDA's ***************************************/

void HASH_ReHash (HASH** pHash, int (*getPrime)(int));
	//	void (*process)(void* dataPtr));
