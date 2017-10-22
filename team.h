#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include "hashADT.h"
#include "AVL_ADT.h"

typedef enum	{ARSON, ASSAULT, DUI, FRAUD, KIDNAPPING,
				PERJURY, PUBLIC_INDECENCY, THEFT, VANDALISM}
				crime_t; // if altering, be sure to update getCrime function and crimeToString 



typedef struct{
	char id[6];
	char *fName;
	char *lName;
	crime_t crime;
	time_t	admitDate;				// more displays than calculations arguably?  decision re  time_t or tm struct storage;
	time_t	projReleaseDate;
	char cellBlock;
	char cell[4];
}PRISONER;


#define MAX_NAME 20
#define TEMP_STR 256
#define FLUSH while(getchar() != '\n')

void setup(HASH** hash, TREE** nameTree, TREE** idTree, char* inFile);
// i/o functions


int getMenuChoice(int n, ...);
int getNumLinesInFile(FILE* fp);
void readFile(HASH** hash, TREE* nameTree, TREE* idTree, char* inFile);
int strlcmp(const char *str1, const char * str2);
int myGets(FILE* fp, char str[], int maxSize);
crime_t getCrime(void);
char* makeString(const char str[]);
const char* crimeToString(crime_t crime);
int getMainMenuChoice(void);
void searchManager(HASH* hash, TREE* nameTree);
int getSearchMenuChoice(void);
void deleteManager(HASH* hash, TREE* nameTree, TREE* idTree);
int addPrisoner(HASH* hash, TREE* nameTree, TREE* idTree, PRISONER* prisoner);
void addManager(HASH** hash, TREE* nameTree, TREE* idTree);
char* getPrisonerID();
int getCellNum(void);
char getCellBlock(void);
char* getName(char* nameDesc, int caseSensitive);
void printWelcome(void);
time_t getSentence(void);
PRISONER* createPrisoner(char* string);
int yesNo(char *prompt);
/// functions passable to ADT
void printPrisoner(void* record);
int compareId(void* arg1, void* arg2);
void freePrisoner(void *record);
int compareName(void* arg1, void* arg2);
void printPrisonerBrief(void* record);
int deleteConfirm(void* data);

//wrappers
//
void *xMalloc(size_t size);
/*************Tin_Testing_Function***********/
void printIndex (void *index);
void writeFile(FILE *fpOut, void *dataPtr);
void printEfficiency(HASH *pHash);

/*************** Brenda **********************/

void cleanUp(HASH** hash, TREE** nameTree, TREE** idTree);
int getHashKey(void *record, int hashSize); 
int getPrime(int x);
