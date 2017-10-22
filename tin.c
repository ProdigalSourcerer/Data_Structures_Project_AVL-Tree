#include "team.h"


/***TIn_Testing_F/unction_Definitions**/
/***

    pre: index;
    post: prints the index
*/
void printIndex (void *index)
{
    printf("Hash table @ index: %d\n", *((int *)index));
    return;
}

/***
    This function takes the file pointer from the ADT and write to that file the format as we want.
    pre:
    post: 1 or 0;
*/
void writeFile(FILE *fpOut, void *dataPtr)
{
    PRISONER *scoopyDoo = (PRISONER *)dataPtr;
    fprintf(fpOut, "%s;%s,%s;%d;%lld;%lld;%c;%s\n", scoopyDoo->id,
                                            scoopyDoo->lName,
                                            scoopyDoo->fName,
                                            scoopyDoo->crime,
                                            (long long)scoopyDoo->admitDate,
                                            (long long)scoopyDoo->projReleaseDate,
                                            scoopyDoo->cellBlock, scoopyDoo->cell);
    printf("Successfully 1 record to file!\n");
//	return 1;
}
/***
    This function
    pre:
    post:
*/
void printEfficiency(HASH *pHash)
{
    printf("\nLongest List: %d", HASH_GetLongestList(pHash));
    printf("\nTotal records: %d", HASH_Count(pHash));
    printf("\nHash loads:    %.2f%%\n", HASH_Load(pHash));

    return;
}
/***END_OF_TIn_Testing_Function_Definitions**/
