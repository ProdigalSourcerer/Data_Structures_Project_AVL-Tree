#include "team.h"

/************************************************************************************
 * PROGRAMMER: JESSIE WRIGHT
 *
 * The functions contained here deal with most of the application layer functions for the program.
 * processing the input file
 * 	- creating the hash table and trees (with calls to ADTs)
 * 	- deteriming initial hash size
 * Add/delete/search managers calling the ADT functions.
 * The compare delete and print functions passed to the hash and AVL adt's.
 * Getting and validating user input.
 * 
 * Also done by me: integration, main, team.h, and reports.
 *
 *
 **************************************************************************************/

/********************** input file processing ****************************************/

/******************************************************
 *  opens/closes main inputfile
 *  creates and populates hash, nameTree, and idTree
 *  exits on fatal failures with error messages
 *  ************************************************/
void setup(HASH** hash, TREE** nameTree, TREE** idTree, char* inFile)
{
	char buff[256];
	PRISONER *prisoner; 

	FILE* fp = fopen(inFile, "r");
	if(!fp){
		printf("\nerror opening input\n");
		exit(100);
	}
	*hash = HASH_Create(getHashKey, compareId, getPrime(getNumLinesInFile(fp) * 2) );
	if(!*hash) printf("\nHash wouldn't create\n"), exit(100);
	*nameTree = CreateTree(compareName, freePrisoner, NULL);
    if(!*nameTree) printf("\nNameTree wouldn't create \n"), exit(100);
	*idTree = CreateTree(compareId, freePrisoner, NULL);
	if(!*idTree) printf("\nIdTree wouldn't create\n"), exit(100);

	while(myGets(fp, buff, 256) != EOF){
		if(!(prisoner = createPrisoner(buff))) printf("error creating prisoner");    
		if(!addPrisoner(*hash, *nameTree, *idTree, prisoner)){
			printf("\nerror inserting:\n");
	   		printPrisoner(prisoner);
		}
	}
	fclose(fp);
}

void readFile(HASH** hash, TREE* nameTree, TREE* idTree, char* inFile)
{
	char buff[256];
	PRISONER *prisoner; 

	FILE* fp = fopen(inFile, "r");
	if(!fp){
		printf("\nerror opening input\n");
		exit(100);
	}
	
	while(myGets(fp, buff, 256) != EOF){
		if(!(prisoner = createPrisoner(buff))) printf("error creating prisoner");    
		if(HASH_Load(*hash) >= 75){
			HASH_ReHash(hash, getPrime);
		}
		if(!addPrisoner(*hash, nameTree, idTree, prisoner)){
			printf("\nerror inserting:\n");
	   		printPrisoner(prisoner);
		}
	}
	fclose(fp);
}

int getNumLinesInFile(FILE* fp)
{
	char temp[1000];
	int lineCount = 0;
	if(!fp) return 0;
	while(fgets(temp, 1000, fp)) lineCount++;
	rewind(fp);
	return lineCount;
}

/*********************  getting user input ******************************************/

/************************************
 * allocates newPrisoner and populates all fields with user or system supplied data, returns
 * pointer to Prisoner
 * **********************************/
PRISONER* getNewPrisoner(void)
{
	PRISONER* prisoner = xMalloc(sizeof(PRISONER));
	strcpy(prisoner->id, getPrisonerID());
	prisoner->lName = makeString(getName("last", 0));
	prisoner->fName = makeString(getName("first", 0));
	prisoner->crime = getCrime();
	time(&prisoner->admitDate); // writes current time to admitDate
	prisoner->projReleaseDate = prisoner->admitDate + getSentence();
	prisoner->cellBlock = getCellBlock();
	sprintf(prisoner->cell,"%d", getCellNum());
	return prisoner;
}


/*********************************
 * prompts user for prisoner id and validates (5 digit string) returns pointer
 * to static string which can be copied to final destination.  Value only persists
 * until next call to function.
 * ******************************/
char* getPrisonerID()
{
	static char id[6];
	char temp[MAX_NAME];
	int len, success = 0;

	while(!success){
		printf("\nEnter 5 digit prisoner ID: ");
		myGets(stdin, temp, MAX_NAME);
		len = strlen(temp);
		if(len == 5) {
		   	strcpy(id, temp);
			if(strspn(id, "0123456789") == 5){
		   		success = 1;
			} else	printf("\nInput contains non-digit\n");
		}else printf("\nInput too %s\n", len > 4 ? "long" : "short");
	}
	return id;
}

/**********************************************************
 * prompts user "enter [desc] name" validates, and returns
 * pointer to static char string, which may then be copied to final
 * destination.  string returned will only persist until next call
 * *****************************************************/
char* getName(char* nameDesc, int caseSensitive)
{
	static char name[MAX_NAME + 1];

	char temp[TEMP_STR + 1];
	int success = 0, i, len;

	while(!success){
		success = 1;
		printf("\nEnter %s name\n", nameDesc);
		myGets(stdin, temp, TEMP_STR + 1);
		len = strlen(temp);
		if (len > MAX_NAME) {
			printf("\nInput too long. %d is max length.", MAX_NAME);
			success = 0;
			continue;
		}
		for(i = 0; i < len ; i++){
			if( isalpha(temp[i])){
				if(!caseSensitive) name[i] = toupper(temp[i]);
				else name[i] = (temp[i]);
			} else{
			   printf("non-alpha char %c\n", temp[i]);
		   	   success = 0;
			   continue;
			}
		}	
		name[len] = '\0';
	}
	return name;
}



/**********************
 * prompts user for offence, validates and returns
 ************************/
crime_t getCrime(void)
{
	crime_t temp;
	char tempStr[TEMP_STR];
	int status;

	printf("\nOffences\n"
			"0) Arson\n"
			"1) Assault\n"
			"2) DUI\n"
			"3) Kidnapping\n"
			"4) Perjury\n"
			"5) Public Indecency\n"
			"6) Theft\n"
			"7) Vandalism\n"
			"--------------------\n\n");
	
	printf("Enter offence: ");
	do{
		status = 1;
		myGets(stdin, tempStr, TEMP_STR); 
		if(strlen(tempStr) > 1 || sscanf(tempStr, "%d", (int*) &temp) != 1 || temp < 0 || temp > 7){
			printf("\nInvalid entry.  Please re-enter.");
			status = 0;
		}
	}while(!status);
	return temp;
}


/****************************
prompts user for sentence in days, validates and returns
sentence in seconds
****************************/
time_t getSentence(void)
{
	int sentence;
	char tempStr[TEMP_STR];
	int valid = 0;

	printf("\nEnter the sentance in days: ");
	while(!valid){
		myGets(stdin, tempStr, TEMP_STR);
		if(!sscanf(tempStr, "%d", &sentence)){
			printf("\nInvalid input, whole number of days only ");
		}
		else if( sentence < 3 ){
			printf("Minimum sentence 3 days\n");
		}
		else valid = 1;
	}
	return (time_t) (sentence * 24 * 60 * 60);
}

/********************************************
 * prompts user for cell block (valid values a-z, case insensitive) validates
 * (reprompting until successful) and returns (uppercase)
 * *****************************************/
char getCellBlock(void)
{
   	int	status;
	char tempStr[TEMP_STR], block;
	printf("\nEnter cell block [A-Z] ");
	do{
		status = 1;
		myGets(stdin, tempStr, TEMP_STR); 
		if( strlen(tempStr) != 1 || sscanf(tempStr, "%c", &block) != 1 || !(isalpha(block))){
			printf("\nInvalid entry.  Please re-enter.");
			status = 0;
		}
	}while(!status);
	return toupper(block);

}

/********************************************
 * prompts user for cell num (valid values 100-999 int) validates
 * (reprompting until successful) and returns integer value
 * *****************************************/
int getCellNum(void)
{
	int cell, status;
	char tempStr[TEMP_STR];
	printf("\nEnter cell number [valid range 100-999]: ");
	do{
		status = 1;
		myGets(stdin, tempStr, TEMP_STR); 
		if( strlen(tempStr) != 3 || sscanf(tempStr, "%d", &cell) != 1 || cell < 100 || cell > 999){
			printf("\nInvalid entry.  Please re-enter.");
			status = 0;
		}
	}while(!status);
	return cell;
}

/********************
 * prints welcome message:
 * ******************/
void printWelcome(void)
{
	printf("\nWelcome to the Toon-Town Prisoner Tracker\n"
			"This database tracks the offences, sentences and cells of Toon offenders.\n"
			"The current inhabitants of our prison are loaded when we fire up the program.\n"
			"You can then add, delete, and search the database.\n"
			"Thanks for helping us keep our streets clean.\n");
}

int getMenuChoice(int n, ...)
{
	int i, choice, success;
	char* temp = (char*)calloc(n, sizeof(char));
	char* endp;
	va_list argp;
	va_start(argp, n);
	
	printf(	"\nMenu Options\n");
	for(i = 1; i <= n ; i++){
		printf(	"%d) %s\n", i, (char*)va_arg(argp, char*));
	}
	printf("-----------------------\n");
	do{
		printf("Enter choice: ");
		success = 1;
		myGets(stdin, temp, n);
		choice = (int) strtol(temp, &endp, 10);
		if(*endp != 0 || choice < 1 || choice > n){
			printf("Invalid Option!\n");
			success = 0;
		}
	}while(!success);
	
	va_end(argp);
	
	return choice;
}



	

void addManager(HASH** hash, TREE* nameTree, TREE* idTree)
{
	PRISONER* prisoner;
		
	switch(getMenuChoice(2, "Add batch from file", "Add individual")){
			case 1: readFile(hash, nameTree, idTree, getName("input file", 1));
					break;	
			case 2: prisoner = getNewPrisoner();
					if(HASH_Load(*hash) >= 75){
						HASH_ReHash(hash, getPrime);
					}
					if(!addPrisoner(*hash, nameTree, idTree, prisoner)){
						printf("\nerror inserting:\n");
						printPrisoner(prisoner);
						putchar('\n');
					}
					break;
	}
}

void deleteManager(HASH* hash, TREE* nameTree, TREE* idTree)
{
	PRISONER temp;
	PRISONER* toDel = HASH_Retrieve(hash, &temp);
	memset(&temp, 0, sizeof(PRISONER)); //to clear any junk since whole record won't be populated
	
	switch(getMenuChoice(3, "Delete by ID", "Delete by Name", "Return to Menu")){
		case 1: strcpy(temp.id, getPrisonerID());
				if(!(toDel = HASH_Retrieve(hash, &temp))){
					printf("\nPrisoner %s not found.  Cannot delete. \n", temp.id);
					return;
				}
				if(!deleteConfirm(toDel)) return;
				break;

		case 2: 

				temp.lName = makeString(getName("last", 0));
				temp.fName = makeString(getName("first", 0));
				printf("last: %s first: %s \n", temp.lName, temp.fName);
				if(Search(nameTree, &temp)){
					while((toDel = GetNextResult(nameTree)) && !deleteConfirm(toDel)) ;
					FlushSearch(nameTree);
					free(temp.lName);
					free(temp.fName);
					if(!toDel) return;
				}
				else{
					printf("\nPrisoner %s, %s not found. Cannot delete.\n", temp.lName, temp.fName);
					free(temp.lName);
					free(temp.fName);
					return;
				}
				break;
		case 3: return;
	}
	
	if(! HASH_Delete(hash, toDel)) printf("Couldn't delete from hash");
	if(! DeleteAt(idTree, toDel, PRESERVE)) printf("Couldn't delte from ID tree\n");//T1 delete, t2 ->n-1 deleteAt pres, tn delete at destroy.
	if(! DeleteAt(nameTree, toDel, DESTROY)) printf("Couldn't delete from nameTree\n");
	return;
}


void searchManager(HASH* hash, TREE* nameTree)
{	
	PRISONER temp, *result;
	int count;
	memset(&temp, 0 , sizeof(PRISONER));
	
	switch(getMenuChoice(3, "Search by ID", "Search by Name", "Return to Menu")){
		case 1:	strcpy(temp.id, getPrisonerID()); //by ID hash 
 				result = HASH_Retrieve(hash, &temp);
				if(result) printPrisoner(result);
				else printf("\nRecord not found for %s \n", temp.id);
				break;
		case 2: temp.lName = makeString(getName("last", 0));		//by name tree
				temp.fName = makeString(getName("first", 0));
				if((count = Search(nameTree, &temp)))  {
					printf("\n\n%d matching prisoners: \n\n", count);
					while((result = (PRISONER*) GetNextResult(nameTree)))  {
						printPrisoner(result);
					}
				}
				else printf("\nRecord not found for %s, %s \n", temp.lName, temp.fName);
				free(temp.lName);
				free(temp.fName);
				break;
						
		case 3:	break;				//back to main
	}

}

int addPrisoner(HASH* hash, TREE* nameTree, TREE* idTree, PRISONER* prisoner)
{
	int hashStatus;

	hashStatus = HASH_Insert(hash, prisoner); 
	if(hashStatus == 1){
	printf("\nadd fail hash\n");
		return 0;
	}
	if(hashStatus == -1){
		printf("%s a duplicate id, unique id's are required.\n", prisoner->id);
		return 0;
	}
	if(!Insert(nameTree, prisoner)){
	printf("\n add fail nameTree\n");
		if(!HASH_Delete(hash, prisoner)) printf("WTF Impossible addP del 1\n");
		return 0;
	}
	if(!Insert(idTree, prisoner)){
	printf("\n add fail idTree\n");
		if(!HASH_Delete(hash, prisoner)) printf("WTF Impossible addP del 2\n");
		if(!DeleteAt(nameTree, prisoner, PRESERVE)) printf("WTF Impossible addP del 3\n");
		return 0;
	}
	return 1;
}



PRISONER* createPrisoner(char* string)
{
	char tempF[MAX_NAME], tempL[MAX_NAME];
	PRISONER* newRecord = (PRISONER*) malloc(sizeof(PRISONER));
	if(!newRecord) return NULL;
	sscanf(string, "%[^;];%[^,],%[^;];%d;%lld;%lld;%c;%s", newRecord->id, tempL, tempF, (int*) &newRecord->crime, 
			(long long*) &newRecord->admitDate, (long long*) &newRecord->projReleaseDate, &newRecord->cellBlock, newRecord->cell);	
	newRecord->fName = makeString(tempF);
	newRecord->lName = makeString(tempL);	
	
	return newRecord;
}

void printPrisoner(void* record)
{
	PRISONER* prisoner = (PRISONER*) record;
	char date[27];
	strcpy(date, ctime(&prisoner->admitDate)); // subsequent calls to ctime cause unpredictable behavior (likely returning
												// ptr to static string, so one date stored temp, other call nn scanf 	
	printf(	"NAME:              %s, %s\n"
			"ID:                %s\n"
			"OFFENSE:           %s\n"
			"ADMISSION:         %s"
			"PROJECTED RELEASE: %s"
			"CELL:              %c %s\n",
			prisoner->lName, prisoner->fName, prisoner->id, crimeToString(prisoner->crime), date,
		   	ctime(&(prisoner->projReleaseDate)), prisoner->cellBlock, prisoner->cell);
	putchar('\n');
}

void printPrisonerBrief(void* record)
{
	printf("ID: %s  NAME: %s, %s\n", ((PRISONER*)record)->id, ((PRISONER*)record)->lName, ((PRISONER*)record)->fName);			
}

const char* crimeToString(crime_t crime)
{
	switch(crime){
		case ARSON: 	return "ARSON";
		case ASSAULT:	return "ASSAULT";
		case DUI:		return "DUI";
		case FRAUD:		return "FRAUD";
		case KIDNAPPING: return "KIDNAPPING";
		case PERJURY: return "PERJURY";
		case PUBLIC_INDECENCY: return "PUBLIC_INDECENCY";
		case THEFT:		return "THEFT";
		case VANDALISM: return "VANDALISM";
		default: return "OTHER";		
	}
}						 	






/**************************************
 *NAME:                   makeString
 *DESC:          dynamically allocates memory for string(exact size), copies string
 *                               to location and returns pointer to string
 *ARGS:          str - string to be stored
 *RETURN:        pointer to string
 *OTHER
 *EFFECTS:
 *NOTES:         if memory not succesfully allocated return value will be NULL
 ***************************************/

char* makeString(const char str[])
{
	char* strloc; 
	int strLen=strlen(str); 

	strloc=(char*)xMalloc(strLen+1);
	strcpy(strloc, str);
	return strloc;
} // end makeString


/*************************************************
 *NAME:  strlcmp
 *DESC:  compares two strings, returns their difference
 *ARGS:   str1 - pointer to first string
 *                       str2 pointer to second string
 *RETURN:        difference in strings (0 means same,
 *                       <0 means str 1 comes first in alphabetical order,
 *                       >0 means str 2 comes first in alphabetical order
 *OTHER
 *EfFECTS:
 *NOTES:
 *************************************************/
int strlcmp(const char *str1, const char * str2)
{
	while( tolower(*str1) == tolower(*str2)   && *str1!='\0' ){
		++str1;
		++str2;
	} // end while
	return tolower(*str1)- tolower(*str2);
} // end strlcmp

/**********************************************
 * wrapper for fgets that replaces '\n' or '\r' with '\0'
 * and flushes buffer if maxSize exceeded before newline
 * takes same args as fgets (fp to stream, dest string,
 * max chars to read)
  *****************************************/
int myGets(FILE* fp, char str[], int maxSize)
{
	int len;
	char ch;

	if(fgets(str, maxSize, fp)== NULL) return EOF;
	len=strlen(str);
	if(str[len-1]=='\n' || str[len-1] == '\r'){
		str[len-1]='\0';
		len--;
	}
	else while((ch=fgetc(fp)) != '\n' /*&& ch != '\r'*/);//flush

	return len;
} 






/*******************
 * TODO: sort out wrappers
 * ****************/

void *xMalloc(size_t size)
{
    void *ptr;

    if ((ptr = (void *) malloc(size)) == NULL)
    {
        printf("Malloc error");
        exit(1);
    }
    return ptr;
}



/*
	token = strtok(buff,";");  //TODO error checking in this section
	strcpy(sample->id, token);
	token = strtok(NULL,";");
	sscanf(token, "%[^,],%s", tempL, tempF);
	sample->lName = makeString(tempL);
	sample->fName = makeString(tempF);
	token = strtok(NULL, ";");
	sample->crime = stringToCrime(token);
	token = strtok(NULL, ";");
	sample->admitDate = (time_t) strtoll(token, endp, 10);		
	token = strtok(NULL, ";");
	sample->projReleaseDate = (time_t) strtoll(token, endp, 10);
	token = strtok(NULL, ";");
	sample->cellBlock = *token;
	token = strtok(NULL, ";");
	str`sample->cell

*/


void freePrisoner(void *record)
{
	free(((PRISONER*)record)->fName);
	free(((PRISONER*)record)->lName);
	free(record);
}

int compareId(void* arg1, void* arg2)
{
	int firstArg, secondArg;
	firstArg = (int) strtol(((PRISONER*)arg1)->id, NULL, 10); //TODO error checking on strtol
	secondArg = (int) strtol(((PRISONER*)arg2)->id, NULL, 10);
	if(firstArg == secondArg) return 0;
	if(firstArg < secondArg) return -1;
	return 1;

}

int compareName(void* arg1, void* arg2)
{
	int compare = strlcmp(((PRISONER*)arg1)->lName, ((PRISONER*)arg2)->lName);
		if(!compare){ //last name match
			compare = strlcmp(((PRISONER*)arg1)->fName, ((PRISONER*)arg2)->fName);
		}
	return compare;
}



int deleteConfirm(void* data)
{
	printf("Prisoner found:\n");
	printPrisoner(data);
	return yesNo("Do you want to delete this prisoner? ");
}
